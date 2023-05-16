add_subdirectory(JUCE)
add_subdirectory(chowdsp_utils)
add_subdirectory(chowdsp_wdf)
add_subdirectory(eigen)
add_subdirectory(LBFGSpp)

subproject_version(JUCE juce_version)
message(STATUS "VERSION for JUCE: ${juce_version}")
subproject_version(chowdsp_utils chowdsp_version)
message(STATUS "VERSION for chowdsp_utils: ${chowdsp_version}")
subproject_version(chowdsp_wdf chowdsp_wdf_version)
message(STATUS "VERSION for chowdsp_wdf: ${chowdsp_wdf_version}")

include(cmake/WarningFlags.cmake)
add_library(juce_plugin_modules STATIC)
target_link_libraries(juce_plugin_modules
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        chowdsp::chowdsp_plugin_base
        chowdsp::chowdsp_plugin_state
        chowdsp::chowdsp_plugin_utils
        chowdsp::chowdsp_presets_v2
        chowdsp::chowdsp_eq
        chowdsp::chowdsp_sources
        chowdsp::chowdsp_waveshapers
        chowdsp::chowdsp_wdf
        chowdsp::chowdsp_gui
        chowdsp::chowdsp_visualizers
        Eigen3::Eigen
        lbfgspp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        warning_flags
)

if(CHOWMULTITOOL_BUILD_CLAP)
    add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)
    target_link_libraries(juce_plugin_modules
        PRIVATE
            chowdsp::chowdsp_clap_extensions
            clap_juce_extensions
    )
endif()

option(CHOWMULTITOOL_BUILD_UI_INSPECTOR "Build the plugin with Melatonin Inspector" OFF)
if (CHOWMULTITOOL_BUILD_UI_INSPECTOR)
    juce_add_modules(ALIAS_NAMESPACE melatonin melatonin_inspector)
    target_link_libraries(juce_plugin_modules PRIVATE melatonin::melatonin_inspector)
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
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/exprtk
    INTERFACE
        $<TARGET_PROPERTY:juce_plugin_modules,INCLUDE_DIRECTORIES>
)

set_target_properties(juce_plugin_modules PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    VISIBILITY_INLINES_HIDDEN TRUE
    C_VISBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
)

# Enable OpenGL on Windows by default.
if (WIN32)
    option(CHOWDSP_ENABLE_OPENGL "Enable OpenGL for this plugin" ON)
else()
    option(CHOWDSP_ENABLE_OPENGL "Enable OpenGL for this plugin" OFF)
endif()

if(CHOWDSP_ENABLE_OPENGL)
    message(STATUS "Linking with OpenGL")
    target_link_libraries(juce_plugin_modules PRIVATE juce::juce_opengl)
endif()
