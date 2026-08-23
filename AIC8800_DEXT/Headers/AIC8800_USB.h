/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * USB Driver Interface Definition
 * This file defines the DriverKit interface for AIC8800 USB WiFi driver.
 */

#ifndef AIC8800_USB_H
#define AIC8800_USB_H

#include <DriverKit/IOTypes.h>
#include <DriverKit/IOService.h>
#include <DriverKit/IOBufferMemoryDescriptor.h>
#include <USBDriverKit/IOUSBHostDevice.h>
#include <USBDriverKit/IOUSBHostInterface.h>
#include <USBDriverKit/IOUSBHostPipe.h>

#include "AIC8800_Driver.h"

// ============================================================================
// AIC8800 USB Driver Interface
// ============================================================================

class AIC8800_USB : public IOUSBHostDevice {
    OSDeclareDefaultStructors(AIC8800_USB)

public:
    struct AIC8800_DriverData *driver_data;

    IOUSBHostDevice *usb_device;
    IOUSBHostInterface *usb_interface;
    IOUSBHostPipe *bulk_in_pipe;
    IOUSBHostPipe *bulk_out_pipe;
    IOUSBHostPipe *interrupt_pipe;
    uint8_t interface_number;

    // USB Lifecycle
    virtual kern_return_t Init(OSDictionary *dictionary) override;
    virtual kern_return_t Start(IOService *provider) override;
    virtual kern_return_t Stop(IOService *provider) override;
    virtual kern_return_t Free() override;

    // Device Matching
    virtual kern_return_t InitwithDevice(IOUSBHostDevice *device,
                                 IOUSBHostInterface *iface);

    // Register I/O
    virtual kern_return_t ReadRegister(uint16_t address, uint32_t *value);
    virtual kern_return_t WriteRegister(uint16_t address, uint32_t value);

    // Memory Access
    virtual kern_return_t ReadMemory(uint16_t address, void *buffer, uint32_t length);
    virtual kern_return_t WriteMemory(uint16_t address, const void *buffer, uint32_t length);

    // Firmware Operations
    virtual kern_return_t LoadFirmware(const uint8_t *data, uint32_t length);
    virtual kern_return_t VerifyFirmware(void);
    virtual kern_return_t ResetDevice(void);

    // Mode Switch
    virtual kern_return_t PerformModeSwitch(void);
    virtual kern_return_t SendSCSICommand(const uint8_t *cdb, uint8_t cdb_len,
                                  void *data, uint32_t data_len, bool is_data_in);

    // Data Transfer
    virtual kern_return_t SendData(const void *data, uint32_t length);
    virtual kern_return_t ReceiveData(void *buffer, uint32_t length, uint32_t *actual_length);
    virtual kern_return_t SendControlTransfer(uint8_t request, uint16_t value,
                                      uint16_t index, void *data,
                                      uint32_t length, bool is_in);

    // Interrupt Handling
    virtual kern_return_t HandleInterrupt(void);
    virtual kern_return_t EnableInterrupts(void);
    virtual kern_return_t DisableInterrupts(void);
};

// ============================================================================
// USB Control Transfer Structures
// ============================================================================

struct AIC8800_USB_ControlRequest {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed));

// ============================================================================
// USB Bulk Transfer Header
// ============================================================================

struct AIC8800_USB_BulkHeader {
    uint8_t  type;
    uint8_t  flags;
    uint16_t length;
    uint16_t sequence;
    uint16_t reserved;
} __attribute__((packed));

#endif /* AIC8800_USB_H */
