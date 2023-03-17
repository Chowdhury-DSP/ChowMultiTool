include(../modules/cmake/CMakeRC.cmake)

file(GLOB SVG_FILES gui_assets/Vector/*.svg)
file(GLOB TTF_FILES gui_assets/Fonts/*.ttf)
#message(STATUS "SVG files: ${SVG_FILES}")
cmrc_add_resource_library(gui-resources
    ALIAS ChowMultiTool::gui-resources
    NAMESPACE gui
    WHENCE gui_assets
    ${SVG_FILES}
    ${TTF_FILES}
)

set_target_properties(gui-resources
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
)

file(GLOB PRESET_FILES presets/*.chowpreset)
#message(STATUS "Preset files: ${PRESET_FILES}")
cmrc_add_resource_library(presets-resources
    ALIAS ChowMultiTool::presets-resources
    NAMESPACE presets
    WHENCE presets
    ${PRESET_FILES}
)

set_target_properties(presets-resources
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
)
