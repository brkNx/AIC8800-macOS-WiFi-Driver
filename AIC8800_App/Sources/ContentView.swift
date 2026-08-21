//
//  ContentView.swift
//  AIC8800WiFi
//
//  Main UI for AIC8800 WiFi Driver
//

import SwiftUI

struct ContentView: View {
    @ObservedObject private var status = StatusManager.shared

    var body: some View {
        VStack(spacing: 20) {
            // Header
            HStack {
                Image(systemName: "wifi")
                    .font(.system(size: 40))
                    .foregroundColor(.blue)
                VStack(alignment: .leading) {
                    Text("AIC8800 WiFi 6")
                        .font(.title)
                        .fontWeight(.bold)
                    Text("HOCO HI34 USB Adapter")
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                }
            }
            .padding(.top, 20)

            Divider()

            // Driver Status
            GroupBox("Driver Status") {
                VStack(alignment: .leading, spacing: 10) {
                    StatusRow(title: "Driver", status: status.driverStatus)
                    StatusRow(title: "Device", status: status.deviceStatus)
                    StatusRow(title: "Firmware", status: status.firmwareStatus)
                    StatusRow(title: "Network", status: status.networkStatus)
                }
                .padding(10)
            }

            // Device Info
            GroupBox("Device Information") {
                VStack(alignment: .leading, spacing: 10) {
                    InfoRow(label: "Chip", value: "AIC8800")
                    InfoRow(label: "VID:PID", value: "A69C:8D81")
                    InfoRow(label: "MAC Address", value: status.macAddress)
                    InfoRow(label: "IP Address", value: status.ipAddress)
                    InfoRow(label: "Signal", value: "\(status.signalQuality)%")
                    InfoRow(label: "Speed", value: status.linkSpeed)
                }
                .padding(10)
            }

            // Actions
            GroupBox("Actions") {
                HStack(spacing: 15) {
                    Button(action: {
                        status.installDriver()
                    }) {
                        Label("Install Driver", systemImage: "arrow.down.circle")
                    }
                    .buttonStyle(.borderedProminent)
                    .disabled(status.driverInstalled)

                    Button(action: {
                        status.uninstallDriver()
                    }) {
                        Label("Uninstall", systemImage: "trash")
                    }
                    .buttonStyle(.bordered)
                    .disabled(!status.driverInstalled)

                    Button(action: {
                        status.scanNetworks()
                    }) {
                        Label("Scan", systemImage: "antenna.radiowaves.left.and.right")
                    }
                    .buttonStyle(.bordered)
                }
                .padding(10)
            }

            // Log
            GroupBox("Log") {
                ScrollView {
                    Text(status.log)
                        .font(.system(.caption, design: .monospaced))
                        .frame(maxWidth: .infinity, alignment: .leading)
                }
                .frame(height: 100)
            }

            Spacer()
        }
        .padding(20)
        .frame(width: 450, height: 600)
    }
}

struct StatusRow: View {
    let title: String
    let status: DriverStatus

    var body: some View {
        HStack {
            Text(title)
                .frame(width: 80, alignment: .leading)
            Circle()
                .fill(status.color)
                .frame(width: 10, height: 10)
            Text(status.text)
                .foregroundColor(.secondary)
        }
    }
}

struct InfoRow: View {
    let label: String
    let value: String

    var body: some View {
        HStack {
            Text(label)
                .frame(width: 100, alignment: .leading)
                .foregroundColor(.secondary)
            Text(value)
                .fontWeight(.medium)
        }
    }
}

#Preview {
    ContentView()
}
