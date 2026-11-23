add_library(style_core STATIC
    Version.cpp
    models/Enums.cpp
    models/ClothingCollection.cpp
)

target_include_directories(style_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})