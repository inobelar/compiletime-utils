#ifndef CT__UTILS__TYPELIST_FIRST_TYPES_HPP
#define CT__UTILS__TYPELIST_FIRST_TYPES_HPP

#include "ct/utils/typelist/ct_utils_typelist.hpp"

#if defined(CT_ENABLE_TESTS)
    #include <type_traits> // for std::is_same<T, U>::value
#endif

namespace ct {

namespace utils {

// Based on: https://stackoverflow.com/a/17508225/

template<int n, typename In, typename... Out>
struct first_impl;

template<int n, typename First, typename... Other, typename... Out>
struct first_impl<n, List<First, Other...>, Out...> {
    using type = typename first_impl<n - 1, List<Other...>, Out..., First>::type; //move first input to output.
};

//need First, Other... here to resolve ambiguity on n = 0
template<typename First, typename... Other, typename... Out>
struct first_impl<0, List<First, Other...>, Out...> {
    using type = List<Out...> ;  //stop if no more elements needed
};

// Explicit rule for empty tuple because of First, Other... in the previous rule.
// actually it's for N == size of tuple
template <typename ... Out>
struct first_impl<0, List<>, Out...> {
    using type = List<Out...>;
};

/// -----------------------

template<int n, typename T>
struct first_types {
    using type = typename first_impl<n, T>::type;
};

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    static_assert(std::is_same< first_types<0, List<> >::type, List<> >::value, "Test failed");
    static_assert(std::is_same< first_types<0, List<bool, char, int, float> >::type, List<> >::value, "Test failed");
    static_assert(std::is_same< first_types<1, List<bool, char, int, float> >::type, List<bool> >::value, "Test failed");
    static_assert(std::is_same< first_types<2, List<bool, char, int, float> >::type, List<bool, char> >::value, "Test failed");
    static_assert(std::is_same< first_types<3, List<bool, char, int, float> >::type, List<bool, char, int> >::value, "Test failed");
    static_assert(std::is_same< first_types<4, List<bool, char, int, float> >::type, List<bool, char, int, float> >::value, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__TYPELIST_FIRST_TYPES_HPP
