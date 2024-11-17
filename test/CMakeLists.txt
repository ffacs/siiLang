include(ExternalProject)
set(GTEST_VERSION "1.12.1")

file(GLOB sc_test_src
    "*.cpp")

list(LENGTH sc_test_src TEST_NUM)
if(NOT TEST_NUM EQUAL 0)

  ##################################
  # Download and install GoogleTest

  set(GTEST_SRC_URL "https://github.com/google/googletest/archive/release-${GTEST_VERSION}.tar.gz")
  set(GTEST_PREFIX "${CMAKE_BINARY_DIR}/googletest")
  set(GTEST_INCLUDE_DIR "${GTEST_PREFIX}/include")
  set(GMOCK_STATIC_LIB "${GTEST_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set(GTEST_STATIC_LIB "${GTEST_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set(GTEST_MAIN_STATIC_LIB "${GTEST_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}")

  set(GTEST_CMAKE_CXX_FLAGS "")

  set(GTEST_CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                     -DCMAKE_INSTALL_PREFIX=${GTEST_PREFIX}
                     -DCMAKE_INSTALL_LIBDIR=lib
                     -Dgtest_force_shared_crt=ON
                     -DCMAKE_CXX_FLAGS=${GTEST_CMAKE_CXX_FLAGS})

  ExternalProject_Add(gtest
    BUILD_IN_SOURCE 1
    URL ${GTEST_SRC_URL}
    CMAKE_ARGS ${GTEST_CMAKE_ARGS}
    BUILD_BYPRODUCTS "${GMOCK_STATIC_LIB}" "${GTEST_STATIC_LIB}" "${GTEST_MAIN_STATIC_LIB}")

  add_library(sc_test_lib INTERFACE)
  add_dependencies(sc_test_lib gtest)
  target_link_libraries(sc_test_lib INTERFACE ${GTEST_MAIN_STATIC_LIB})
  target_link_libraries(sc_test_lib INTERFACE ${GTEST_STATIC_LIB})
  target_link_libraries(sc_test_lib INTERFACE ${GMOCK_STATIC_LIB})
  target_include_directories(sc_test_lib INTERFACE ${GTEST_INCLUDE_DIR})

  add_executable(sc_test ${sc_test_src})
  add_dependencies(sc_test sc_test_lib)
  target_link_libraries(sc_test sc_test_lib sc_lib_static)
endif()
