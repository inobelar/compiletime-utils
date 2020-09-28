#ifndef CT__UTILS__TYPELIST_HPP
#define CT__UTILS__TYPELIST_HPP

#include <cstdint> // for std::size_t

namespace ct {

namespace utils {

template <typename ... Types>
struct List {
    static constexpr std::size_t count = sizeof...(Types);
};

// -----------------------------------------------------------------------------
#if defined(CT_ENABLE_TESTS)
namespace tests {

    // List elements counts test
    static_assert(List<>::count == 0, "Test failed");
    static_assert(List<bool>::count == 1, "Test failed");
    static_assert(List<bool, char>::count == 2, "Test failed");
    static_assert(List<bool, char, int>::count == 3, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__TYPELIST_HPP
