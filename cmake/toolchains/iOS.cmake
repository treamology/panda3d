set(IOS YES)

# Since we're building a library, we won't be needing codesigning
set(CMAKE_MACOSX_BUNDLE YES)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)

# CMake's compiler ABI checks fail if using an executable
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(IOS_PLATFORM "Device" CACHE STRING
  "Decides whether to build for iOS Simulator (Simulator) or a device
  (Device).")
set_property(CACHE IOS_PLATFORM PROPERTY STRINGS Simulator Device)
set(IOS_MIN_VERSION 11.0 CACHE STRING "Minimum required iOS version.")

# Determine the location of the Developer directory if not specified
if(${IOS_PLATFORM} STREQUAL Simulator)
  set(IOS_PLATFORM_NAME "iPhoneSimulator")
elseif(${IOS_PLATFORM} STREQUAL Device)
  set(IOS_PLATFORM_NAME "iPhoneOS")
else()
  message(FATAL_ERROR
    "Invalid IOS_PLATFORM. Valid options are Simulator or Device.")
endif()

exec_program(/usr/bin/xcode-select
  ARGS --print-path
  OUTPUT_VARIABLE XCODE_DEVELOPER_DIR)
set(IOS_DEVELOPER_DIR
  "${XCODE_DEVELOPER_DIR}/Platforms/${IOS_PLATFORM_NAME}.platform/Developer"
  CACHE PATH "Location of the iOS Developer Directory")

# If not specified, the iOS SDK defaults to the latest version.
set(IOS_SDK_ROOT "${IOS_DEVELOPER_DIR}/SDKs/${IOS_PLATFORM_NAME}.sdk"
CACHE PATH "Location of the iOS SDK")

set(CMAKE_OSX_SYSROOT ${IOS_SDK_ROOT})

if(${IOS_PLATFORM} STREQUAL Simulator)
  set(CMAKE_OSX_ARCHITECTURES x86_64 CACHE STRING "" FORCE)
  set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
elseif(${IOS_PLATFORM} STREQUAL Device)
  set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "" FORCE)
  set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
endif()

# Make sure the iOS versions of frameworks are found
set(CMAKE_FIND_ROOT_PATH ${IOS_DEVELOPER_DIR} ${IOS_SDK_ROOT}
  ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_FRAMEWORK FIRST)
set(CMAKE_SYSTEM_FRAMEWORK_PATH
  ${CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
  ${CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
  ${CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks)

# Needs to be set so clang knows to compile for iOS, even when using x86_64
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -miphoneos-version-min=${IOS_MIN_VERSION}")
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -miphoneos-version-min=${IOS_MIN_VERSION}")