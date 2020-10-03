cmake_minimum_required(VERSION 3.5)

project(serialization_tests_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(serialization_tests_app
    ct_serialization_test.cpp
    rt_serialization_test.cpp
    main.cpp)

# ------------------------------------------------------------------------------

# Additionaly enable in-place compile-time tests
add_definitions(-DCT_ENABLE_TESTS)

# Note: How to get "directory above": https://stackoverflow.com/a/45650833/

# Add Catch2 include directory into include pathes
set(CATCH2_DIRECTORY get_filename_component(DIR_ONE_ABOVE ../catch2 ABSOLUTE) )
target_include_directories(serialization_tests_app PRIVATE ${CATCH2_DIRECTORY})

# Add Library include directory into include pathes
set(LIBRARY_DIRECTORY get_filename_component(DIR_ONE_ABOVE ../../include ABSOLUTE) )
target_include_directories(serialization_tests_app PRIVATE ${LIBRARY_DIRECTORY})
