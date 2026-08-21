/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * USB Lifecycle and Device Matching Implementation
 */

#include "AIC8800_Driver.h"
#include "AIC8800_USB.iig"

// ============================================================================
// USB Device Matching
// ============================================================================

// Device matching dictionary
static const IOUSBHostMatchingDescriptor AIC8800_USB_Matching = {
    .bcdDevice = 0x0200,
    .bDeviceClass = 0xFF,  // Vendor-specific
    .bDeviceSubClass = 0xFF,
    .bDeviceProtocol = 0xFF,
    .idVendor = AIC8800_VENDOR_ID,
    .idProduct = 0,  // Will be set dynamically
    .bNumConfigurations = 1
};

// ============================================================================
// Driver Lifecycle
// ============================================================================

kern_return_t AIC8800_USB::Start(IOService *provider)
{
    kern_return_t result;

    // Call super
    result = IOUSBHostDevice::Start(provider);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Failed to start USB device driver\n");
        return result;
    }

    // Get USB device and interface
    usb_device = OSDynamicCast(IOUSBHostDevice, provider);
    if (!usb_device) {
        IOLog("AIC8800: Provider is not a USB device\n");
        return kIOReturnBadArgument;
    }

    // Get the first interface
    result = usb_device->GetFirstInterface(&usb_interface);
    if (result != kIOReturnSuccess || !usb_interface) {
        IOLog("AIC8800: Failed to get USB interface\n");
        return result;
    }

    // Get interface number
    const IOUSBHostInterfaceDescriptor *desc = usb_interface->GetInterfaceDescriptor();
    if (desc) {
        interface_number = desc->bInterfaceNumber;
    }

    // Initialize driver data
    result = InitwithDevice(usb_device, usb_interface);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Failed to initialize driver data\n");
        return result;
    }

    // Register the driver
    RegisterService();

    IOLog("AIC8800: USB driver started successfully\n");
    return kIOReturnSuccess;
}

kern_return_t AIC8800_USB::Stop(IOService *provider)
{
    IOLog("AIC8800: Stopping USB driver\n");

    // Cleanup resources
    if (bulk_in_pipe) {
        bulk_in_pipe->Release();
        bulk_in_pipe = nullptr;
    }
    if (bulk_out_pipe) {
        bulk_out_pipe->Release();
        bulk_out_pipe = nullptr;
    }
    if (interrupt_pipe) {
        interrupt_pipe->Release();
        interrupt_pipe = nullptr;
    }

    return IOUSBHostDevice::Stop(provider);
}

kern_return_t AIC8800_USB::Free()
{
    IOLog("AIC8800: Freeing driver resources\n");

    // Free buffers
    if (tx_buffer) {
        tx_buffer->Release();
        tx_buffer = nullptr;
    }
    if (rx_buffer) {
        rx_buffer->Release();
        rx_buffer = nullptr;
    }

    // Free lock
    if (lock) {
        IOLockFree(lock);
        lock = nullptr;
    }

    return IOUSBHostDevice::Free();
}

// ============================================================================
// Device Initialization
// ============================================================================

kern_return_t AIC8800_USB::InitwithDevice(IOUSBHostDevice *device,
                                           IOUSBHostInterface *interface)
{
    kern_return_t result;

    // Create synchronization lock
    lock = IOLockAlloc();
    if (!lock) {
        IOLog("AIC8800: Failed to allocate lock\n");
        return kIOReturnNoMemory;
    }

    // Get USB endpoints
    const IOUSBHostEndpointDescriptor *ep_desc;
    ep_desc = interface->FindNextEndpoint(nullptr, nullptr);
    while (ep_desc) {
        if (ep_desc->bmAttributes == AIC8800_USB_ENDPOINT_BULK) {
            if (ep_desc->bEndpointAddress & 0x80) {
                // Bulk IN endpoint
                result = interface->GetPipeObj(ep_desc->bEndpointAddress,
                                               &bulk_in_pipe);
                if (result != kIOReturnSuccess) {
                    IOLog("AIC8800: Failed to get bulk IN pipe\n");
                    return result;
                }
            } else {
                // Bulk OUT endpoint
                result = interface->GetPipeObj(ep_desc->bEndpointAddress,
                                               &bulk_out_pipe);
                if (result != kIOReturnSuccess) {
                    IOLog("AIC8800: Failed to get bulk OUT pipe\n");
                    return result;
                }
            }
        } else if (ep_desc->bmAttributes == AIC8800_USB_ENDPOINT_INTERRUPT) {
            // Interrupt endpoint
            result = interface->GetPipeObj(ep_desc->bEndpointAddress,
                                           &interrupt_pipe);
            if (result != kIOReturnSuccess) {
                IOLog("AIC8800: Failed to get interrupt pipe\n");
                return result;
            }
        }
        ep_desc = interface->FindNextEndpoint(ep_desc, nullptr);
    }

    // Allocate TX/RX buffers
    tx_buffer = IOBufferMemoryDescriptor::withBytes(nullptr,
        AIC8800_TX_DESC_SIZE * AIC8800_MAX_TX_QUEUES,
        kIODirectionOut, false);
    if (!tx_buffer) {
        IOLog("AIC8800: Failed to allocate TX buffer\n");
        return kIOReturnNoMemory;
    }

    rx_buffer = IOBufferMemoryDescriptor::withBytes(nullptr,
        AIC8800_RX_DESC_SIZE * 32,
        kIODirectionIn, false);
    if (!rx_buffer) {
        IOLog("AIC8800: Failed to allocate RX buffer\n");
        return kIOReturnNoMemory;
    }

    // Map buffer addresses
    tx_desc_ring = (uint8_t *)tx_buffer->getBytesNoCopy();
    rx_desc_ring = (uint8_t *)rx_buffer->getBytesNoCopy();

    // Initialize state
    state = AIC8800_STATE_IDLE;
    firmware_loaded = false;
    interface_active = false;

    IOLog("AIC8800: Driver data initialized\n");
    return kIOReturnSuccess;
}

// ============================================================================
// Register I/O Operations
// ============================================================================

kern_return_t AIC8800_USB::ReadRegister(uint16_t address, uint32_t *value)
{
    if (!value) return kIOReturnBadArgument;

    IOLockLock(lock);

    uint32_t data = 0;
    kern_return_t result = SendControlTransfer(
        AIC8800_USB_REQ_READ_REG,
        address,
        0,
        &data,
        sizeof(data),
        true
    );

    if (result == kIOReturnSuccess) {
        *value = data;
    }

    IOLockUnlock(lock);
    return result;
}

kern_return_t AIC8800_USB::WriteRegister(uint16_t address, uint32_t value)
{
    IOLockLock(lock);

    kern_return_t result = SendControlTransfer(
        AIC8800_USB_REQ_WRITE_REG,
        address,
        0,
        &value,
        sizeof(value),
        false
    );

    IOLockUnlock(lock);
    return result;
}

kern_return_t AIC8800_USB::ReadMemory(uint16_t address, void *buffer,
                                       uint32_t length)
{
    if (!buffer || length == 0) return kIOReturnBadArgument;

    IOLockLock(lock);

    kern_return_t result = SendControlTransfer(
        AIC8800_USB_REQ_READ_MEM,
        address,
        0,
        buffer,
        length,
        true
    );

    IOLockUnlock(lock);
    return result;
}

kern_return_t AIC8800_USB::WriteMemory(uint16_t address, const void *buffer,
                                        uint32_t length)
{
    if (!buffer || length == 0) return kIOReturnBadArgument;

    IOLockLock(lock);

    kern_return_t result = SendControlTransfer(
        AIC8800_USB_REQ_WRITE_MEM,
        address,
        0,
        (void *)buffer,
        length,
        false
    );

    IOLockUnlock(lock);
    return result;
}

// ============================================================================
// USB Control Transfer
// ============================================================================

kern_return_t AIC8800_USB::SendControlTransfer(uint8_t request,
                                                uint16_t value,
                                                uint16_t index,
                                                void *data,
                                                uint32_t length,
                                                bool is_in)
{
    if (!usb_interface) return kIOReturnNotReady;

    IOUSBHostPipe *control_pipe = usb_interface->GetPipeObj(0);
    if (!control_pipe) return kIOReturnNotReady;

    AIC8800_USB_ControlRequest req;
    req.bmRequestType = is_in ? AIC8800_REQ_TYPE_IN : AIC8800_REQ_TYPE_OUT;
    req.bRequest = request;
    req.wValue = value;
    req.wIndex = index;
    req.wLength = length;

    IOUSBHostCompletion completion;
    kern_return_t result;

    if (is_in) {
        result = control_pipe->DeviceRequest(
            &completion,
            (uint8_t *)&req,
            length,
            (uint8_t *)data,
            5000  // 5 second timeout
        );
    } else {
        result = control_pipe->DeviceRequest(
            &completion,
            (uint8_t *)&req,
            length,
            (uint8_t *)data,
            5000
        );
    }

    return result;
}

// ============================================================================
// Firmware Operations
// ============================================================================

kern_return_t AIC8800_USB::LoadFirmware(const uint8_t *data, uint32_t length)
{
    if (!data || length == 0) return kIOReturnBadArgument;
    if (state != AIC8800_STATE_FIRMWARE_LOADING) {
        IOLog("AIC8800: Invalid state for firmware load\n");
        return kIOReturnIllegalCommand;
    }

    IOLog("AIC8800: Loading firmware (%u bytes)\n", length);

    // Calculate checksum
    uint16_t checksum = AIC8800_FW_CHECKSUM_INIT;
    for (uint32_t i = 0; i < length; i++) {
        checksum ^= data[i];
        checksum = (checksum << 8) | (checksum >> 8);
    }

    // Send firmware in blocks
    uint32_t offset = 0;
    while (offset < length) {
        uint32_t block_size = length - offset;
        if (block_size > AIC8800_FW_BLOCK_SIZE) {
            block_size = AIC8800_FW_BLOCK_SIZE;
        }

        // Write block to device memory
        kern_return_t result = WriteMemory(offset, data + offset, block_size);
        if (result != kIOReturnSuccess) {
            IOLog("AIC8800: Firmware block write failed at offset %u\n", offset);
            return result;
        }

        offset += block_size;
    }

    // Verify firmware
    kern_return_t result = VerifyFirmware();
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Firmware verification failed\n");
        return result;
    }

    // Send checksum to device
    result = SendControlTransfer(
        AIC8800_USB_REQ_FIRMWARE,
        checksum,
        length,
        nullptr,
        0,
        false
    );

    if (result == kIOReturnSuccess) {
        firmware_loaded = true;
        state = AIC8800_STATE_READY;
        IOLog("AIC8800: Firmware loaded successfully\n");
    }

    return result;
}

kern_return_t AIC8800_USB::VerifyFirmware(void)
{
    // Read back firmware and compare
    uint8_t buffer[AIC8800_FW_BLOCK_SIZE];
    uint32_t address = 0;

    while (address < AIC8800_FW_BLOCK_SIZE * 10) {  // Verify first 2.5KB
        kern_return_t result = ReadMemory(address, buffer, AIC8800_FW_BLOCK_SIZE);
        if (result != kIOReturnSuccess) {
            return result;
        }

        // Basic verification - check for non-zero data
        bool all_zero = true;
        for (uint32_t i = 0; i < AIC8800_FW_BLOCK_SIZE; i++) {
            if (buffer[i] != 0) {
                all_zero = false;
                break;
            }
        }

        if (all_zero) {
            IOLog("AIC8800: Firmware verification failed at address %u\n", address);
            return kIOReturnError;
        }

        address += AIC8800_FW_BLOCK_SIZE;
    }

    return kIOReturnSuccess;
}

kern_return_t AIC8800_USB::ResetDevice(void)
{
    IOLog("AIC8800: Resetting device\n");

    uint32_t reg_value;
    kern_return_t result = ReadRegister(AIC8800_SYS_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    // Set reset bit
    reg_value |= 0x01;
    result = WriteRegister(AIC8800_SYS_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Wait for reset to complete
    IODelay(10000);  // 10ms

    // Clear reset bit
    reg_value &= ~0x01;
    result = WriteRegister(AIC8800_SYS_CTRL, reg_value);

    return result;
}

// ============================================================================
// Mode Switch Operations
// ============================================================================

kern_return_t AIC8800_USB::PerformModeSwitch(void)
{
    IOLog("AIC8800: Performing mode switch\n");

    // Send SCSI command to switch from mass storage to WiFi mode
    kern_return_t result = SendSCSICommand(
        AIC8800_MODE_SWITCH_CDB,
        AIC8800_MODE_SWITCH_CDB_LEN,
        nullptr,
        0,
        false
    );

    if (result == kIOReturnSuccess) {
        IOLog("AIC8800: Mode switch initiated\n");
        // Device will disconnect and re-enumerate as WiFi device
    } else {
        IOLog("AIC8800: Mode switch failed\n");
    }

    return result;
}

kern_return_t AIC8800_USB::SendSCSICommand(const uint8_t *cdb,
                                             uint8_t cdb_len,
                                             void *data,
                                             uint32_t data_len,
                                             bool is_data_in)
{
    if (!cdb || cdb_len == 0) return kIOReturnBadArgument;

    // For USB mass storage, we need to send SCSI commands via
    // the CBW (Command Block Wrapper) protocol
    struct __attribute__((packed)) {
        uint32_t signature;      // 0x43425355 "USBC"
        uint32_t tag;
        uint32_t data_length;
        uint8_t  flags;          // 0x80 = data in, 0x00 = data out
        uint8_t  lun;
        uint8_t  cdb_length;
    } cbw;

    cbw.signature = 0x43425355;
    cbw.tag = 0x12345678;
    cbw.data_length = data_len;
    cbw.flags = is_data_in ? 0x80 : 0x00;
    cbw.lun = 0;
    cbw.cdb_length = cdb_len;

    // Create buffer with CBW + CDB
    uint32_t total_length = sizeof(cbw) + cdb_len;
    uint8_t *buffer = (uint8_t *)IOMalloc(total_length);
    if (!buffer) return kIOReturnNoMemory;

    memcpy(buffer, &cbw, sizeof(cbw));
    memcpy(buffer + sizeof(cbw), cdb, cdb_len);

    // Send via bulk OUT
    kern_return_t result = SendData(buffer, total_length);

    IOFree(buffer, total_length);

    // If data in, receive response
    if (result == kIOReturnSuccess && is_data_in && data && data_len > 0) {
        uint32_t actual_length;
        result = ReceiveData(data, data_len, &actual_length);
    }

    return result;
}

// ============================================================================
// Data Transfer Operations
// ============================================================================

kern_return_t AIC8800_USB::SendData(const void *data, uint32_t length)
{
    if (!bulk_out_pipe || !data || length == 0) {
        return kIOReturnNotReady;
    }

    IOBufferMemoryDescriptor *buffer = IOBufferMemoryDescriptor::withBytes(
        data, length, kIODirectionOut, false);
    if (!buffer) return kIOReturnNoMemory;

    IOUSBHostCompletion completion;
    kern_return_t result = bulk_out_pipe->Write(
        &completion,
        buffer,
        length,
        5000  // 5 second timeout
    );

    buffer->Release();
    return result;
}

kern_return_t AIC8800_USB::ReceiveData(void *buffer, uint32_t length,
                                        uint32_t *actual_length)
{
    if (!bulk_in_pipe || !buffer || length == 0) {
        return kIOReturnNotReady;
    }

    IOBufferMemoryDescriptor *mem_desc = IOBufferMemoryDescriptor::withBytes(
        buffer, length, kIODirectionIn, false);
    if (!mem_desc) return kIOReturnNoMemory;

    uint32_t bytes_read = 0;
    IOUSBHostCompletion completion;
    kern_return_t result = bulk_in_pipe->Read(
        &completion,
        mem_desc,
        length,
        &bytes_read,
        5000  // 5 second timeout
    );

    if (actual_length) {
        *actual_length = bytes_read;
    }

    mem_desc->Release();
    return result;
}

// ============================================================================
// Interrupt Handling
// ============================================================================

kern_return_t AIC8800_USB::HandleInterrupt(void)
{
    if (!interrupt_pipe) return kIOReturnNotReady;

    uint8_t interrupt_data[8];
    uint32_t actual_length;

    kern_return_t result = ReceiveData(interrupt_data, sizeof(interrupt_data),
                                        &actual_length);
    if (result != kIOReturnSuccess) {
        return result;
    }

    // Parse interrupt status
    uint8_t status = interrupt_data[0];

    if (status & 0x01) {
        // TX complete
        IOLog("AIC8800: TX complete interrupt\n");
    }

    if (status & 0x02) {
        // RX data available
        IOLog("AIC8800: RX data interrupt\n");
    }

    if (status & 0x04) {
        // Error
        IOLog("AIC8800: Error interrupt\n");
    }

    return kIOReturnSuccess;
}

kern_return_t AIC8800_USB::EnableInterrupts(void)
{
    uint32_t reg_value;
    kern_return_t result = ReadRegister(AIC8800_INT_MASK, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x07;  // Enable all interrupts
    return WriteRegister(AIC8800_INT_MASK, reg_value);
}

kern_return_t AIC8800_USB::DisableInterrupts(void)
{
    uint32_t reg_value;
    kern_return_t result = ReadRegister(AIC8800_INT_MASK, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value &= ~0x07;  // Disable all interrupts
    return WriteRegister(AIC8800_INT_MASK, reg_value);
}
