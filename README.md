# AIC8800 macOS WiFi 6 Driver

macOS DriverKit driver for AIC8800-based USB WiFi adapters (WiFi 6, 802.11ax).

## Supported Hardware

| Device | Chipset | USB VID:PID | Status |
|--------|---------|-------------|--------|
| HOCO HI34 | AIC8800 | A69C:8D81 | ✓ Tested |
| UGREEN AX900 | AIC8800D80 | A69C:8D83 | ✓ Supported |
| Tenda U11 | AIC8800D80 | 368B:8D8X | ✓ Supported |

## Features

- WiFi 6 (802.11ax) support
- Dual-band (2.4GHz / 5GHz)
- WPA2/WPA3 security
- USB 2.0 interface
- Native macOS DriverKit (no kext required)
- Apple Silicon + Intel support

## Requirements

- macOS 13.0 (Ventura) or later
- Xcode 15.0 or later
- Apple Developer ID (for distribution)

## Building

### Quick Start

```bash
# Clone the repository
git clone https://github.com/brkN/AIC8800-macOS-WiFi-Driver.git
cd AIC8800-macOS-WiFi-Driver

# Run setup script
./setup.sh

# Open in Xcode
open AIC8800WiFi.xcodeproj
```

### Manual Build

```bash
# Build the app
xcodebuild -scheme AIC8800WiFi -configuration Debug build

# Build the DEXT
xcodebuild -scheme AIC8800WiFi_DEXT -configuration Debug build
```

## Installation

### Development (Personal Team)

1. Open `AIC8800WiFi.xcodeproj` in Xcode
2. Select your Development Team in Signing & Capabilities
3. Build and Run the app
4. Click "Install Driver" in the app
5. Approve the system extension in System Settings > Privacy & Security

### Distribution (Developer ID)

1. Archive the app (Product > Archive)
2. Export with "Developer ID" signing
3. Notarize the app with Apple
4. Distribute the notarized app

## Usage

1. **Plug in** the HOCO HI34 USB WiFi adapter
2. **Open** the AIC8800WiFi app
3. **Click "Install Driver"** if not already installed
4. **Click "Scan"** to find available networks
5. **Select a network** and enter the password
6. **Connect** - the adapter should now work as a standard WiFi interface

## Architecture

```
AIC8800WiFi/
├── AIC8800_App/                 # SwiftUI companion app
│   ├── AppDelegate.swift        # System extension install
│   ├── ContentView.swift        # Driver status UI
│   └── StatusManager.swift      # USB device detection
│
├── AIC8800_DEXT/                # DriverKit driver extension
│   ├── Headers/
│   │   └── AIC8800_Driver.h     # Register defines, USB constants
│   ├── Sources/
│   │   ├── AIC8800_USB.cpp      # USB lifecycle, device matching
│   │   ├── AIC8800_HALInit.cpp  # Power-on, MAC/PHY init
│   │   ├── AIC8800_NetIf.cpp    # Network interface
│   │   ├── AIC8800_USB.iig      # USB driver interface
│   │   └── AIC8800_NetIf.iig    # Network interface
│   └── AIC8800_DEXT.entitlements
│
└── Firmware/                    # Firmware files (not included)
```

## USB Enumeration

The AIC8800 chip goes through three USB enumeration stages:

```
Stage 0: 1111:1111  →  Mass Storage (fake CD-ROM)
           ↓ SCSI: FD 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F2
Stage 1: a69c:8d80  →  Boot ROM (firmware upload)
           ↓ firmware loaded, USB soft disconnect
Stage 2: a69c:8d81  →  WiFi + Bluetooth operational
```

## Firmware

The AIC8800 chip requires firmware files to operate. These are typically:

- `fw_adid_8800d80_u02.bin` - Main firmware
- `fw_patch_8800d80.bin` - Patch file

**Note:** Firmware files are proprietary and cannot be distributed. Contact the device manufacturer for firmware files.

## Troubleshooting

### Device not detected

1. Check if the adapter is properly connected
2. Run `systemextensionsctl list` to verify DEXT is installed
3. Check System Settings > Privacy & Security for pending approval

### Driver fails to load

1. Ensure SIP is configured correctly:
   ```bash
   csrutil disable --without kext --without dext
   ```
2. Check the system log for errors:
   ```bash
   log show --predicate 'subsystem == "com.aic8800.wifi"' --last 5m
   ```

### WiFi not working

1. Ensure firmware is loaded (check app status)
2. Try unplugging and reconnecting the adapter
3. Check if the network interface appears:
   ```bash
   ifconfig | grep en
   ```

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This project is licensed under the GNU General Public License v2.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [RTL8812AU macOS Driver](https://github.com/eencacao/rtl8812au) - Reference for DriverKit WiFi driver architecture
- [AIC8800 Linux Driver](https://github.com/radxa-pkg/aic8800) - Linux driver reference implementation
- [Apple Developer Documentation](https://developer.apple.com/documentation/driverkit) - DriverKit documentation

## Disclaimer

This driver is provided as-is, without warranty of any kind. Use at your own risk. The author is not responsible for any damage to your device or data loss.
