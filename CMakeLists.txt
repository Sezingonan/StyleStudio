cmake_minimum_required(VERSION 3.16)
project(StyleStudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/cli)
add_subdirectory(src/tools)
add_subdirectory(tests)

# Qt isteğe bağlı: bulunamazsa GUI atlanır, geri kalan her şey derlenir.
find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    add_subdirectory(src/gui)
else()
    message(STATUS "Qt6 bulunamadi, GUI atlandi (CMAKE_PREFIX_PATH ile Qt yolunu ver)")
endif()