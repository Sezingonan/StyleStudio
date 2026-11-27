add_executable(style_gen gen_data.cpp)
target_link_libraries(style_gen PRIVATE style_core)

add_executable(style_bench bench.cpp)
target_link_libraries(style_bench PRIVATE style_core)