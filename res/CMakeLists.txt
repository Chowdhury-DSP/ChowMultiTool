include(../modules/cmake/CMakeRC.cmake)

cmrc_add_resource_library(gui-resources
    ALIAS ChowMultiTool::gui-resources
    NAMESPACE gui
    redo-solid.svg
    undo-solid.svg
)
