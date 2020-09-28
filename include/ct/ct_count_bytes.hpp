#ifndef CT__COUNT_BYTES_HPP
#define CT__COUNT_BYTES_HPP

#include "ct/utils/ct_utils_accumulate.hpp"

namespace ct {

namespace impl {

// Default implementation for POD-types
template <typename T>
struct bytes_count_trait
{
    static_assert(std::is_pod<T>::value == true, "T must be POD type");

    static constexpr std::size_t bytes_count = sizeof (T);
};

// Specialization for raw array
template <typename T, std::size_t SIZE>
struct bytes_count_trait< T[SIZE] >
{
    static constexpr std::size_t bytes_count
        = SIZE * bytes_count_trait<T>::bytes_count;
};

// Specialization for std::array
template <typename T, std::size_t Size>
struct bytes_count_trait< std::array<T, Size> >
{
    static constexpr std::size_t bytes_count
        = Size * bytes_count_trait<T>::bytes_count;
};

// Specialization for std::pair
template <typename First, typename Second>
struct bytes_count_trait< std::pair<First, Second> >
{
    static constexpr std::size_t bytes_count
        = bytes_count_trait<First>::bytes_count + bytes_count_trait<Second>::bytes_count;
};

// Specialization for std::tuple
template <typename ... Types>
struct bytes_count_trait< std::tuple<Types...> >
{
    static constexpr std::size_t bytes_count
        = ct::utils::accumulate( { bytes_count_trait<Types>::bytes_count ...}, std::size_t{0});
};

} // namespace impl

// -----------------------------------------------------------------------------

template <typename ... Types>
constexpr std::size_t get_bytes_count() {
    return ct::utils::accumulate( { impl::bytes_count_trait<Types>::bytes_count ...}, std::size_t{0});
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests
{
    // POD types check
    static_assert( get_bytes_count<std::int8_t>()  == 1, "Test failed");
    static_assert( get_bytes_count<std::int16_t>() == 2, "Test failed");
    static_assert( get_bytes_count<std::int32_t>() == 4, "Test failed");

    // -------------------------------------------------------------------------
    // Simple raw arrays check
    static_assert( get_bytes_count< std::int8_t[3]   >() == (3*1),  "Test failed");
    static_assert( get_bytes_count< std::int16_t[5]  >() == (5*2),  "Test failed");
    static_assert( get_bytes_count< std::int32_t[10] >() == (10*4), "Test failed");

    // Nested std::arrays in raw arrays check
    static_assert( get_bytes_count< std::array<std::int8_t,  2>[3]  >() == (1*2*3),  "Test failed");
    static_assert( get_bytes_count< std::array<std::int16_t, 4>[5]  >() == (2*4*5),  "Test failed");
    static_assert( get_bytes_count< std::array<std::int32_t, 6>[10] >() == (4*6*10), "Test failed");

    // -------------------------------------------------------------------------
    // Simple std::array size check
    static_assert( get_bytes_count< std::array<std::int8_t,   3> >() == (1*3),  "Test failed");
    static_assert( get_bytes_count< std::array<std::int16_t,  5> >() == (2*5),  "Test failed");
    static_assert( get_bytes_count< std::array<std::int32_t, 10> >() == (4*10), "Test failed");

    // Nested std::arrays sizes checking
    static_assert( get_bytes_count< std::array< std::array<std::int8_t,  2>, 3> >() == (1*2*3), "Test failed");
    static_assert( get_bytes_count< std::array< std::array<std::int16_t, 3>, 5> >() == (2*3*5), "Test failed");
    static_assert( get_bytes_count< std::array< std::array<std::int32_t, 4>, 7> >() == (4*4*7), "Test failed");

    // Extreme nested std::array case
    static_assert( get_bytes_count< std::array<std::array<std::array<std::array<std::int32_t,5>,6>,7>,8> >() == (4*5*6*7*8), "Test failed");

    // -------------------------------------------------------------------------

    // Simple pairs check
    static_assert( get_bytes_count< std::pair<std::int8_t,  std::int16_t> >() == (1+2), "Test failed");
    static_assert( get_bytes_count< std::pair<std::int16_t, std::int32_t> >() == (2+4), "Test failed");
    static_assert( get_bytes_count< std::pair<std::int32_t, std::int64_t> >() == (4+8), "Test failed");

    // Nested arrays in pairs check
    static_assert( get_bytes_count< std::pair< std::array<std::int8_t,  2>, std::int16_t> >() == ((1*2)+2), "Test failed");
    static_assert( get_bytes_count< std::pair< std::array<std::int16_t, 4>, std::int32_t> >() == ((2*4)+4), "Test failed");
    static_assert( get_bytes_count< std::pair< std::array<std::int32_t, 6>, std::int64_t> >() == ((4*6)+8), "Test failed");

    // Extreme nested std::tuple case
    static_assert( get_bytes_count< std::tuple<std::int8_t, std::array<std::int16_t, 3>, std::pair<std::int32_t, std::int64_t>> >() == (1+(2*3)+(4+8)), "Test failed");

    // -------------------------------------------------------------------------
    // Parameters pack test

    static_assert( get_bytes_count< std::int8_t, std::int16_t, std::int32_t >() == (1+2+4), "Test failed");


} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace ct

#endif // CT__COUNT_BYTES_HPP
