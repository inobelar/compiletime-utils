#ifndef CT__UTILS__TYPELIST_REPEAT_HPP
#define CT__UTILS__TYPELIST_REPEAT_HPP

#include "ct/utils/typelist/ct_utils_typelist_join.hpp"

namespace ct {

namespace utils {

// via: https://stackoverflow.com/a/33511913/

template<typename T, std::size_t N>
struct repeat
{
    using left  = typename repeat<T, N/2>::type;
    using right = typename repeat<T, N/2 + N%2>::type;
    using type  = typename join<left, right>::type;
};


// Explicit specialization for single type (to stop recursion)
template <typename T>
struct repeat<T, 1>
{
    using type = List<T>;
};

// Explicit specialization for none type (to stop recursion)
template <typename T>
struct repeat<T, 0>
{
    using type = List<>;
};

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Test for generating empty typeist (zero size)
    static_assert(std::is_same< repeat<int, 0>::type, List<> >::value, "Test failed");

    // Test for generating single-type typelist
    static_assert(std::is_same< repeat<int, 1>::type, List<int> >::value, "Test failed");

    // Test for generating typelist with N same types
    static_assert(std::is_same< repeat<int, 2>::type, List<int, int> >::value, "Test failed");
    static_assert(std::is_same< repeat<int, 3>::type, List<int, int, int> >::value, "Test failed");
    static_assert(std::is_same< repeat<int, 4>::type, List<int, int, int, int> >::value, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__TYPELIST_REPEAT_HPP
