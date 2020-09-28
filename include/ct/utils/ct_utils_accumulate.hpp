#ifndef CT__UTILS__ACCUMULATE_HPP
#define CT__UTILS__ACCUMULATE_HPP

#include <array> // for std::array<T, SIZE>

namespace ct {

namespace utils {

/*
    Note: implementation inspired by:
        - https://stackoverflow.com/a/42223988/
        - https://en.cppreference.com/w/cpp/algorithm/accumulate
*/

// Overloading for raw array
template <typename T, std::size_t SIZE>
constexpr T accumulate(const T (&array)[SIZE], const T& init, std::size_t idx = 0U)
{
    return (idx < SIZE) ? (array[idx] + accumulate<T, SIZE>(array, init, idx+1U)) : init;
}

// Overloading for std::array
template <typename T, std::size_t SIZE>
constexpr T accumulate(const std::array<T, SIZE>& array, const T& init, std::size_t idx = 0U)
{
    return (idx < SIZE) ? (array[idx] + accumulate<T, SIZE>(array, init, idx+1U)) : init;
}

// Overloading for std::initializer_list
template <typename T>
constexpr T accumulate(const std::initializer_list<T>& list, const T& init, std::size_t idx = 0U)
{
    return (idx < list.size()) ? ( *(list.begin() + idx) + accumulate<T>(list, init, idx+1U)) : init;
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Raw array test
    static_assert( accumulate({1, 2, 3, 4, 5}, 0) == (1+2+3+4+5)+0, "Test failed");
    static_assert( accumulate({1, 2, 3, 4, 5}, 6) == (1+2+3+4+5)+6, "Test failed");

    // std::array test
    static_assert( accumulate( std::array<int, 5>{1, 2, 3, 4, 5}, 0) == (1+2+3+4+5)+0, "Test failed");
    static_assert( accumulate( std::array<int, 5>{1, 2, 3, 4, 5}, 6) == (1+2+3+4+5)+6, "Test failed");

    // Well ... really strange cases, when tests will be passed, even with non-fully inited arrays
    static_assert( accumulate( std::array<int, /*wrong_size=*/ 7>{1, 2, 3, 4, 5}, 0) == (1+2+3+4+5)+0, "Test failed");
    static_assert( accumulate( std::array<int, /*wrong_size=*/ 8>{1, 2, 3, 4, 5}, 6) == (1+2+3+4+5)+6, "Test failed");

    // std::initializer_list test
    static_assert( accumulate( std::initializer_list<int>{1, 2, 3, 4, 5}, 0) == (1+2+3+4+5)+0, "Test failed");
    static_assert( accumulate( std::initializer_list<int>{1, 2, 3, 4, 5}, 6) == (1+2+3+4+5)+6, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__ACCUMULATE_HPP
