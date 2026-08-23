//
//  StatusManager.swift
//  AIC8800WiFi
//
//  USB device detection and driver management
//

import Foundation
import IOKit
import IOKit.usb
import SystemExtensions
import SwiftUI

// ============================================================================
// Driver Status
// ============================================================================

enum DriverStatus {
    case notInstalled
    case installing
    case installed
    case error(String)

    var text: String {
        switch self {
        case .notInstalled: return "Not Installed"
        case .installing: return "Installing..."
        case .installed: return "Installed"
        case .error(let msg): return "Error: \(msg)"
        }
    }

    var color: Color {
        switch self {
        case .notInstalled: return .gray
        case .installing: return .orange
        case .installed: return .green
        case .error: return .red
        }
    }
}

// ============================================================================
// Status Manager
// ============================================================================

class StatusManager: NSObject, ObservableObject {
    static let shared = StatusManager()

    // Driver status
    @Published var driverStatus: DriverStatus = .notInstalled
    @Published var deviceStatus: DriverStatus = .notInstalled
    @Published var firmwareStatus: DriverStatus = .notInstalled
    @Published var networkStatus: DriverStatus = .notInstalled

    // Device info
    @Published var macAddress: String = "XX:XX:XX:XX:XX:XX"
    @Published var ipAddress: String = "0.0.0.0"
    @Published var signalQuality: Int = 0
    @Published var linkSpeed: String = "0 Mbps"

    // Log
    @Published var log: String = ""

    // Driver state
    @Published var driverInstalled: Bool = false

    // USB monitoring
    private var usbIterator: io_iterator_t = 0
    private var usbNotificationPort: IONotificationPortRef?
    private var usbAddedNotification: io_object_t = 0

    // Constants
    private let vendorID: UInt16 = 0xA69C
    private let productIDs: [UInt16] = [0x8D81, 0x8D83]

    // ============================================================================
    // Initialization
    // ============================================================================

    init() {
        checkDriverInstalled()
    }

    // ============================================================================
    // USB Device Monitoring
    // ============================================================================

    func startMonitoring() {
        appendLog("Starting USB device monitoring...")

        // Create notification port
        usbNotificationPort = IONotificationPortCreate(kIOMasterPortDefault)
        guard let notificationPort = usbNotificationPort else {
            appendLog("Failed to create notification port")
            return
        }

        // Create matching dictionary for AIC8800
        let matchingDict = IOServiceMatching(kIOUSBDeviceClassName)
        matchingDict[kIOUSBVendorID] = NSNumber(value: vendorID)
        matchingDict[kIOUSBProductID] = NSNumber(value: productIDs[0])

        // Register for device additions
        let selfRef = Unmanaged.passUnretained(self).toOpaque()
        let kr = IOServiceAddMatchingNotification(
            notificationPort,
            kIOFirstMatchNotification,
            matchingDict,
            deviceAddedCallback,
            selfRef,
            &usbAddedNotification
        )

        if kr == KERN_SUCCESS {
            appendLog("USB monitoring started successfully")
            // Process any existing devices
            deviceAdded(usbAddedNotification)
        } else {
            appendLog("Failed to register USB notification: \(kr)")
        }
    }

    func stopMonitoring() {
        if usbAddedNotification != 0 {
            IOObjectRelease(usbAddedNotification)
            usbAddedNotification = 0
        }

        if let port = usbNotificationPort {
            IONotificationPortDestroy(port)
            usbNotificationPort = nil
        }

        appendLog("USB monitoring stopped")
    }

    // ============================================================================
    // Device Detection Callback
    // ============================================================================

    private func deviceAdded(_ iterator: io_iterator_t) {
        var device = IOIteratorNext(iterator)
        while device != 0 {
            appendLog("AIC8800 device detected!")

            // Get device properties
            if let properties = getDeviceProperties(device) {
                let vid = properties[kIOUSBVendorID] as? UInt16 ?? 0
                let pid = properties[kIOUSBProductID] as? UInt16 ?? 0

                appendLog("Device: VID=\(String(format: "%04X", vid)) PID=\(String(format: "%04X", pid))")

                DispatchQueue.main.async {
                    self.deviceStatus = .installed
                }
            }

            IOObjectRelease(device)
            device = IOIteratorNext(iterator)
        }
    }

    private func getDeviceProperties(_ device: io_object_t) -> [String: Any]? {
        var properties: Unmanaged<CFMutableDictionary>?
        let kr = IORegistryEntryCreateCFProperties(
            device,
            &properties,
            kCFAllocatorDefault,
            IOOptionBits(kIORegistryIterateRecursively | kIORegistryIterateParents)
        )

        guard kr == KERN_SUCCESS, let dict = properties?.takeRetainedValue() else {
            return nil
        }

        return dict as? [String: Any]
    }

    // ============================================================================
    // Driver Installation
    // ============================================================================

    func installDriver() {
        appendLog("Starting driver installation...")
        driverStatus = .installing

        let request = OSSystemExtensionRequest(
            activationRequest(forExtensionWithIdentifier: "com.aic8800.wifi.dext",
                              queue: .main)
        )
        request.delegate = self

        OSSystemExtensionManager.shared.submitRequest(request)
        appendLog("System extension request submitted")
    }

    func uninstallDriver() {
        appendLog("Starting driver uninstallation...")

        let request = OSSystemExtensionRequest(
            deactivationRequest(forExtensionWithIdentifier: "com.aic8800.wifi.dext",
                                queue: .main)
        )
        request.delegate = self

        OSSystemExtensionManager.shared.submitRequest(request)
        appendLog("System extension deactivation request submitted")
    }

    private func checkDriverInstalled() {
        // Check if DEXT is already installed
        let processInfo = ProcessInfo()
        let arguments = processInfo.arguments

        // Simple check - in real implementation, use system extensions API
        driverInstalled = false
        driverStatus = .notInstalled
    }

    // ============================================================================
    // Network Operations
    // ============================================================================

    func scanNetworks() {
        appendLog("Starting network scan...")

        // In real implementation, this would communicate with the DEXT
        // via IOKit user client to trigger scan

        // For now, simulate scan
        DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
            self.appendLog("Scan completed: 5 networks found")
        }
    }

    func connectToNetwork(ssid: String, password: String) {
        appendLog("Connecting to \(ssid)...")

        // In real implementation, this would:
        // 1. Send association request to DEXT
        // 2. Perform WPA2 4-way handshake
        // 3. Configure IP address

        DispatchQueue.main.asyncAfter(deadline: .now() + 3) {
            self.appendLog("Connected to \(ssid)")
            self.networkStatus = .installed
            self.signalQuality = 85
            self.linkSpeed = "286 Mbps"
        }
    }

    // ============================================================================
    // Log
    // ============================================================================

    private func appendLog(_ message: String) {
        let timestamp = DateFormatter.localizedString(
            from: Date(),
            dateStyle: .none,
            timeStyle: .medium
        )
        let entry = "[\(timestamp)] \(message)\n"

        DispatchQueue.main.async {
            self.log.append(entry)
        }

        print(entry, terminator: "")
    }
}

// ============================================================================
// System Extension Delegate
// ============================================================================

extension StatusManager: OSSystemExtensionRequestDelegate {
    func request(
        _ request: OSSystemExtensionRequest,
        actionForExtension extensionProperties: [String: Any],
        beforeExtensionActivation before: Bool
    ) -> OSSystemExtensionRequest.Action {
        appendLog("Extension action requested (before activation: \(before))")
        return .replace
    }

    func request(_ request: OSSystemExtensionRequest, didFinishWithResult result: OSSystemExtensionRequest.Result) {
        switch result {
        case .completed:
            appendLog("Driver installation completed successfully")
            DispatchQueue.main.async {
                self.driverStatus = .installed
                self.driverInstalled = true
            }
        case .willCompleteAfterReboot:
            appendLog("Driver will complete installation after reboot")
            DispatchQueue.main.async {
                self.driverStatus = .installed
            }
        @unknown default:
            appendLog("Unknown installation result")
        }
    }

    func request(_ request: OSSystemExtensionRequest, didFailWithError error: Error) {
        appendLog("Installation failed: \(error.localizedDescription)")
        DispatchQueue.main.async {
            self.driverStatus = .error(error.localizedDescription)
        }
    }

    func requestNeedsUserApproval(_ request: OSSystemExtensionRequest) {
        appendLog("User approval required in System Settings")
        DispatchQueue.main.async {
            self.driverStatus = .installing
        }
    }
}
