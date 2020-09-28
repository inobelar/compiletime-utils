#ifndef CT__SERIALIZATION__UTILS__MEMCPY_VALUES_COUNT_HPP
#define CT__SERIALIZATION__UTILS__MEMCPY_VALUES_COUNT_HPP

#include "ct/utils/ct_utils_accumulate.hpp"

/*
    Important note: next trait below basically the same as
    `ct::impl::count_values_trait`, BUT behave differently for arrays.

    Because it is designed specially for usage in `pack_trait` & `unpack_trait`,
    for perofmance reason that trait check is array contains items of same
    `scalar` type - in that case returns values count: `1` - because such arrays
    may be SAFELY copied via `std::memcpy()` as single peace, or as single value.

    In other cases (arrays of non-scalar or nested/custom types) its returns
    items count, because we need to copy such array item-by-item.
 */

namespace ct {

namespace serialization {

namespace utils {

namespace impl {

template <typename T>
struct memcpy_values_count_trait
{
    static_assert(std::is_pod<T>::value == true, "T must be a POD type");

    static constexpr std::size_t values_count = 1;
};

// Specialization for raw array
template <typename T, std::size_t SIZE>
struct memcpy_values_count_trait< T[SIZE] >
{
    static constexpr std::size_t values_count
        = (std::is_scalar<T>::value == true) ?
            1
            :
            (SIZE * memcpy_values_count_trait<T>::values_count);
};

// Specialization for std::array
template <typename T, std::size_t SIZE>
struct memcpy_values_count_trait< std::array<T, SIZE> >
{
    static constexpr std::size_t values_count
        = (std::is_scalar<T>::value == true) ?
            1
            :
            (SIZE * memcpy_values_count_trait<T>::values_count);
};

// Specialization for std::pair
template <typename First, typename Second>
struct memcpy_values_count_trait< std::pair<First, Second> >
{
    static constexpr std::size_t values_count
        = memcpy_values_count_trait<First>::values_count + memcpy_values_count_trait<Second>::values_count;
};

// Specialization for std::tuple
template <typename ... Types>
struct memcpy_values_count_trait< std::tuple<Types...> >
{
    static constexpr std::size_t values_count
        = ct::utils::accumulate( { memcpy_values_count_trait<Types>::values_count ...}, std::size_t{0});
};

} // namespace impl

// -----------------------------------------------------------------------------

template <typename ... Types>
constexpr std::size_t get_memcpy_values_count() {
    return ct::utils::accumulate( {impl::memcpy_values_count_trait<Types>::values_count ...}, std::size_t{0});
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // POD types check
    static_assert( get_memcpy_values_count<std::int32_t>() == 1, "Test failed");

    // -------------------------------------------------------------------------
    // Simple raw arrays check
    static_assert( get_memcpy_values_count< std::int32_t[3] >() == 1,  "Test failed");

    // Nested std::arrays in raw arrays check
    static_assert( get_memcpy_values_count< std::array<std::int32_t, 6>[10] >() == (1*10), "Test failed");

    // -------------------------------------------------------------------------
    // Simple std::array size check
    static_assert( get_memcpy_values_count< std::array<std::int32_t, 10> >() == 1, "Test failed");

    // Nested std::arrays sizes checking
    static_assert( get_memcpy_values_count< std::array< std::array<std::int32_t, 4>, 7> >() == (1*7), "Test failed");

    // Extreme nested std::array case
    static_assert( get_memcpy_values_count< std::array<std::array<std::array<std::array<std::int32_t,5>,6>,7>,8> >() == (1*6*7*8), "Test failed");

    // -------------------------------------------------------------------------

    // Simple std::pairs check
    static_assert( get_memcpy_values_count< std::pair<std::int32_t, std::int64_t> >() == 2, "Test failed");

    // Nested std::pairs check
    static_assert( get_memcpy_values_count< std::pair< std::array<std::int16_t,  2>, std::int32_t> >() == (1+1), "Test failed");
    static_assert( get_memcpy_values_count< std::pair< std::pair<std::int16_t, std::int32_t>, std::pair< std::array<std::int64_t, 3>, std::int16_t> > >() == (2+1+1), "Test failed");

    // Simple std::tuple check
    static_assert( get_memcpy_values_count< std::tuple<std::int8_t, std::int16_t, std::int32_t> >() == 3, "Test failed");

    // Extreme nested std::tuple case
    static_assert( get_memcpy_values_count< std::tuple<std::int8_t, std::array<std::int16_t, 3>, std::pair<std::int32_t, std::int64_t> > >() == (1+1+2), "Test failed");

    // -------------------------------------------------------------------------

    // Parameters pack test
    static_assert( get_memcpy_values_count<std::int8_t, std::int16_t, std::int32_t>() == 3, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)


} // namespace utils

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__UTILS__MEMCPY_VALUES_COUNT_HPP
