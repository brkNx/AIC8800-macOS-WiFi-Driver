# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial project structure
- USB device matching for AIC8800 chips
- Mode switch (SCSI vendor command)
- Firmware upload protocol
- HAL initialization (power-on, MAC/PHY init, RF calibration)
- Register I/O via USB vendor control transfers
- Network interface (IOUserNetworkEthernet)
- 802.11 management frame handling
- Companion app (SwiftUI) with driver installation UI
- README with badges and documentation
- CONTRIBUTING guidelines
- GPL-2.0 license

### Planned
- Bluetooth HCI support
- Power management (suspend/resume)
- 5GHz band optimization
- MU-MIMO support
- Beacon filtering
- Roaming support
- WPA3-SAE support
- 802.11r fast roaming
- 802.11k/v neighbor reports

## [0.1.0] - 2024-01-01

### Added
- Initial release
- Basic WiFi functionality
- 2.4GHz support
- WPA2-Personal

[Unreleased]: https://github.com/brkNx/AIC8800-macOS-WiFi-Driver/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/brkNx/AIC8800-macOS-WiFi-Driver/releases/tag/v0.1.0
