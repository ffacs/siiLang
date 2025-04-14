file(GLOB sc_ir_test_src "*.cpp")

add_executable(sc_ir_test ${sc_ir_test_src})
add_dependencies(sc_ir_test gtest)

target_link_libraries(sc_ir_test PUBLIC
    sc_ir_lib_static
    ${GTEST_MAIN_STATIC_LIB}
    ${GTEST_STATIC_LIB}
    ${GMOCK_STATIC_LIB}
)

target_include_directories(sc_ir_test PUBLIC
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/test/IR
)

# Add test configuration
enable_testing()
add_test(NAME sc_ir_test COMMAND sc_ir_test)
