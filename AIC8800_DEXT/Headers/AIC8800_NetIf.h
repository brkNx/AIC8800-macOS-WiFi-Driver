/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * Network Interface Definition
 */

#ifndef AIC8800_NETIF_H
#define AIC8800_NETIF_H

#include <DriverKit/IOTypes.h>
#include <DriverKit/IOService.h>
#include <DriverKit/IOBufferMemoryDescriptor.h>
#include <DriverKit/IOLocks.h>

#include "AIC8800_Driver.h"

struct AIC8800_EthernetAddress {
    uint8_t bytes[6];
};

class AIC8800_NetIf : public IOService {
    OSDeclareDefaultStructors(AIC8800_NetIf)

public:
    virtual kern_return_t Init(OSDictionary *dictionary) override;
    virtual kern_return_t Start(IOService *provider) override;
    virtual kern_return_t Stop(IOService *provider) override;
    virtual kern_return_t Free() override;

    virtual kern_return_t GetHardwareAddress(AIC8800_EthernetAddress *addr);
    virtual kern_return_t SetHardwareAddress(const AIC8800_EthernetAddress *addr);
    virtual kern_return_t GetPacketFilters(uint32_t *filters);
    virtual kern_return_t SetPacketFilters(uint32_t filters);

    virtual kern_return_t OutputPacket(void *data, uint32_t length, void *param);
    virtual kern_return_t InputPacket(const uint8_t *data, uint32_t length);

    virtual kern_return_t SetPowerState(uint32_t powerState);

    virtual kern_return_t StartScan(const uint8_t *ssid, uint8_t ssid_len,
                            uint8_t channel);
    virtual kern_return_t GetScanResults(void *buffer, uint32_t buffer_size,
                                 uint32_t *result_count);

    virtual kern_return_t Associate(const uint8_t *bssid, const uint8_t *ssid,
                            uint8_t ssid_len, const uint8_t *key,
                            uint32_t key_len);
    virtual kern_return_t Disassociate(void);

    virtual kern_return_t GetLinkStatus(uint32_t *status);
    virtual kern_return_t GetSignalQuality(int8_t *quality);
};

#endif /* AIC8800_NETIF_H */
