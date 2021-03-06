#ifndef CT__COUNT_VALUES_HPP
#define CT__COUNT_VALUES_HPP

#include "ct/utils/ct_utils_accumulate.hpp"

/*
    Important note : for collections, like raw arrays or std::arrays, returned
    values count is always items count, even for arrays of POD-types.

    This is typical miss-understanding (even for me - the author of that code),
    that simple non-nested and non-composite arrays, which may be interpreted
    like a single thing (just because of thinking about it like about single
    piece of memory), always being recursively traversed item-by-item.

    TLDR: `values_count( std::array<int, 3> ) == 3`, not `1`.
*/

namespace ct {

namespace impl {

template <typename T>
struct count_values_trait
{
    static_assert(std::is_pod<T>::value == true, "T must be a POD type");

    static constexpr std::size_t values_count = 1;
};

// Specialization for raw array
template <typename T, std::size_t SIZE>
struct count_values_trait< T[SIZE] >
{
    static constexpr std::size_t values_count
        = SIZE * count_values_trait<T>::values_count;
};

// Specialization for std::array
template <typename T, std::size_t SIZE>
struct count_values_trait< std::array<T, SIZE> >
{
    static constexpr std::size_t values_count
        = SIZE * count_values_trait<T>::values_count;
};

// Specialization for std::pair
template <typename First, typename Second>
struct count_values_trait< std::pair<First, Second> >
{
    static constexpr std::size_t values_count
        = count_values_trait<First>::values_count + count_values_trait<Second>::values_count;
};

// Specialization for std::tuple
template <typename ... Types>
struct count_values_trait< std::tuple<Types...> >
{
    static constexpr std::size_t values_count
        = ct::utils::accumulate( { count_values_trait<Types>::values_count ...}, std::size_t{0});
};

} // namespace impl

// -----------------------------------------------------------------------------

template <typename ... Types>
constexpr std::size_t get_values_count() {
    return ct::utils::accumulate( { impl::count_values_trait<Types>::values_count ...}, std::size_t{0});
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // POD types check
    static_assert( get_values_count<std::int32_t>() == 1, "Test failed");

    // -------------------------------------------------------------------------
    // Simple raw arrays check
    static_assert( get_values_count< std::int32_t[3] >() == 3,  "Test failed");

    // Nested std::arrays in raw arrays check
    static_assert( get_values_count< std::array<std::int32_t, 6>[10] >() == (6*10), "Test failed");

    // -------------------------------------------------------------------------
    // Simple std::array size check
    static_assert( get_values_count< std::array<std::int32_t, 10> >() == 10, "Test failed");

    // Nested std::arrays sizes checking
    static_assert( get_values_count< std::array< std::array<std::int32_t, 4>, 7> >() == (4*7), "Test failed");

    // Extreme nested std::array case
    static_assert( get_values_count< std::array<std::array<std::array<std::array<std::int32_t,5>,6>,7>,8> >() == (5*6*7*8), "Test failed");

    // -------------------------------------------------------------------------

    // Simple std::pairs check
    static_assert( get_values_count< std::pair<std::int32_t, std::int64_t> >() == 2, "Test failed");

    // Nested std::pairs check
    static_assert( get_values_count< std::pair< std::array<std::int16_t,  2>, std::int32_t> >() == (2+1), "Test failed");
    static_assert( get_values_count< std::pair< std::pair<std::int16_t, std::int32_t>, std::pair< std::array<std::int64_t, 3>, std::int16_t> > >() == (2+3+1), "Test failed");

    // Simple std::tuple check
    static_assert( get_values_count< std::tuple<std::int8_t, std::int16_t, std::int32_t> >() == 3, "Test failed");

    // Extreme nested std::tuple case
    static_assert( get_values_count< std::tuple<std::int8_t, std::array<std::int16_t, 3>, std::pair<std::int32_t, std::int64_t> > >() == (1+3+2), "Test failed");

    // -------------------------------------------------------------------------

    // Parameters pack test
    static_assert( get_values_count<std::int8_t, std::int16_t, std::int32_t>() == 3, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace ct

#endif // CT__COUNT_VALUES_HPP
