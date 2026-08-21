#!/bin/bash
# AIC8800 macOS WiFi Driver - Build Helper
# This script verifies the environment and provides build commands

set -e

echo "=== AIC8800 macOS WiFi Driver Build Helper ==="
echo ""

# Check for Xcode
if ! command -v xcodebuild &> /dev/null; then
    echo "ERROR: Xcode not found. Please install Xcode from the App Store."
    exit 1
fi

XCODE_VERSION=$(xcodebuild -version | head -n 1)
echo "Xcode version: $XCODE_VERSION"

MACOS_VERSION=$(sw_vers -productVersion)
echo "macOS version: $MACOS_VERSION"
echo ""

# Verify Xcode project exists
if [ ! -d "AIC8800WiFi.xcodeproj" ]; then
    echo "ERROR: AIC8800WiFi.xcodeproj not found"
    exit 1
fi

if [ ! -f "AIC8800WiFi.xcodeproj/project.pbxproj" ]; then
    echo "ERROR: project.pbxproj not found"
    exit 1
fi

echo "Xcode project found: AIC8800WiFi.xcodeproj"
echo ""

# Validate project can be parsed
if xcodebuild -project AIC8800WiFi.xcodeproj -list &> /dev/null; then
    echo "Xcode project validated successfully"
else
    echo "WARNING: Xcode project validation failed"
    echo "You may need to open it in Xcode to resolve issues"
fi

echo ""
echo "=== Available Schemes ==="
xcodebuild -project AIC8800WiFi.xcodeproj -list 2>/dev/null | grep -A 10 "Schemes:" || true
echo ""

echo "=== Build Commands ==="
echo ""
echo "Open in Xcode:"
echo "  open AIC8800WiFi.xcodeproj"
echo ""
echo "Build app from command line:"
echo "  xcodebuild -project AIC8800WiFi.xcodeproj -scheme AIC8800WiFi -configuration Debug build"
echo ""
echo "Build DEXT from command line:"
echo "  xcodebuild -project AIC8800WiFi.xcodeproj -scheme AIC8800WiFi_DEXT -configuration Debug build"
echo ""
echo "=== Important Notes ==="
echo ""
echo "1. Select your Development Team in Signing & Capabilities for both targets"
echo "2. The DEXT bundle identifier is: com.aic8800.wifi.dext"
echo "3. Firmware files are NOT included (proprietary)"
echo "4. This driver is a work-in-progress skeleton"
echo ""
