//
//  AppDelegate.swift
//  AIC8800WiFi
//
//  Companion app for AIC8800 WiFi Driver
//

import SwiftUI
import SystemExtensions

@main
struct AIC8800WiFiApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate

    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .windowStyle(.hiddenTitleBar)
        .windowResizability(.contentSize)
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationDidFinishLaunching(_ notification: Notification) {
        // Initialize USB device detection
        StatusManager.shared.startMonitoring()
    }

    func applicationWillTerminate(_ notification: Notification) {
        StatusManager.shared.stopMonitoring()
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}
