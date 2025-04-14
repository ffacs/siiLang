cmake_minimum_required(VERSION 3.22)

project (siiLang)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    elseif(${CMAKE_CXX_STANDARD} VERSION_LESS 17)
    message(FATAL_ERROR "Cannot set a CMAKE_CXX_STANDARD smaller than 17")
endif()
  
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_TESTING "Enable unit test build" ON)

include_directories(include)
add_subdirectory(src)

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

add_executable(sc main.cpp)
target_link_libraries(sc sc_front_lib_shared)
target_link_libraries(sc sc_ir_lib_shared)

set(FORMAT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/format/run-format.sh)

add_custom_target(
    format
    COMMAND ${FORMAT_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running code formatter..."
)

add_custom_target(
    check-format
    COMMAND ${FORMAT_SCRIPT} --dry-run
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking code formatting..."
)
