# WIP: Compile-Time utilities library

<p align="center">
  <a href="http://hits.dwyl.com/inobelar/compiletime-utils">
    <img title="HitCount" src="http://hits.dwyl.com/inobelar/compiletime-utils.svg"/>
  </a>
</p>

Written with type-traits (structures, with specialization for types) with **extentibility by custom types** in mind.

Written for **c++11**, without such useful things like:
- `fold expression` - [since c++17](https://en.cppreference.com/w/cpp/language/fold)
- `variable declaration` & `if/switch/for` in `constexpr` functions
- `std::index_sequence` - [since c++14](https://en.cppreference.com/w/cpp/utility/integer_sequence)

For enabling in-place compile-time tests (useful during hacking) - add `CT_ENABLE_TESTS` define.

## Including into your project

This library is header-only (since most of the code is non-splitable template magic), so for usage in your codebase, simply add `/include/` directory into include search path.

For example:
- `CMake` - References: [How to properly add include directories with CMake](https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake), [CMake::include_directories()](https://cmake.org/cmake/help/latest/command/include_directories.html)
  - Add into `CMakeLists.txt`:
    ```cmake
    set(CT_LIB_INCLUDE_DIR <path/to/compiletime-utils/include/> )
    target_include_directories( <YOUR_TARGET_NAME> PRIVATE ${CT_LIB_INCLUDE_DIR})
    ```
- `QMake` - In `project_name.pro` file add next line:
  - Add all includes as part of the project:
    ```qmake
    include(<path/to/compiletime-utils/include>/ct.pri)
    ```
  - Add all includes simply into search path:
    ```qmake
    INCLUDEPATH += <path/to/compiletime-utils/include/>
    ```
- `gcc`/`g++` single-file compilation:
   - `$ gcc -c -std=c++11 -O2 -I <path/to/compiletime-utils/include> -o <app_name> main.cpp`

## [Serialization](./include/ct/serialization/README.md)
