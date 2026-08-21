/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * HAL Initialization - Power-on, MAC enable, RF calibration
 */

#include "AIC8800_Driver.h"

// ============================================================================
// HAL Initialization Sequence
// ============================================================================

kern_return_t AIC8800_HALInit(AIC8800_DriverData *driver)
{
    kern_return_t result;

    IOLog("AIC8800: Starting HAL initialization\n");

    // Step 1: Power-on sequence
    result = AIC8800_PowerOn(driver);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Power-on failed\n");
        return result;
    }

    // Step 2: Reset device
    result = driver->usb_driver->ResetDevice();
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Device reset failed\n");
        return result;
    }

    // Step 3: Read chip ID to verify communication
    uint32_t chip_id;
    result = driver->usb_driver->ReadRegister(AIC8800_CHIP_ID, &chip_id);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Failed to read chip ID\n");
        return result;
    }

    IOLog("AIC8800: Chip ID: 0x%08X\n", chip_id);

    // Step 4: MAC initialization
    result = AIC8800_MACInit(driver);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: MAC initialization failed\n");
        return result;
    }

    // Step 5: PHY initialization
    result = AIC8800_PHYInit(driver);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: PHY initialization failed\n");
        return result;
    }

    // Step 6: RF calibration
    result = AIC8800_RFCalibration(driver);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: RF calibration failed\n");
        return result;
    }

    // Step 7: Enable interrupts
    result = driver->usb_driver->EnableInterrupts();
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Failed to enable interrupts\n");
        return result;
    }

    // Step 8: Set MAC address
    result = AIC8800_SetMACAddress(driver, driver->config.mac_address);
    if (result != kIOReturnSuccess) {
        IOLog("AIC8800: Failed to set MAC address\n");
        return result;
    }

    IOLog("AIC8800: HAL initialization completed successfully\n");
    return kIOReturnSuccess;
}

// ============================================================================
// Power-on Sequence
// ============================================================================

kern_return_t AIC8800_PowerOn(AIC8800_DriverData *driver)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: Power-on sequence started\n");

    // Step 1: Read current power status
    result = driver->usb_driver->ReadRegister(AIC8800_PM_STATUS, &reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 2: Enable power domains
    reg_value |= 0x01;  // Digital power
    reg_value |= 0x02;  // Analog power
    reg_value |= 0x04;  // RF power
    result = driver->usb_driver->WriteRegister(AIC8800_PM_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 3: Wait for power stabilization
    IODelay(10000);  // 10ms

    // Step 4: Release from reset
    result = driver->usb_driver->ReadRegister(AIC8800_SYS_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x02;  // Release reset
    result = driver->usb_driver->WriteRegister(AIC8800_SYS_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 5: Wait for clock stabilization
    IODelay(5000);  // 5ms

    IOLog("AIC8800: Power-on sequence completed\n");
    return kIOReturnSuccess;
}

// ============================================================================
// MAC Initialization
// ============================================================================

kern_return_t AIC8800_MACInit(AIC8800_DriverData *driver)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: MAC initialization started\n");

    // Step 1: Reset MAC
    result = driver->usb_driver->ReadRegister(AIC8800_MAC_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // MAC reset
    result = driver->usb_driver->WriteRegister(AIC8800_MAC_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    IODelay(1000);  // 1ms

    reg_value &= ~0x01;  // Release reset
    result = driver->usb_driver->WriteRegister(AIC8800_MAC_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 2: Configure MAC address filter
    result = driver->usb_driver->ReadRegister(AIC8800_MAC_FILTER, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable unicast filter
    reg_value |= 0x02;  // Enable multicast filter
    reg_value |= 0x04;  // Enable broadcast
    result = driver->usb_driver->WriteRegister(AIC8800_MAC_FILTER, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 3: Configure TX/RX
    result = driver->usb_driver->ReadRegister(AIC8800_DMA_TX_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable TX DMA
    result = driver->usb_driver->WriteRegister(AIC8800_DMA_TX_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    result = driver->usb_driver->ReadRegister(AIC8800_DMA_RX_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable RX DMA
    result = driver->usb_driver->WriteRegister(AIC8800_DMA_RX_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    IOLog("AIC8800: MAC initialization completed\n");
    return kIOReturnSuccess;
}

// ============================================================================
// PHY Initialization
// ============================================================================

kern_return_t AIC8800_PHYInit(AIC8800_DriverData *driver)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: PHY initialization started\n");

    // Step 1: Reset PHY
    result = driver->usb_driver->ReadRegister(AIC8800_PHY_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // PHY reset
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    IODelay(1000);  // 1ms

    reg_value &= ~0x01;  // Release reset
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 2: Configure PHY for 2.4GHz
    reg_value = 0x01;  // 2.4GHz band
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_CHANNEL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 3: Set TX power (default 12 dBm)
    reg_value = 12;
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_TX_POWER, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 4: Configure RX gain
    reg_value = 0x03;  // Maximum gain
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_RX_GAIN, reg_value);
    if (result != kIOReturnSuccess) return result;

    IOLog("AIC8800: PHY initialization completed\n");
    return kIOReturnSuccess;
}

// ============================================================================
// RF Calibration
// ============================================================================

kern_return_t AIC8800_RFCalibration(AIC8800_DriverData *driver)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: RF calibration started\n");

    // Step 1: Enable RF PLL
    result = driver->usb_driver->ReadRegister(AIC8800_RF_PLL_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable PLL
    result = driver->usb_driver->WriteRegister(AIC8800_RF_PLL_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 2: Wait for PLL lock
    IODelay(5000);  // 5ms

    // Step 3: Verify PLL lock
    result = driver->usb_driver->ReadRegister(AIC8800_RF_PLL_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    if (!(reg_value & 0x02)) {
        IOLog("AIC8800: RF PLL lock failed\n");
        return kIOReturnTimeout;
    }

    // Step 4: Enable TX/RX paths
    result = driver->usb_driver->ReadRegister(AIC8800_RF_TX_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable TX
    result = driver->usb_driver->WriteRegister(AIC8800_RF_TX_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    result = driver->usb_driver->ReadRegister(AIC8800_RF_RX_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable RX
    result = driver->usb_driver->WriteRegister(AIC8800_RF_RX_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Step 5: Configure PA (Power Amplifier)
    result = driver->usb_driver->ReadRegister(AIC8800_RF_PA_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    reg_value |= 0x01;  // Enable PA
    reg_value &= ~0x06; // Clear gain bits
    reg_value |= 0x02;  // Set PA gain
    result = driver->usb_driver->WriteRegister(AIC8800_RF_PA_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    IOLog("AIC8800: RF calibration completed\n");
    return kIOReturnSuccess;
}

// ============================================================================
// MAC Address Configuration
// ============================================================================

kern_return_t AIC8800_SetMACAddress(AIC8800_DriverData *driver,
                                     const uint8_t *mac_addr)
{
    kern_return_t result;

    if (!mac_addr) return kIOReturnBadArgument;

    IOLog("AIC8800: Setting MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
          mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);

    // Set MAC address low (first 4 bytes)
    uint32_t mac_low = mac_addr[0] | (mac_addr[1] << 8) |
                       (mac_addr[2] << 16) | (mac_addr[3] << 24);
    result = driver->usb_driver->WriteRegister(AIC8800_MAC_ADDR_LOW, mac_low);
    if (result != kIOReturnSuccess) return result;

    // Set MAC address high (last 2 bytes)
    uint32_t mac_high = mac_addr[4] | (mac_addr[5] << 8);
    result = driver->usb_driver->WriteRegister(AIC8800_MAC_ADDR_HIGH, mac_high);
    if (result != kIOReturnSuccess) return result;

    return kIOReturnSuccess;
}

// ============================================================================
// Channel Configuration
// ============================================================================

kern_return_t AIC8800_SetChannel(AIC8800_DriverData *driver, uint8_t channel)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: Setting channel %d\n", channel);

    // Configure PHY channel
    reg_value = channel;
    result = driver->usb_driver->WriteRegister(AIC8800_PHY_CHANNEL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Configure RF PLL for channel
    reg_value = channel * 5;  // MHz offset
    result = driver->usb_driver->WriteRegister(AIC8800_RF_PLL_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Wait for PLL to lock
    IODelay(2000);  // 2ms

    // Update config
    driver->config.channel = channel;

    return kIOReturnSuccess;
}

// ============================================================================
// Band Selection
// ============================================================================

kern_return_t AIC8800_SetBand(AIC8800_DriverData *driver, uint8_t band)
{
    kern_return_t result;
    uint32_t reg_value;

    IOLog("AIC8800: Setting band %s\n", band == 0 ? "2.4GHz" : "5GHz");

    // Configure PHY for band
    result = driver->usb_driver->ReadRegister(AIC8800_PHY_CTRL, &reg_value);
    if (result != kIOReturnSuccess) return result;

    if (band == 0) {
        reg_value &= ~0x01;  // 2.4GHz
    } else {
        reg_value |= 0x01;   // 5GHz
    }

    result = driver->usb_driver->WriteRegister(AIC8800_PHY_CTRL, reg_value);
    if (result != kIOReturnSuccess) return result;

    // Update config
    driver->config.band = band;

    return kIOReturnSuccess;
}

// ============================================================================
// Power Control
// ============================================================================

kern_return_t AIC8800_SetTxPower(AIC8800_DriverData *driver, uint16_t power_dbm)
{
    kern_return_t result;

    IOLog("AIC8800: Setting TX power %d dBm\n", power_dbm);

    // Clamp to valid range (0-20 dBm)
    if (power_dbm > 20) power_dbm = 20;

    result = driver->usb_driver->WriteRegister(AIC8800_PHY_TX_POWER, power_dbm);
    if (result != kIOReturnSuccess) return result;

    driver->config.tx_power = power_dbm;

    return kIOReturnSuccess;
}
