file(GLOB sc_front_test_src
    "*.cpp")

add_executable(sc_front_test ${sc_front_test_src})
add_dependencies(sc_front_test gtest)
target_link_libraries(sc_front_test PUBLIC sc_front_lib_static)
target_link_libraries(sc_front_test PUBLIC ${GTEST_MAIN_STATIC_LIB})
target_link_libraries(sc_front_test PUBLIC ${GTEST_STATIC_LIB})
target_link_libraries(sc_front_test PUBLIC ${GMOCK_STATIC_LIB})
target_include_directories(sc_front_test PUBLIC ${GTEST_INCLUDE_DIR})
