set(CMAKE_AUTOMOC ON)

add_executable(style_gui
    main.cpp
    CatalogWindow.cpp CatalogWindow.h
    OutfitPanel.cpp OutfitPanel.h
    GuiCommon.h)
target_link_libraries(style_gui PRIVATE style_core Qt6::Widgets)

# Veri ve görselleri çalışma dizininden bağımsız bulmak için kaynak klasör yolu
target_compile_definitions(style_gui PRIVATE STYLE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")