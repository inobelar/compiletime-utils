#ifndef CT__UTILS__TYPELIST_FLATTEN_HPP
#define CT__UTILS__TYPELIST_FLATTEN_HPP

#include "ct/utils/typelist/ct_utils_typelist_join.hpp"

namespace ct {

namespace utils {

// via: https://stackoverflow.com/a/19840797/13057514

// The flatten metafunction guarantees (all specializations):
// the nested `type` is a flat `List<..>`
template <typename T>
struct flatten
{
    // Because of the partial specialization below, this primary template is
    // only used if `T` is not a `List<..>`
    using type = List<T>; // wrap the argument in a `List`
};

template <typename ... Types>
struct flatten< List<Types...> > // if the argument is a `List` of multiple elements
{
    // Then flatten each element of the `List` argument and join the
    // resulting `List<..>`s
    using type = typename join<typename flatten<Types>::type...>::type;

    // ex. the argument is `List<List<int>, List<double>>`
    // then `TT...` is deduced to `List<int>, List<double>`
    // `List<int>` flattened is `List<int>`, similarly for `List<double>`
    // `join< List<int>, List<double> >` yields `List<int, double>`
};

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Test for concatenation empty lists - produces empty list
    static_assert( std::is_same< flatten< List< List<>, List<> > >::type , List<> >::value, "Test failed");

    // Test for flattening multiple nested empty lists - produces empty list
    static_assert( std::is_same< flatten< List< List< List<>, List<> >, List<> > >::type , List<> >::value, "Test failed");

    // Tests for concatenation with empty list
    static_assert( std::is_same< flatten< List< List<bool, float>, List<> > >::type , List<bool, float> >::value, "Test failed");
    static_assert( std::is_same< flatten< List< List<>, List<bool, float> > >::type , List<bool, float> >::value, "Test failed");

    // Test for common concatenation
    static_assert( std::is_same< flatten< List< List<bool, short, float>, List<char, int, double> > >::type , List<bool, short, float, char, int, double> >::value, "Test failed");

    // Test for flattening multiple nested lists
    static_assert( std::is_same< flatten< List< List< List<bool>, List<short>, List<int> >, List<float, double> > >::type, List<bool, short, int, float, double> >::value, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__TYPELIST_FLATTEN_HPP
