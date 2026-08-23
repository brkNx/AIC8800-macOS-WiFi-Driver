/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * User Client Interface Definition
 * Allows companion app to communicate with the driver extension.
 */

#ifndef AIC8800_USERCLIENT_H
#define AIC8800_USERCLIENT_H

#include <DriverKit/IOTypes.h>
#include <DriverKit/IOService.h>
#include <DriverKit/IOUserClient.h>

#include "AIC8800_Driver.h"

#define AIC8800_USERCLIENT_GET_STATUS       0
#define AIC8800_USERCLIENT_START_SCAN       1
#define AIC8800_USERCLIENT_GET_SCAN_RESULTS 2
#define AIC8800_USERCLIENT_CONNECT          3
#define AIC8800_USERCLIENT_DISCONNECT       4
#define AIC8800_USERCLIENT_GET_SIGNAL       5
#define AIC8800_USERCLIENT_METHOD_COUNT     6

class AIC8800_UserClient : public IOUserClient {
    OSDeclareDefaultStructors(AIC8800_UserClient)

public:
    virtual kern_return_t Init(OSDictionary *dictionary) override;
    virtual kern_return_t Start(IOService *provider) override;
    virtual kern_return_t Stop(IOService *provider) override;
    virtual kern_return_t Free() override;

    virtual IOReturn externalMethod(uint32_t selector,
                                  IOExternalMethodArguments *arguments,
                                  const IOExternalMethodDispatch *dispatch,
                                  OSObject *target,
                                  void *reference) override;

    virtual kern_return_t GetDriverStatus(uint32_t *state, uint32_t *firmware_loaded);
    virtual kern_return_t StartScan(const uint8_t *ssid, uint8_t ssid_len,
                            uint8_t channel);
    virtual kern_return_t GetScanResults(uint8_t *buffer, uint32_t buffer_size,
                                 uint32_t *result_count);
    virtual kern_return_t Connect(const uint8_t *bssid, const uint8_t *ssid,
                          uint8_t ssid_len, const uint8_t *key,
                          uint32_t key_len);
    virtual kern_return_t Disconnect(void);
    virtual kern_return_t GetSignalQuality(int8_t *quality);
};

#endif /* AIC8800_USERCLIENT_H */
