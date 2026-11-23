add_library(style_core STATIC
    Version.cpp
    models/Enums.cpp
    models/ClothingCollection.cpp
    storage/JsonManager.cpp
)

target_include_directories(style_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# nlohmann/json: tek header'lık üçüncü parti kütüphane
target_include_directories(style_core PUBLIC ${CMAKE_SOURCE_DIR}/third_party)