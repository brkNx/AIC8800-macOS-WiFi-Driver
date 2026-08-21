<p align="center">
  <img src="https://img.shields.io/badge/macOS-13%2B-blue?logo=apple&logoColor=white" alt="macOS 13+">
  <img src="https://img.shields.io/badge/DriverKit-Native-black?logo=apple&logoColor=white" alt="DriverKit Native">
  <img src="https://img.shields.io/badge/WiFi%206-802.11ax-purple?logo=wifi&logoColor=white" alt="WiFi 6">
  <img src="https://img.shields.io/badge/Status-Development-yellow" alt="Status">
  <img src="https://img.shields.io/github/license/brkNx/AIC8800-macOS-WiFi-Driver?color=green" alt="License">
</p>

<h1 align="center">AIC8800 macOS WiFi 6 Driver</h1>

<p align="center">
  <strong>Native macOS DriverKit driver for AIC8800-based USB WiFi 6 adapters</strong>
</p>

<p align="center">
  No kext. No hacks. Pure DriverKit.<br>
  Apple Silicon + Intel • 2.4GHz + 5GHz • WPA2/WPA3
</p>

---

## Supported Devices

<table>
  <tr>
    <td align="center"><strong>Device</strong></td>
    <td align="center"><strong>Chipset</strong></td>
    <td align="center"><strong>USB ID</strong></td>
    <td align="center"><strong>Status</strong></td>
  </tr>
  <tr>
    <td>HOCO HI34</td>
    <td>AIC8800</td>
    <td><code>A69C:8D81</code></td>
    <td>✅ Tested</td>
  </tr>
  <tr>
    <td>UGREEN AX900</td>
    <td>AIC8800D80</td>
    <td><code>A69C:8D83</code></td>
    <td>✅ Supported</td>
  </tr>
  <tr>
    <td>Tenda U11</td>
    <td>AIC8800D80</td>
    <td><code>368B:8D8X</code></td>
    <td>✅ Supported</td>
  </tr>
  <tr>
    <td>AIC8800D80 Clones</td>
    <td>AIC8800D80</td>
    <td><code>A69C:8D80</code></td>
    <td>✅ Supported</td>
  </tr>
</table>

---

## Features

<table>
  <tr>
    <td width="50%" valign="top">

### WiFi 6 (802.11ax)
- ✅ Dual-band 2.4GHz / 5GHz
- ✅ Up to 286 Mbps per band
- ✅ MU-MIMO support (firmware dependent)
- ✅ Beamforming support

    </td>
    <td width="50%" valign="top">

### macOS Native
- ✅ DriverKit (no kext required)
- ✅ Apple Silicon (M1/M2/M3/M4)
- ✅ Intel Mac support
- ✅ macOS 13.0 Ventura+

    </td>
  </tr>
  <tr>
    <td width="50%" valign="top">

### Security
- ✅ WPA2-Personal
- ✅ WPA3-Personal
- ✅ Hardware encryption offload
- ✅ 802.11i compliant

    </td>
    <td width="50%" valign="top">

### Architecture
- ✅ USB 2.0 interface
- ✅ FullMAC driver model
- ✅ Companion app (SwiftUI)
- ✅ System extension install

    </td>
  </tr>
</table>

---

## Quick Start

### 1. Clone & Build

```bash
git clone https://github.com/brkNx/AIC8800-macOS-WiFi-Driver.git
cd AIC8800-macOS-WiFi-Driver
chmod +x setup.sh
./setup.sh
```

### 2. Open in Xcode

```bash
open AIC8800WiFi.xcodeproj
```

### 3. Configure Signing

1. Select **AIC8800WiFi** target → Signing & Capabilities
2. Select your **Development Team**
3. Repeat for **AIC8800WiFi_DEXT** target

### 4. Build & Run

1. Press **⌘R** to build and run
2. Click **Install Driver** in the app
3. Approve in **System Settings → Privacy & Security**

### 5. Connect

1. Plug in HOCO HI34
2. Click **Scan** in the app
3. Select network → Enter password
4. Done! ✅

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     AIC8800 WiFi Driver                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────┐    ┌─────────────────────┐            │
│  │   AIC8800_App       │    │   AIC8800_DEXT       │            │
│  │   (SwiftUI App)     │◄──►│   (DriverKit DEXT)   │            │
│  │                     │    │                     │            │
│  │  • Status UI        │    │  • USB Lifecycle    │            │
│  │  • Device Detection │    │  • Register I/O     │            │
│  │  • Install Driver   │    │  • Firmware Upload  │            │
│  │  • Scan Networks    │    │  • MAC/PHY Init     │            │
│  │                     │    │  • 802.11 State     │            │
│  └─────────────────────┘    └──────────┬──────────┘            │
│                                        │                        │
│                                        ▼                        │
│                              ┌─────────────────┐               │
│                              │  USB Hardware    │               │
│                              │  (AIC8800 Chip)  │               │
│                              └─────────────────┘               │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### USB Enumeration Flow

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  Stage 0     │     │  Stage 1     │     │  Stage 2     │
│  1111:1111   │────►│  A69C:8D80   │────►│  A69C:8D81   │
│  Mass Storage│     │  Boot ROM    │     │  WiFi + BT   │
└──────────────┘     └──────────────┘     └──────────────┘
       │                    │                    │
       │ SCSI Command       │ Firmware           │ Ready
       │ FD 00...00 F2      │ Upload             │
       ▼                    ▼                    ▼
```

---

## Project Structure

```
AIC8800-macOS-WiFi-Driver/
│
├── 📱 AIC8800_App/              # Companion App
│   ├── Sources/
│   │   ├── AppDelegate.swift    # App lifecycle
│   │   ├── ContentView.swift    # Main UI
│   │   └── StatusManager.swift  # USB detection & driver mgmt
│   ├── Info.plist
│   └── AIC8800_App.entitlements
│
├── 🔧 AIC8800_DEXT/             # DriverKit Extension
│   ├── Headers/
│   │   └── AIC8800_Driver.h     # Hardware definitions
│   ├── Sources/
│   │   ├── AIC8800_USB.cpp      # USB lifecycle & matching
│   │   ├── AIC8800_HALInit.cpp  # Power-on & calibration
│   │   ├── AIC8800_NetIf.cpp    # Network interface
│   │   ├── AIC8800_USB.iig      # USB interface definition
│   │   └── AIC8800_NetIf.iig    # Network interface definition
│   ├── Info.plist
│   └── AIC8800_DEXT.entitlements
│
├── 📦 Firmware/                  # Firmware (not included)
│   └── aic8800D80/
│
├── README.md
├── LICENSE
├── setup.sh
└── .gitignore
```

---

## Installation Guide

### Development (Personal Team)

```bash
# 1. Build the project
xcodebuild -scheme AIC8800WiFi -configuration Debug build

# 2. Run the app
open ~/Library/Developer/Xcode/DerivedData/AIC8800WiFi-*/Build/Products/Debug/AIC8800WiFi.app

# 3. Click "Install Driver" in the app
# 4. Approve in System Settings → Privacy & Security
```

### Distribution (Developer ID)

```bash
# 1. Archive in Xcode
# Product → Archive → Distribute App → Developer ID

# 2. Notarize
xcrun notarytool submit AIC8800WiFi.zip --apple-id "your@email.com" --password "app-specific-password" --team-id "TEAMID"

# 3. Staple
xcrun stapler staple AIC8800WiFi.app
```

---

## Troubleshooting

### Device Not Detected

```bash
# Check USB connection
system_profiler SPUSBDataType | grep -A 5 "AIC\|HOCO\|A69C"

# Check DEXT installation
systemextensionsctl list | grep aic8800

# Check system log
log show --predicate 'subsystem == "com.aic8800.wifi"' --last 5m
```

### Driver Fails to Load

```bash
# Check SIP status
csrutil status

# For development, disable DEXT restrictions
sudo csrutil disable --without kext --without dext

# Reboot after SIP changes
sudo reboot
```

### WiFi Not Working

```bash
# Check if interface exists
ifconfig | grep en

# Check driver loaded
kextstat | grep aic8800

# Reset USB
sudo ifconfig en0 down && sudo ifconfig en0 up
```

---

## Firmware

The AIC8800 requires proprietary firmware files:

| File | Purpose |
|------|---------|
| `fw_adid_8800d80_u02.bin` | Main firmware |
| `fw_patch_8800d80.bin` | Patch file |

> ⚠️ **Note:** Firmware files are proprietary and cannot be distributed. Contact the device manufacturer or check the [Linux driver repository](https://github.com/radxa-pkg/aic8800) for firmware extraction methods.

---

## Technical Details

### Register Map

| Address | Name | Description |
|---------|------|-------------|
| `0x0000` | `SYS_CTRL` | System control |
| `0x0008` | `CHIP_ID` | Chip identifier |
| `0x0100` | `MAC_CTRL` | MAC control |
| `0x0200` | `PHY_CTRL` | PHY control |
| `0x0300` | `RF_TX_CTRL` | RF TX control |
| `0x0400` | `DMA_TX_CTRL` | DMA TX control |
| `0x0500` | `INT_STATUS` | Interrupt status |

### USB Control Transfers

| Request | Code | Direction | Description |
|---------|------|-----------|-------------|
| READ_REG | `0x01` | IN | Read 32-bit register |
| WRITE_REG | `0x02` | OUT | Write 32-bit register |
| READ_MEM | `0x03` | IN | Read memory block |
| WRITE_MEM | `0x04` | OUT | Write memory block |
| FIRMWARE | `0x05` | OUT | Upload firmware |

---

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'feat: add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## License

This project is licensed under the **GNU General Public License v2.0** - see the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/eencacao/rtl8812au">
        <b>RTL8812AU Driver</b><br>
        DriverKit WiFi reference
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/radxa-pkg/aic8800">
        <b>AIC8800 Linux Driver</b><br>
        Linux driver reference
      </a>
    </td>
    <td align="center">
      <a href="https://developer.apple.com/documentation/driverkit">
        <b>Apple DriverKit</b><br>
        Official documentation
      </a>
    </td>
  </tr>
</table>

---

<p align="center">
  Made with ❤️ for macOS<br>
  <sub>Star ⭐ this repo if you find it useful!</sub>
</p>
