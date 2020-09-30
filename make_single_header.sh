#!/usr/bin/env bash

# ----------------------------------------------------------
# Using `find` to get all header files
# 
# References:
#   - https://stackoverflow.com/a/5905066/
#     - find is the best approach for such things
#
#   - https://unix.stackexchange.com/questions/215234/explanation-of-directives-in-find-printf
#     - %P - File's name with the name of the starting-point under which it was found removed.
#            For making #include "file/name.hpp" instead of #include "./file/name.hpp" (in case of %p)
#            Also it is important for Quom below - includes must be 'local'-style (with ""), not 'global'-style (with <>)

find ./include/ -name "*.hpp" -printf "#include \"%P\"\n" > ./__all_headers.hpp


# ----------------------------------------------------------
# Using `Quom` for making single header-file
#
# References:
#   - https://softwarerecs.stackexchange.com/a/69491
#   - https://github.com/Viatorus/quom
# 
# Installation:
#   - $ pip3 install quom
#
# Important note: Quom produces a mess in cases like:
#   #if defined(CT_ENABLE_TESTS)
#     #include <ct/ct_test_equal.hpp>
#   #endif

python3 -m quom --include_directory ./include/ ./__all_headers.hpp ./ct_utils_lib.hpp


# ----------------------------------------------------------
# Remove temp file
rm ./__all_headers.hpp
