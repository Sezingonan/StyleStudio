set(CMAKE_AUTOMOC ON)

add_executable(style_gui main.cpp)
target_link_libraries(style_gui PRIVATE style_core Qt6::Widgets)

# Veri dosyalarını çalışma dizininden bağımsız bulmak için kaynak klasör yolu
target_compile_definitions(style_gui PRIVATE STYLE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")