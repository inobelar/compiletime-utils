#ifndef CT__BYTES_UTILS_HPP
#define CT__BYTES_UTILS_HPP

#include "ct/ct_count_bytes.hpp"

namespace ct {

// via: https://stackoverflow.com/a/53281524/
template <typename ... Args>
struct bytes_utils
{
    static constexpr std::size_t items_count = sizeof...(Args);

    using sizes_array_t = std::array<std::size_t, items_count>;

    static constexpr sizes_array_t sizeofs() {
        return sizes_array_t{ ct::get_bytes_count<Args>() ...};
    }

    static constexpr std::size_t bytes_count = ct::utils::accumulate(sizeofs(), std::size_t{0});
};

#if defined(CT_ENABLE_TESTS)
namespace test {

    static_assert( bytes_utils<std::int8_t>::bytes_count  == 1, "Test failed");
    static_assert( bytes_utils<std::int16_t>::bytes_count == 2, "Test failed");
    static_assert( bytes_utils<std::int32_t>::bytes_count == 4, "Test failed");

    static_assert( bytes_utils<float>::bytes_count  == 4, "Test failed");
    static_assert( bytes_utils<double>::bytes_count == 8, "Test failed");

    // raw test
    static_assert( bytes_utils< std::int8_t  [3] >::bytes_count ==    3, "Test failed");
    static_assert( bytes_utils< std::int16_t [5] >::bytes_count ==  2*5, "Test failed");
    static_assert( bytes_utils< std::int32_t[10] >::bytes_count == 4*10, "Test failed");

    // std::array test
    static_assert( bytes_utils< std::array<std::int8_t ,  3> >::bytes_count ==    3, "Test failed");
    static_assert( bytes_utils< std::array<std::int16_t,  5> >::bytes_count ==  2*5, "Test failed");
    static_assert( bytes_utils< std::array<std::int32_t, 10> >::bytes_count == 4*10, "Test failed");

    // Nested items test
    static_assert( bytes_utils< std::tuple< std::pair< std::array<std::int16_t, 3>, std::int32_t>, std::array<std::int8_t, 5>, std::int64_t> >::bytes_count == ((2*3)+4) + (1*5) + 8, "Test failed");

    // Parameter pack test
    static_assert( bytes_utils<std::int8_t, std::pair<std::int16_t, std::int8_t>, std::int32_t>::bytes_count == (1+(2+1)+4), "Test failed");

} // namespace test
#endif // defined(CT_ENABLE_TESTS)

} // namespace ct

#endif // CT__BYTES_UTILS_HPP
