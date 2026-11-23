function(add_style_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE style_core)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_style_test(test_smoke)
add_style_test(test_enums)
