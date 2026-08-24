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
    @Published var vidPid: String = "—"

    // Log
    @Published var log: String = ""

    // Driver state
    @Published var driverInstalled: Bool = false

    // USB monitoring
    private var usbNotificationPort: IONotificationPortRef?
    private var usbAddedNotification: io_object_t = 0
    private var usbRemovedNotification: io_object_t = 0

    // Pending system extension operation. OSSystemExtensionRequest carries no
    // userInfo, so track which verb the delegate callback belongs to.
    private var pendingRequestIsDeactivation = false

    // Constants
    private let vendorID: UInt16 = 0xA69C
    private let productIDs: [UInt16] = [0x8D81, 0x8D83]
    private let dextIdentifier = "com.aic8800.wifi.dext"

    private static let logTimestampFormatter: DateFormatter = {
        let f = DateFormatter()
        f.dateStyle = .none
        f.timeStyle = .medium
        return f
    }()

    // ======================================================================
    // Initialization
    // ======================================================================

    override init() {
        super.init()
        checkDriverInstalled()
    }

    // ======================================================================
    // USB Device Monitoring
    // ======================================================================

    func startMonitoring() {
        guard usbNotificationPort == nil else {
            appendLog("USB monitoring already running")
            return
        }

        appendLog("Starting USB device monitoring...")

        // Create notification port
        let notificationPort = IONotificationPortCreate(kIOMasterPortDefault)
        usbNotificationPort = notificationPort
        guard let notificationPort else {
            appendLog("Failed to create notification port")
            return
        }

        // Match on vendor only; product ID is filtered in deviceAdded so both
        // WiFi (0x8D81) and WiFi-only (0x8D83) variants are detected.
        let addedDict = IOServiceMatching(kIOUSBDeviceClassName) as NSMutableDictionary
        addedDict["idVendor"] = NSNumber(value: vendorID)

        let selfRef = Unmanaged.passUnretained(self).toOpaque()
        var kr = IOServiceAddMatchingNotification(
            notificationPort,
            kIOFirstMatchNotification,
            addedDict,
            usbDeviceAddedCallback,
            selfRef,
            &usbAddedNotification
        )

        guard kr == KERN_SUCCESS else {
            appendLog("Failed to register USB-add notification: \(kr)")
            IONotificationPortDestroy(notificationPort)
            usbNotificationPort = nil
            return
        }

        let removedDict = IOServiceMatching(kIOUSBDeviceClassName) as NSMutableDictionary
        removedDict["idVendor"] = NSNumber(value: vendorID)

        kr = IOServiceAddMatchingNotification(
            notificationPort,
            kIOTerminatedNotification,
            removedDict,
            usbDeviceRemovedCallback,
            selfRef,
            &usbRemovedNotification
        )

        if kr != KERN_SUCCESS {
            appendLog("Failed to register USB-removal notification: \(kr)")
        }

        appendLog("USB monitoring started successfully")
        // Drain the iterators once to arm the notifications and process any
        // already-attached devices.
        deviceAdded(usbAddedNotification)
        if usbRemovedNotification != 0 {
            while true {
                let object = IOIteratorNext(usbRemovedNotification)
                if object == 0 { break }
                IOObjectRelease(object)
            }
        }
    }

    func stopMonitoring() {
        if usbAddedNotification != 0 {
            IOObjectRelease(usbAddedNotification)
            usbAddedNotification = 0
        }

        if usbRemovedNotification != 0 {
            IOObjectRelease(usbRemovedNotification)
            usbRemovedNotification = 0
        }

        if let port = usbNotificationPort {
            IONotificationPortDestroy(port)
            usbNotificationPort = nil
        }

        appendLog("USB monitoring stopped")
    }

    // ======================================================================
    // Device Detection Callbacks
    // ======================================================================

    func deviceAdded(_ iterator: io_iterator_t) {
        while case let device = IOIteratorNext(iterator), device != 0 {
            defer { IOObjectRelease(device) }

            guard let properties = getDeviceProperties(device) else { continue }
            let vid = properties["idVendor"] as? UInt16 ?? 0
            let pid = properties["idProduct"] as? UInt16 ?? 0

            guard vid == vendorID, productIDs.contains(pid) else { continue }

            appendLog("AIC8800 device detected!")
            appendLog("Device: VID=\(String(format: "%04X", vid)) PID=\(String(format: "%04X", pid))")

            DispatchQueue.main.async {
                self.deviceStatus = .installed
                self.vidPid = String(format: "%04X:%04X", vid, pid)
            }
        }
    }

    func deviceRemoved(_ iterator: io_iterator_t) {
        while case let object = IOIteratorNext(iterator), object != 0 {
            IOObjectRelease(object)
        }

        appendLog("AIC8800 device removed")
        DispatchQueue.main.async {
            self.deviceStatus = .notInstalled
            self.vidPid = "—"
            self.networkStatus = .notInstalled
            self.signalQuality = 0
            self.linkSpeed = "0 Mbps"
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

    // ======================================================================
    // Driver Installation
    // ======================================================================

    func installDriver() {
        appendLog("Starting driver installation...")
        driverStatus = .installing
        pendingRequestIsDeactivation = false

        let request = OSSystemExtensionRequest.activationRequest(
            forExtensionWithIdentifier: dextIdentifier,
            queue: .main
        )
        request.delegate = self

        OSSystemExtensionManager.shared.submitRequest(request)
        appendLog("System extension request submitted")
    }

    func uninstallDriver() {
        appendLog("Starting driver uninstallation...")
        driverStatus = .installing
        pendingRequestIsDeactivation = true

        let request = OSSystemExtensionRequest.deactivationRequest(
            forExtensionWithIdentifier: dextIdentifier,
            queue: .main
        )
        request.delegate = self

        OSSystemExtensionManager.shared.submitRequest(request)
        appendLog("System extension deactivation request submitted")
    }

    private func checkDriverInstalled() {
        // The DEXT populates an AIC8800_USB node in the IORegistry once it is
        // loaded; its presence is a reliable proxy for "installed and running".
        let running = dextUserClientService != nil
        driverInstalled = false
        driverStatus = .notInstalled
        if running {
            driverInstalled = true
            driverStatus = .installed
        }
        if let service = dextUserClientService {
            IOObjectRelease(service)
        }
    }

    // ======================================================================
    // Network Operations
    // ======================================================================

    private var dextUserClientService: io_service_t? {
        let matching = IOServiceMatching("AIC8800_UserClient") as CFDictionary
        let service = IOServiceGetMatchingService(kIOMasterPortDefault, matching)
        return service != 0 ? service : nil
    }

    func scanNetworks() {
        appendLog("Starting network scan...")

        guard let service = dextUserClientService else {
            appendLog("DEXT user client not found — install the driver and attach a device first")
            networkStatus = .error("driver not responding")
            return
        }
        IOObjectRelease(service)

        // TODO: open the user client via IOServiceOpen and issue the scan
        // selector once AIC8800_UserClient is implemented in the DEXT.
        appendLog("DEXT found, but user client scan API not implemented yet")
        networkStatus = .error("scan not implemented in DEXT")
    }

    func connectToNetwork(ssid: String, password: String) {
        appendLog("Connecting to \(ssid)...")

        guard let service = dextUserClientService else {
            appendLog("DEXT user client not found — install the driver and attach a device first")
            networkStatus = .error("driver not responding")
            return
        }
        IOObjectRelease(service)

        // TODO: association + 4-way handshake via the user client.
        appendLog("DEXT found, but association API not implemented yet")
        networkStatus = .error("connect not implemented in DEXT")
    }

    // ======================================================================
    // Log
    // ======================================================================

    private func appendLog(_ message: String) {
        let timestamp = StatusManager.logTimestampFormatter.string(from: Date())
        let entry = "[\(timestamp)] \(message)\n"

        DispatchQueue.main.async {
            self.log.append(entry)
        }

        print(entry, terminator: "")
    }
}

// ============================================================================
// IOKit USB Callbacks
// ============================================================================

private func usbDeviceAddedCallback(
    refcon: UnsafeMutableRawPointer?,
    iterator: io_iterator_t
) {
    guard let refcon else { return }
    let manager = Unmanaged<StatusManager>.fromOpaque(refcon).takeUnretainedValue()
    manager.deviceAdded(iterator)
}

private func usbDeviceRemovedCallback(
    refcon: UnsafeMutableRawPointer?,
    iterator: io_iterator_t
) {
    guard let refcon else { return }
    let manager = Unmanaged<StatusManager>.fromOpaque(refcon).takeUnretainedValue()
    manager.deviceRemoved(iterator)
}

// ============================================================================
// System Extension Delegate
// ============================================================================

extension StatusManager: OSSystemExtensionRequestDelegate {
    func request(
        _ request: OSSystemExtensionRequest,
        actionForReplacingExtension existing: OSSystemExtensionProperties,
        withExtension ext: OSSystemExtensionProperties
    ) -> OSSystemExtensionRequest.ReplacementAction {
        appendLog("Extension replacing v\(existing.bundleShortVersion) with v\(ext.bundleShortVersion)")
        return .replace
    }

    func request(_ request: OSSystemExtensionRequest, didFinishWithResult result: OSSystemExtensionRequest.Result) {
        let wasDeactivation = pendingRequestIsDeactivation

        switch result {
        case .completed:
            appendLog(wasDeactivation
                ? "Driver uninstallation completed"
                : "Driver installation completed successfully")
            DispatchQueue.main.async {
                self.driverInstalled = !wasDeactivation
                self.driverStatus = wasDeactivation ? .notInstalled : .installed
            }
        case .willCompleteAfterReboot:
            appendLog(wasDeactivation
                ? "Driver uninstallation will complete after reboot"
                : "Driver will complete installation after reboot")
            DispatchQueue.main.async {
                self.driverInstalled = !wasDeactivation
                self.driverStatus = wasDeactivation ? .notInstalled : .installed
            }
        @unknown default:
            appendLog("Unknown system extension result")
        }
    }

    func request(_ request: OSSystemExtensionRequest, didFailWithError error: Error) {
        let wasDeactivation = pendingRequestIsDeactivation
        appendLog((wasDeactivation ? "Uninstallation" : "Installation")
            + " failed: \(error.localizedDescription)")
        DispatchQueue.main.async {
            self.driverStatus = .error(error.localizedDescription)
            if wasDeactivation {
                self.checkDriverInstalled()
            }
        }
    }

    func requestNeedsUserApproval(_ request: OSSystemExtensionRequest) {
        appendLog("User approval required in System Settings")
        DispatchQueue.main.async {
            self.driverStatus = .installing
        }
    }
}
