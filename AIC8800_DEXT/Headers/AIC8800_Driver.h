/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * AIC8800 USB WiFi 6 (802.11ax) Driver for macOS
 * Supports: AIC8800D80, AIC8800DC, AIC8800DW
 * Interface: USB 2.0
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AIC8800_DRIVER_H
#define AIC8800_DRIVER_H

#include <DriverKit/IOService.h>
#include <DriverKit/IOBufferMemoryDescriptor.h>
#include <DriverKit/IOTimerEventSource.h>
#include <USBDriverKit/IOUSBHostDevice.h>
#include <USBDriverKit/IOUSBHostInterface.h>
#include <USBDriverKit/IOUSBHostPipe.h>

// ============================================================================
// AIC8800 USB Identifiers
// ============================================================================

#define AIC8800_VENDOR_ID           0xA69C
#define AIC8800_PRODUCT_ID_STORAGE  0x5721  // Mass storage mode (fake CD-ROM)
#define AIC8800_PRODUCT_ID_BOOT     0x8D80  // Boot ROM (firmware upload)
#define AIC8800_PRODUCT_ID_WIFI     0x8D81  // WiFi + Bluetooth operational
#define AIC8800_PRODUCT_ID_WIFI_ONLY 0x8D83 // WiFi only (no BT)

// Pandora mode (fake CD-ROM)
#define AIC8800_PANDORA_VID         0x1111
#define AIC8800_PANDORA_PID         0x1111

// ============================================================================
// USB Descriptor Constants
// ============================================================================

#define AIC8800_USB_CLASS_MASS_STORAGE  0x08
#define AIC8800_USB_CLASS_WIRELESS      0xE0
#define AIC8800_USB_SUBCLASS_RF         0x01
#define AIC8800_USB_PROTOCOL_BT         0x01

#define AIC8800_USB_ENDPOINT_BULK       0x02
#define AIC8800_USB_ENDPOINT_INTERRUPT  0x03

// ============================================================================
// AIC8800 Register Space (USB Vendor Control Transfers)
// ============================================================================

// System registers
#define AIC8800_SYS_CTRL              0x0000
#define AIC8800_SYS_STAT              0x0004
#define AIC8800_CHIP_ID               0x0008
#define AIC8800_FW_VER                0x000C

// MAC registers
#define AIC8800_MAC_CTRL              0x0100
#define AIC8800_MAC_ADDR_LOW          0x0104
#define AIC8800_MAC_ADDR_HIGH         0x0108
#define AIC8800_MAC_FILTER            0x010C

// PHY registers
#define AIC8800_PHY_CTRL              0x0200
#define AIC8800_PHY_TX_POWER          0x0204
#define AIC8800_PHY_RX_GAIN           0x0208
#define AIC8800_PHY_CHANNEL           0x020C

// RF registers
#define AIC8800_RF_TX_CTRL            0x0300
#define AIC8800_RF_RX_CTRL            0x0304
#define AIC8800_RF_PLL_CTRL           0x0308
#define AIC8800_RF_PA_CTRL            0x030C

// DMA registers
#define AIC8800_DMA_TX_CTRL           0x0400
#define AIC8800_DMA_TX_ADDR           0x0404
#define AIC8800_DMA_TX_STATUS         0x0408
#define AIC8800_DMA_RX_CTRL           0x0410
#define AIC8800_DMA_RX_ADDR           0x0414
#define AIC8800_DMA_RX_STATUS         0x0418

// Interrupt registers
#define AIC8800_INT_STATUS            0x0500
#define AIC8800_INT_MASK              0x0504
#define AIC8800_INT_CLEAR             0x0508

// Power management
#define AIC8800_PM_CTRL               0x0600
#define AIC8800_PM_STATUS             0x0604

// ============================================================================
// USB Control Transfer Commands
// ============================================================================

// Vendor-specific requests (bRequest)
#define AIC8800_USB_REQ_READ_REG      0x01
#define AIC8800_USB_REQ_WRITE_REG     0x02
#define AIC8800_USB_REQ_READ_MEM      0x03
#define AIC8800_USB_REQ_WRITE_MEM     0x04
#define AIC8800_USB_REQ_FIRMWARE      0x05
#define AIC8800_USB_REQ_MODE_SWITCH   0x06

// Control transfer bmRequestType
#define AIC8800_REQ_TYPE_OUT          0x40  // Host-to-device, vendor, interface
#define AIC8800_REQ_TYPE_IN           0xC0  // Device-to-host, vendor, interface

// ============================================================================
// Firmware Upload Protocol
// ============================================================================

#define AIC8800_FW_BLOCK_SIZE         256
#define AIC8800_FW_CHECKSUM_INIT      0x4321
#define AIC8800_FW_MAGIC              0x8800

// Firmware section types
#define AIC8800_FW_SECTION_FW         0x01
#define AIC8800_FW_SECTION_PATCH      0x02
#define AIC8800_FW_SECTION_CONFIG     0x03

// Firmware status
#define AIC8800_FW_STATUS_IDLE        0x00
#define AIC8800_FW_STATUS_LOADING     0x01
#define AIC8800_FW_STATUS_LOADED      0x02
#define AIC8800_FW_STATUS_ERROR       0xFF

// ============================================================================
// Mode Switch SCSI Command
// ============================================================================

// Vendor-specific SCSI CDB to switch from mass storage to WiFi mode
#define AIC8800_MODE_SWITCH_CDB_LEN   16
static const uint8_t AIC8800_MODE_SWITCH_CDB[AIC8800_MODE_SWITCH_CDB_LEN] = {
    0xFD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2
};

// ============================================================================
// 802.11 Constants
// ============================================================================

#define AIC8800_MAX_SSIDS             16
#define AIC8800_MAX_SCAN_RESULTS     64
#define AIC8800_MAX_TX_QUEUES        4
#define AIC8800_ETH_ALEN             6
#define AIC8800_TX_DESC_SIZE         32
#define AIC8800_RX_DESC_SIZE         32

// Frame types
#define AIC8800_FRAME_TYPE_MGMT      0x00
#define AIC8800_FRAME_TYPE_CTRL      0x01
#define AIC8800_FRAME_TYPE_DATA      0x02

// Management frame subtypes
#define AIC8800_MGMT_SUBTYPE_ASSOC_REQ    0x00
#define AIC8800_MGMT_SUBTYPE_ASSOC_RESP   0x01
#define AIC8800_MGMT_SUBTYPE_REASSOC_REQ  0x02
#define AIC8800_MGMT_SUBTYPE_REASSOC_RESP 0x03
#define AIC8800_MGMT_SUBTYPE_PROBE_REQ    0x04
#define AIC8800_MGMT_SUBTYPE_PROBE_RESP   0x05
#define AIC8800_MGMT_SUBTYPE_BEACON       0x08
#define AIC8800_MGMT_SUBTYPE_ATIM         0x05
#define AIC8800_MGMT_SUBTYPE_DISASSOC     0x0A
#define AIC8800_MGMT_SUBTYPE_AUTH         0x0B
#define AIC8800_MGMT_SUBTYPE_DEAUTH       0x0C
#define AIC8800_MGMT_SUBTYPE_ACTION       0x0D

// ============================================================================
// TX Descriptor Structure
// ============================================================================

struct AIC8800_TX_DESC {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
    uint32_t word4;
    uint32_t word5;
    uint32_t word6;
    uint32_t word7;
} __attribute__((packed));

// TX descriptor bit fields
#define TX_DESC_SET(n, v, s, m)  (((n) & ~(((m) << (s)))) | (((v) & (m)) << (s)))
#define TX_DESC_GET(n, s, m)     (((n) >> (s)) & (m))

#define TX_DESC_TYPE_S           0
#define TX_DESC_TYPE_M           0x03
#define TX_DESC_80211_EN_S       2
#define TX_DESC_80211_EN_M       0x01
#define TX_DESC_QOS_EN_S         3
#define TX_DESC_QOS_EN_M         0x01
#define TX_DESC_HTC_EN_S         4
#define TX_DESC_HTC_EN_M         0x01
#define TX_DESC_BW_S             5
#define TX_DESC_BW_M             0x03
#define TX_DESC_RATE_S           8
#define TX_DESC_RATE_M           0x1F
#define TX_DESC_POWER_S          16
#define TX_DESC_POWER_M          0x3F
#define TX_DESC_PACKET_ID_S      24
#define TX_DESC_PACKET_ID_M      0x0F
#define TX_DESC_RETRY_S          28
#define TX_DESC_RETRY_M          0x07

// ============================================================================
// RX Descriptor Structure
// ============================================================================

struct AIC8800_RX_DESC {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
} __attribute__((packed));

// RX descriptor bit fields
#define RX_DESC_LENGTH_S         0
#define RX_DESC_LENGTH_M         0x0FFF
#define RX_DESC_RATE_S           12
#define RX_DESC_RATE_M           0x0F
#define RX_DESC_SIGNAL_S         16
#define RX_DESC_SIGNAL_M         0xFF
#define RX_DESC_NOISE_S          24
#define RX_DESC_NOISE_M          0xFF

// ============================================================================
// Driver State Machine
// ============================================================================

typedef enum {
    AIC8800_STATE_IDLE = 0,
    AIC8800_STATE_INITIALIZING,
    AIC8800_STATE_FIRMWARE_LOADING,
    AIC8800_STATE_READY,
    AIC8800_STATE_SCANNING,
    AIC8800_STATE_AUTHENTICATING,
    AIC8800_STATE_ASSOCIATING,
    AIC8800_STATE_CONNECTED,
    AIC8800_STATE_DISCONNECTING,
    AIC8800_STATE_ERROR
} AIC8800_State;

// ============================================================================
// WiFi Configuration
// ============================================================================

struct AIC8800_WiFiConfig {
    uint8_t  mac_address[AIC8800_ETH_ALEN];
    uint8_t  channel;
    uint8_t  band;          // 0 = 2.4GHz, 1 = 5GHz
    uint16_t tx_power;      // in dBm
    uint8_t  tx_rate;       // MCS index
    uint8_t  retry_count;
    uint8_t  preamble;
    uint8_t  slot_time;
};

// ============================================================================
// Scan Result Structure
// ============================================================================

struct AIC8800_ScanResult {
    uint8_t  bssid[AIC8800_ETH_ALEN];
    uint8_t  ssid[33];
    uint8_t  ssid_len;
    uint8_t  channel;
    int8_t   rssi;
    uint16_t capability;
    uint8_t  signal_quality;
    uint8_t  encryption;    // 0=none, 1=WEP, 2=WPA, 3=WPA2, 4=WPA3
    uint8_t  mode;          // 0=infrastructure, 1=ad-hoc
};

// ============================================================================
// Driver Instance Structure
// ============================================================================

struct AIC8800_DriverData {
    // USB
    IOUSBHostDevice *usb_device;
    IOUSBHostInterface *usb_interface;
    IOUSBHostPipe *bulk_in_pipe;
    IOUSBHostPipe *bulk_out_pipe;
    IOUSBHostPipe *interrupt_pipe;
    uint8_t interface_number;

    // State
    AIC8800_State state;
    bool firmware_loaded;
    bool interface_active;

    // Configuration
    struct AIC8800_WiFiConfig config;

    // Scan
    struct AIC8800_ScanResult scan_results[AIC8800_MAX_SCAN_RESULTS];
    uint32_t scan_count;

    // TX/RX
    IOBufferMemoryDescriptor *tx_buffer;
    IOBufferMemoryDescriptor *rx_buffer;
    uint8_t *tx_desc_ring;
    uint8_t *rx_desc_ring;
    uint32_t tx_head;
    uint32_t tx_tail;
    uint32_t rx_head;

    // Timers
    IOTimerEventSource *timer;
    IOTimerEventSource *watchdog_timer;

    // Synchronization
    IOLock *lock;
};

#endif /* AIC8800_DRIVER_H */
