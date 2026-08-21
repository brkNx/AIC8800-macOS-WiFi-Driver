#!/bin/bash
# AIC8800 macOS WiFi Driver - Build Script
# This script helps set up and build the driver project

set -e

echo "=== AIC8800 macOS WiFi Driver Build Script ==="
echo ""

# Check for Xcode
if ! command -v xcodebuild &> /dev/null; then
    echo "ERROR: Xcode not found. Please install Xcode from the App Store."
    exit 1
fi

# Check Xcode version
XCODE_VERSION=$(xcodebuild -version | head -n 1)
echo "Xcode version: $XCODE_VERSION"

# Check for macOS version
MACOS_VERSION=$(sw_vers -productVersion)
echo "macOS version: $MACOS_VERSION"
echo ""

# Create Xcode project
echo "Creating Xcode project..."

# Create project directory
mkdir -p AIC8800WiFi.xcodeproj

# Create project.pbxproj
cat > AIC8800WiFi.xcodeproj/project.pbxproj << 'EOF'
// !$*UTF8*$!
{
	archiveVersion = 1;
	classes = {
	};
	objectVersion = 56;
	objects = {

/* Begin PBXBuildFile section */
		A1000001 /* AppDelegate.swift in Sources */ = {isa = PBXBuildFile; fileRef = A2000001; };
		A1000002 /* ContentView.swift in Sources */ = {isa = PBXBuildFile; fileRef = A2000002; };
		A1000003 /* StatusManager.swift in Sources */ = {isa = PBXBuildFile; fileRef = A2000003; };
		A1000004 /* AIC8800_USB.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A2000004; };
		A1000005 /* AIC8800_HALInit.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A2000005; };
		A1000006 /* AIC8800_NetIf.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A2000006; };
		A1000007 /* AIC8800_Driver.h in Headers */ = {isa = PBXBuildFile; fileRef = A2000007; };
/* End PBXBuildFile section */

/* Begin PBXFileReference section */
		A2000001 /* AppDelegate.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = AppDelegate.swift; sourceTree = "<group>"; };
		A2000002 /* ContentView.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = ContentView.swift; sourceTree = "<group>"; };
		A2000003 /* StatusManager.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = StatusManager.swift; sourceTree = "<group>"; };
		A2000004 /* AIC8800_USB.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = AIC8800_USB.cpp; sourceTree = "<group>"; };
		A2000005 /* AIC8800_HALInit.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = AIC8800_HALInit.cpp; sourceTree = "<group>"; };
		A2000006 /* AIC8800_NetIf.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = AIC8800_NetIf.cpp; sourceTree = "<group>"; };
		A2000007 /* AIC8800_Driver.h */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.c.h; path = AIC8800_Driver.h; sourceTree = "<group>"; };
		A2000008 /* AIC8800_USB.iig */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.iig; path = AIC8800_USB.iig; sourceTree = "<group>"; };
		A2000009 /* AIC8800_NetIf.iig */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.iig; path = AIC8800_NetIf.iig; sourceTree = "<group>"; };
		A2000010 /* Info.plist */ = {isa = PBXFileReference; lastKnownFileType = text.plist.xml; path = Info.plist; sourceTree = "<group>"; };
		A2000011 /* AIC8800_App.entitlements */ = {isa = PBXFileReference; lastKnownFileType = text.plist.entitlements; path = AIC8800_App.entitlements; sourceTree = "<group>"; };
		A2000012 /* AIC8800_DEXT.entitlements */ = {isa = PBXFileReference; lastKnownFileType = text.plist.entitlements; path = AIC8800_DEXT.entitlements; sourceTree = "<group>"; };
		A2000013 /* AIC8800WiFi */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = AIC8800WiFi.app; sourceTree = BUILT_PRODUCTS_DIR; };
		A2000014 /* AIC8800WiFi_DEXT */ = {isa = PBXFileReference; explicitFileType = "wrapper.app-extension"; includeInIndex = 0; path = AIC8800WiFi_DEXT.appex; sourceTree = BUILT_PRODUCTS_DIR; };
/* End PBXFileReference section */

/* Begin PBXGroup section */
		A3000001 = {
			isa = PBXGroup;
			children = (
				A3000002 /* AIC8800_App */,
				A3000003 /* AIC8800_DEXT */,
				A3000004 /* Products */,
			);
			sourceTree = "<group>";
		};
		A3000002 /* AIC8800_App */ = {
			isa = PBXGroup;
			children = (
				A2000001 /* AppDelegate.swift */,
				A2000002 /* ContentView.swift */,
				A2000003 /* StatusManager.swift */,
				A2000010 /* Info.plist */,
				A2000011 /* AIC8800_App.entitlements */,
			);
			path = AIC8800_App;
			sourceTree = "<group>";
		};
		A3000003 /* AIC8800_DEXT */ = {
			isa = PBXGroup;
			children = (
				A2000004 /* AIC8800_USB.cpp */,
				A2000005 /* AIC8800_HALInit.cpp */,
				A2000006 /* AIC8800_NetIf.cpp */,
				A2000007 /* AIC8800_Driver.h */,
				A2000008 /* AIC8800_USB.iig */,
				A2000009 /* AIC8800_NetIf.iig */,
				A2000010 /* Info.plist */,
				A2000012 /* AIC8800_DEXT.entitlements */,
			);
			path = AIC8800_DEXT;
			sourceTree = "<group>";
		};
		A3000004 /* Products */ = {
			isa = PBXGroup;
			children = (
				A2000013 /* AIC8800WiFi */,
				A2000014 /* AIC8800WiFi_DEXT */,
			);
			name = Products;
			sourceTree = "<group>";
		};
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
		A4000001 /* AIC8800WiFi */ = {
			isa = PBXNativeTarget;
			buildConfigurationList = A6000001 /* Build configuration list for PBXNativeTarget "AIC8800WiFi" */;
			buildPhases = (
				A5000001 /* Sources */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = AIC8800WiFi;
			productName = AIC8800WiFi;
			productReference = A2000013 /* AIC8800WiFi.app */;
			productType = "com.apple.product-type.application";
		};
		A4000002 /* AIC8800WiFi_DEXT */ = {
			isa = PBXNativeTarget;
			buildConfigurationList = A6000002 /* Build configuration list for PBXNativeTarget "AIC8800WiFi_DEXT" */;
			buildPhases = (
				A5000002 /* Sources */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = AIC8800WiFi_DEXT;
			productName = AIC8800WiFi_DEXT;
			productReference = A2000014 /* AIC8800WiFi_DEXT.appex */;
			productType = "com.apple.product-type.app-extension";
		};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
		A7000001 /* Project object */ = {
			isa = PBXProject;
			attributes = {
				BuildIndependentTargetsInParallel = 1;
				LastSwiftUpdateCheck = 1500;
				LastUpgradeCheck = 1500;
				TargetAttributes = {
					A4000001 = {
						CreatedOnToolsVersion = 15.0;
					};
					A4000002 = {
						CreatedOnToolsVersion = 15.0;
					};
				};
			};
			buildConfigurationList = A6000003 /* Build configuration list for PBXProject "AIC8800WiFi" */;
			compatibilityVersion = "Xcode 14.0";
			developmentRegion = en;
			hasScannedForEncodings = 0;
			knownRegions = (
				en,
				Base,
			);
			mainGroup = A3000001;
			productRefGroup = A3000004 /* Products */;
			projectDirPath = "";
			projectRoot = "";
			targets = (
				A4000001 /* AIC8800WiFi */,
				A4000002 /* AIC8800WiFi_DEXT */,
			);
		};
/* End PBXProject section */

/* Begin PBXSourcesBuildPhase section */
		A5000001 /* Sources */ = {
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
				A1000001 /* AppDelegate.swift in Sources */,
				A1000002 /* ContentView.swift in Sources */,
				A1000003 /* StatusManager.swift in Sources */,
			);
			runOnlyForDeploymentPostprocessing = 0;
		};
		A5000002 /* Sources */ = {
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
				A1000004 /* AIC8800_USB.cpp in Sources */,
				A1000005 /* AIC8800_HALInit.cpp in Sources */,
				A1000006 /* AIC8800_NetIf.cpp in Sources */,
			);
			runOnlyForDeploymentPostprocessing = 0;
		};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
		A8000001 /* Debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++20";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = dwarf;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_TESTABILITY = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = YES;
				GCC_C_LANGUAGE_STANDARD = gnu17;
				GCC_DYNAMIC_NO_PIC = NO;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_OPTIMIZATION_LEVEL = 0;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=1",
					"$(inherited)",
				);
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 13.0;
				MTL_ENABLE_DEBUG_INFO = INCLUDE_SOURCE;
				MTL_FAST_MATH = YES;
				ONLY_ACTIVE_ARCH = YES;
				SDKROOT = macosx;
				SWIFT_ACTIVE_COMPILATION_CONDITIONS = DEBUG;
				SWIFT_OPTIMIZATION_LEVEL = "-Onone";
			};
			name = Debug;
		};
		A8000002 /* Release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++20";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
				ENABLE_NS_ASSERTIONS = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = YES;
				GCC_C_LANGUAGE_STANDARD = gnu17;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 13.0;
				MTL_FAST_MATH = YES;
				SDKROOT = macosx;
				SWIFT_COMPILATION_MODE = wholemodule;
				SWIFT_OPTIMIZATION_LEVEL = "-O";
			};
			name = Release;
		};
		A8000003 /* Debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				CODE_SIGN_ENTITLEMENTS = AIC8800_App/AIC8800_App.entitlements;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				INFOPLIST_FILE = AIC8800_App/Info.plist;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
				);
				PRODUCT_BUNDLE_IDENTIFIER = com.aic8800.wifi;
				PRODUCT_NAME = "$(TARGET_NAME)";
				SWIFT_EMIT_LOC_STRINGS = YES;
				SWIFT_VERSION = 5.0;
			};
			name = Debug;
		};
		A8000004 /* Release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				CODE_SIGN_ENTITLEMENTS = AIC8800_App/AIC8800_App.entitlements;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				INFOPLIST_FILE = AIC8800_App/Info.plist;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
				);
				PRODUCT_BUNDLE_IDENTIFIER = com.aic8800.wifi;
				PRODUCT_NAME = "$(TARGET_NAME)";
				SWIFT_EMIT_LOC_STRINGS = YES;
				SWIFT_VERSION = 5.0;
			};
			name = Release;
		};
		A8000005 /* Debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				CODE_SIGN_ENTITLEMENTS = AIC8800_DEXT/AIC8800_DEXT.entitlements;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				HEADER_SEARCH_PATHS = (
					"$(inherited)",
					"$(SDKROOT)/System/Library/Frameworks/DriverKit.framework/Headers",
					"$(SDKROOT)/System/Library/Frameworks/USBDriverKit.framework/Headers",
				);
				INFOPLIST_FILE = AIC8800_DEXT/Info.plist;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
					"@executable_path/../../../../Frameworks",
				);
				OTHER_LDFLAGS = (
					"-framework",
					"DriverKit",
					"-framework",
					"USBDriverKit",
				);
				PRODUCT_BUNDLE_IDENTIFIER = com.aic8800.wifi.dext;
				PRODUCT_NAME = "$(TARGET_NAME)";
				SKIP_INSTALL = YES;
			};
			name = Debug;
		};
		A8000006 /* Release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				CODE_SIGN_ENTITLEMENTS = AIC8800_DEXT/AIC8800_DEXT.entitlements;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				HEADER_SEARCH_PATHS = (
					"$(inherited)",
					"$(SDKROOT)/System/Library/Frameworks/DriverKit.framework/Headers",
					"$(SDKROOT)/System/Library/Frameworks/USBDriverKit.framework/Headers",
				);
				INFOPLIST_FILE = AIC8800_DEXT/Info.plist;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
					"@executable_path/../../../../Frameworks",
				);
				OTHER_LDFLAGS = (
					"-framework",
					"DriverKit",
					"-framework",
					"USBDriverKit",
				);
				PRODUCT_BUNDLE_IDENTIFIER = com.aic8800.wifi.dext;
				PRODUCT_NAME = "$(TARGET_NAME)";
				SKIP_INSTALL = YES;
			};
			name = Release;
		};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */
		A6000001 /* Build configuration list for PBXNativeTarget "AIC8800WiFi" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				A8000003 /* Debug */,
				A8000004 /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		};
		A6000002 /* Build configuration list for PBXNativeTarget "AIC8800WiFi_DEXT" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				A8000005 /* Debug */,
				A8000006 /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		};
		A6000003 /* Build configuration list for PBXProject "AIC8800WiFi" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				A8000001 /* Debug */,
				A8000002 /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		};
/* End XCConfigurationList section */

	};
	rootObject = A7000001 /* Project object */;
}
EOF

echo "Xcode project created successfully!"
echo ""

# Build instructions
echo "=== Build Instructions ==="
echo ""
echo "1. Open the project in Xcode:"
echo "   open AIC8800WiFi.xcodeproj"
echo ""
echo "2. Configure code signing:"
echo "   - Select AIC8800WiFi target"
echo "   - Go to Signing & Capabilities"
echo "   - Select your Development Team"
echo "   - Repeat for AIC8800WiFi_DEXT target"
echo ""
echo "3. Build the project:"
echo "   xcodebuild -scheme AIC8800WiFi -configuration Debug build"
echo ""
echo "4. Install the driver:"
echo "   - Run the AIC8800WiFi app"
echo "   - Click 'Install Driver'"
echo "   - Approve the system extension in System Settings"
echo ""
echo "5. Plug in the HOCO HI34 adapter"
echo "   - The driver should automatically detect and initialize the device"
echo "   - Use the app to scan and connect to WiFi networks"
echo ""
echo "=== Important Notes ==="
echo ""
echo "- You need a valid Apple Developer ID for distribution"
echo "- For development, you can use 'Personal Team' signing"
echo "- The driver requires macOS 13.0 or later"
echo "- Disable SIP (System Integrity Protection) for testing:"
echo "  csrutil disable --without kext --without dext"
echo ""
echo "=== Firmware ==="
echo ""
echo "The AIC8800 chip requires firmware files. These are typically"
echo "provided by the manufacturer and cannot be distributed."
echo "Place firmware files in the Firmware/aic8800D80/ directory."
echo ""
echo "For more information, see the README.md file."
