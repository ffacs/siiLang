file(GLOB sc_utils_test_src
    "*.cpp")

add_executable(sc_utils_test ${sc_utils_test_src})
add_dependencies(sc_utils_test gtest)
target_link_libraries(sc_utils_test PUBLIC ${GTEST_MAIN_STATIC_LIB})
target_link_libraries(sc_utils_test PUBLIC ${GTEST_STATIC_LIB})
target_link_libraries(sc_utils_test PUBLIC ${GMOCK_STATIC_LIB})
target_include_directories(sc_utils_test PUBLIC ${GTEST_INCLUDE_DIR})
