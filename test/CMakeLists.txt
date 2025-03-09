include(ExternalProject)
set(GTEST_VERSION "1.12.1")

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

add_subdirectory(front)
add_subdirectory(IR)
add_subdirectory(utils)

