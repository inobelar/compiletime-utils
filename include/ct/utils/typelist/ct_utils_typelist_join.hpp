#ifndef CT__UTILS__TYPELIST_JOIN_HPP
#define CT__UTILS__TYPELIST_JOIN_HPP

#include "ct/utils/typelist/ct_utils_typelist.hpp"

#if defined(CT_ENABLE_TESTS)
    #include<type_traits> // for std::is_same<T, U>::value
#endif

namespace ct {

namespace utils {

// via: https://stackoverflow.com/a/19840797/13057514

// A join metafunction. Joins multiple Lists into a single List
// e.g.    List<TT1...>, List<TT2...>, etc., List<TTN...>
//      => List<TT1..., TT2..., etc., TTN...>
// requires: all template arguments are `List<..>`s
template <typename ... Types>
struct join
{
    using type = List<>; // End recursion for no parameters
};

template <typename ... Types>
struct join< List<Types...> >
{
    using type = List<Types...>; // End recursion for a single parameter
};

template <typename ... TT0, typename ... TT1, typename ... TT2>
struct join<List<TT0...>, List<TT1...>, TT2...>
{
    // Join two adjacent lists into one, recurse by joining the two
    // lists `List<TT0...>` and `List<TT1...>`, we get one template argument
    // less for the next instantiation of `join` this recurs until there's
    // only one argument left, which then matches the
    // specialization `struct join< List<Types...> >`
    using type = typename join< List<TT0..., TT1...>, TT2... >::type;
};

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Test for joining empty lists - produces empty list
    static_assert(std::is_same< join< List<>, List<>, List<> >::type , List<> >::value, "Test failed");

    // Test for joining multiple nested empty lists - produces list with empty lists
    static_assert(std::is_same< join< List< List<>, List<> >, List<> >::type , List< List<>, List<> > >::value, "Test failed");

    // Tests for joining with empty list with common types
    static_assert(std::is_same< join< List<bool, float>, List<> >::type , List<bool, float> >::value, "Test failed");
    static_assert(std::is_same< join< List<>, List<bool, float> >::type , List<bool, float> >::value, "Test failed");

    // Test for common joining
    static_assert(std::is_same< join< List<bool, short, float>, List<char, int, double> >::type , List<bool, short, float, char, int, double> >::value, "Test failed");

    // Test for joining multiple nested lists with common types
    static_assert(std::is_same< join< List< List<bool>, List<short>, List<int> >, List<float, double> >::type, List<List<bool>, List<short>, List<int>, float, double> >::value, "Test failed");
    static_assert(std::is_same< join< List< List<bool, char>, List<short, int> >, List<float, double> >::type, List< List<bool, char>, List<short, int>, float, double> >::value, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__TYPELIST_JOIN_HPP
