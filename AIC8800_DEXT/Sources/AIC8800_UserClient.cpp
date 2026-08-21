/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * User Client Implementation
 */

#include "AIC8800_Driver.h"
#include "AIC8800_USB.iig"
#include "AIC8800_NetIf.iig"
#include "AIC8800_UserClient.iig"

kern_return_t AIC8800_UserClient::Start(IOService *provider)
{
    kern_return_t result = IOUserClient::Start(provider);
    if (result != kIOReturnSuccess) {
        return result;
    }

    return kIOReturnSuccess;
}

kern_return_t AIC8800_UserClient::Stop(IOService *provider)
{
    return IOUserClient::Stop(provider);
}

kern_return_t AIC8800_UserClient::Free()
{
    return IOUserClient::Free();
}

kern_return_t AIC8800_UserClient::ExternalMethod(uint32_t selector,
                                                  IOUserClientMethodArguments *arguments,
                                                  const IOUserClientMethodDispatch *dispatch,
                                                  OSObject *target,
                                                  void *reference)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver) return kIOReturnNotReady;

    switch (selector) {
        case AIC8800_USERCLIENT_GET_STATUS: {
            if (!arguments->scalarOutput || arguments->scalarOutputCount < 2) {
                return kIOReturnBadArgument;
            }
            arguments->scalarOutput[0] = driver->state;
            arguments->scalarOutput[1] = driver->firmware_loaded ? 1 : 0;
            return kIOReturnSuccess;
        }

        case AIC8800_USERCLIENT_START_SCAN: {
            if (!arguments->structureInput || arguments->structureInputSize < 2) {
                return kIOReturnBadArgument;
            }
            uint8_t ssid_len = arguments->structureInput[0];
            uint8_t channel = arguments->structureInput[1];
            const uint8_t *ssid = nullptr;
            if (ssid_len > 0 && arguments->structureInputSize >= (size_t)(2 + ssid_len)) {
                ssid = arguments->structureInput + 2;
            }
            return AIC8800_SendProbeRequest(driver, ssid, ssid_len);
        }

        case AIC8800_USERCLIENT_GET_SCAN_RESULTS: {
            if (!arguments->structureOutput || !arguments->structureOutputSize) {
                return kIOReturnBadArgument;
            }
            uint32_t count = 0;
            uint32_t max_count = *arguments->structureOutputSize / sizeof(struct AIC8800_ScanResult);
            uint32_t copy_count = driver->scan_count;
            if (copy_count > max_count) copy_count = max_count;
            if (copy_count > AIC8800_MAX_SCAN_RESULTS) copy_count = AIC8800_MAX_SCAN_RESULTS;

            memcpy(arguments->structureOutput, driver->scan_results,
                   copy_count * sizeof(struct AIC8800_ScanResult));
            *arguments->structureOutputSize = copy_count * sizeof(struct AIC8800_ScanResult);
            return kIOReturnSuccess;
        }

        case AIC8800_USERCLIENT_CONNECT: {
            if (!arguments->structureInput || arguments->structureInputSize < 14) {
                return kIOReturnBadArgument;
            }
            const uint8_t *bssid = arguments->structureInput;
            uint8_t ssid_len = arguments->structureInput[6];
            if (arguments->structureInputSize < (size_t)(7 + ssid_len + 4)) {
                return kIOReturnBadArgument;
            }
            const uint8_t *ssid = arguments->structureInput + 7;
            uint32_t key_len = *(uint32_t *)(arguments->structureInput + 7 + ssid_len);
            const uint8_t *key = nullptr;
            if (key_len > 0) {
                if (arguments->structureInputSize < (size_t)(7 + ssid_len + 4 + key_len)) {
                    return kIOReturnBadArgument;
                }
                key = arguments->structureInput + 7 + ssid_len + 4;
            }

            return AIC8800_SendAuthFrame(driver, bssid, ssid, ssid_len);
        }

        case AIC8800_USERCLIENT_DISCONNECT: {
            return AIC8800_SendDeauthFrame(driver);
        }

        case AIC8800_USERCLIENT_GET_SIGNAL: {
            if (!arguments->scalarOutput || arguments->scalarOutputCount < 1) {
                return kIOReturnBadArgument;
            }
            uint32_t reg_value;
            kern_return_t result = usb_driver->ReadRegister(AIC8800_PHY_RX_GAIN, &reg_value);
            if (result == kIOReturnSuccess) {
                arguments->scalarOutput[0] = (reg_value >> 8) & 0xFF;
            }
            return result;
        }

        default:
            return kIOReturnUnsupported;
    }
}

kern_return_t AIC8800_UserClient::GetDriverStatus(uint32_t *state,
                                                   uint32_t *firmware_loaded)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver || !state || !firmware_loaded) return kIOReturnBadArgument;

    *state = driver->state;
    *firmware_loaded = driver->firmware_loaded ? 1 : 0;
    return kIOReturnSuccess;
}

kern_return_t AIC8800_UserClient::StartScan(const uint8_t *ssid, uint8_t ssid_len,
                                             uint8_t channel)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver) return kIOReturnBadArgument;

    return AIC8800_SendProbeRequest(driver, ssid, ssid_len);
}

kern_return_t AIC8800_UserClient::GetScanResults(uint8_t *buffer,
                                                  uint32_t buffer_size,
                                                  uint32_t *result_count)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver || !buffer || !result_count) return kIOReturnBadArgument;

    uint32_t copy_count = driver->scan_count;
    if (copy_count > AIC8800_MAX_SCAN_RESULTS) copy_count = AIC8800_MAX_SCAN_RESULTS;
    uint32_t copy_size = copy_count * sizeof(struct AIC8800_ScanResult);
    if (copy_size > buffer_size) copy_size = buffer_size;

    memcpy(buffer, driver->scan_results, copy_size);
    *result_count = copy_count;
    return kIOReturnSuccess;
}

kern_return_t AIC8800_UserClient::Connect(const uint8_t *bssid, const uint8_t *ssid,
                                           uint8_t ssid_len, const uint8_t *key,
                                           uint32_t key_len)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver) return kIOReturnBadArgument;

    return AIC8800_SendAuthFrame(driver, bssid, ssid, ssid_len);
}

kern_return_t AIC8800_UserClient::Disconnect(void)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    AIC8800_DriverData *driver = usb_driver ? usb_driver->driver_data : nullptr;
    if (!driver) return kIOReturnBadArgument;

    return AIC8800_SendDeauthFrame(driver);
}

kern_return_t AIC8800_UserClient::GetSignalQuality(int8_t *quality)
{
    AIC8800_USB *usb_driver = (AIC8800_USB *)GetProvider();
    if (!usb_driver || !quality) return kIOReturnBadArgument;

    uint32_t reg_value;
    kern_return_t result = usb_driver->ReadRegister(AIC8800_PHY_RX_GAIN, &reg_value);
    if (result == kIOReturnSuccess) {
        *quality = (int8_t)((reg_value >> 8) & 0xFF);
    }
    return result;
}
