cmake_minimum_required(VERSION 3.5)

project(ct_serialization_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ct_serialization_tests
    ct_serialization_test.cpp
    main.cpp)

# ------------------------------------------------------------------------------

# Additionaly enable in-place compile-time tests
add_definitions(-DCT_ENABLE_TESTS)

# Note: How to get "directory above": https://stackoverflow.com/a/45650833/

# Add Catch2 include directory into include pathes
set(CATCH2_DIRECTORY get_filename_component(DIR_ONE_ABOVE ../catch2 ABSOLUTE) )
target_include_directories(ct_serialization_tests PRIVATE ${CATCH2_DIRECTORY})

# Add Library include directory into include pathes
set(LIBRARY_DIRECTORY get_filename_component(DIR_ONE_ABOVE ../../include ABSOLUTE) )
target_include_directories(ct_serialization_tests PRIVATE ${LIBRARY_DIRECTORY})
