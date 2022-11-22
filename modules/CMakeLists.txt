add_subdirectory(JUCE)
add_subdirectory(chowdsp_utils)

subproject_version(JUCE juce_version)
message(STATUS "VERSION for JUCE: ${juce_version}")
subproject_version(chowdsp_utils chowdsp_version)
message(STATUS "VERSION for chowdsp: ${chowdsp_version}")

include(cmake/WarningFlags.cmake)
add_library(juce_plugin_modules STATIC)
target_link_libraries(juce_plugin_modules
    PRIVATE
        BinaryData
        juce::juce_audio_utils
        juce::juce_dsp
        chowdsp::chowdsp_plugin_base
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        warning_flags
)

if(NOT IOS)
    add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)
    target_link_libraries(juce_plugin_modules
        PRIVATE
            chowdsp::chowdsp_clap_extensions
            clap_juce_extensions
    )
endif()

add_diagnostic_info(juce_plugin_modules)

target_compile_definitions(juce_plugin_modules
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_JACK=1
        JUCE_ALSA=1
        JucePlugin_Manufacturer="${COMPANY_NAME}"
        JucePlugin_VersionString="${CMAKE_PROJECT_VERSION}"
        JucePlugin_Name="${PRODUCT_NAME}"
        JUCE_MODAL_LOOPS_PERMITTED=0
    INTERFACE
        $<TARGET_PROPERTY:juce_plugin_modules,COMPILE_DEFINITIONS>
)

target_include_directories(juce_plugin_modules
    INTERFACE
        $<TARGET_PROPERTY:juce_plugin_modules,INCLUDE_DIRECTORIES>
)

set_target_properties(juce_plugin_modules PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    VISIBILITY_INLINES_HIDDEN TRUE
    C_VISBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
)
