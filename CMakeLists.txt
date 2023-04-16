cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
if(WIN32)
    set(CMAKE_SYSTEM_VERSION 7.1 CACHE STRING INTERNAL FORCE) # Windows SDK for Windows 7 and up
endif()
project(ChowMultiTool VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(COMPANY_NAME "chowdsp")
set(PRODUCT_NAME "ChowMultiTool")

if(NOT IOS)
    option(CHOWMULTITOOL_BUILD_CLAP "Create a CLAP plugin target" ON)
else()
    option(CHOWMULTITOOL_BUILD_CLAP "Create a CLAP plugin target" OFF)
endif()

if(CHOWMULTITOOL_BUILD_CLAP AND (CMAKE_VERSION VERSION_LESS 3.21.0))
    message(WARNING "CLAP builds require CMake version 3.21 or higher! To disable CLAP builds, run CMake configuration with \"-DCHOWMULTITOOL_BUILD_CLAP=OFF\"")
endif()

add_subdirectory(modules)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK)
# juce_set_aax_sdk_path(NONE)

# set default plugin formats to build
if(IOS)
    set(JUCE_FORMATS Standalone AUv3)
else()
    set(JUCE_FORMATS AU VST3 Standalone)
endif()

# Build LV2 only on Linux
if(UNIX AND NOT APPLE)
    message(STATUS "Building LV2 plugin format")
    list(APPEND JUCE_FORMATS LV2)
endif()

# Build VST2 is SDK set
if(TARGET juce_vst2_sdk)
    message(STATUS "Building VST2 plugin format")
    list(APPEND JUCE_FORMATS VST)
endif()

# Build AAX if SDK target exists
if(TARGET juce_aax_sdk)
    message(STATUS "Building AAX plugin format")
    list(APPEND JUCE_FORMATS AAX)
endif()

juce_add_plugin(ChowMultiTool
    COMPANY_NAME "${COMPANY_NAME}"
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Mlt4
    FORMATS ${JUCE_FORMATS}
    PRODUCT_NAME "${PRODUCT_NAME}"
    ICON_BIG res/logo.png
    
    LV2URI https://github.com/Chowdhury-DSP/ChowMultiTool
    
    MICROPHONE_PERMISSION_ENABLED TRUE
    REQUIRES_FULL_SCREEN TRUE
    IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
    IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
)

if(CHOWMULTITOOL_BUILD_CLAP)
    clap_juce_extensions_plugin(
        TARGET ChowMultiTool
        CLAP_ID "org.chowdsp.ChowMultiTool"
        CLAP_FEATURES audio-effect
        CLAP_PROCESS_EVENTS_RESOLUTION_SAMPLES 64
        CLAP_USE_JUCE_PARAMETER_RANGES DISCRETE
    )
endif()

add_subdirectory(src)
include_directories(src)
add_subdirectory(res)

target_compile_definitions(ChowMultiTool PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ChowMultiTool
    PRIVATE
        juce_plugin_modules
        ChowMultiTool::gui-resources
        ChowMultiTool::presets-resources
)

# we need these flags for notarization on MacOS
option(MACOS_RELEASE "Set build flags for MacOS Release" OFF)
if(MACOS_RELEASE)
    message(STATUS "Setting MacOS release flags...")
    set_target_properties(ChowMultiTool_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()
