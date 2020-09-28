#ifndef CT__SERIALIZATION__UTILS__OFFSETS_HPP
#define CT__SERIALIZATION__UTILS__OFFSETS_HPP

#include "ct/utils/ct_utils_index_sequence.hpp"
#include "ct/ct_bytes_utils.hpp"

#if defined(CT_ENABLE_TESTS)
    #include "ct/ct_test_equal.hpp" // Only for tests
#endif

namespace ct {

namespace serialization {

namespace utils {

namespace offsets {

template <std::size_t COUNT>
struct offsets_utils
{
    static constexpr std::size_t count = COUNT;

    // Descriptive aliases
    using sizeofs_array_t = std::array<std::size_t, count>;
    using offsets_array_t = std::array<std::size_t, count>;

    /**
        Explanation of output:

        @code
        <i8, i32, i32> <-- types
        [ 1,   4,   4] <-- sizeofs
        [ 0,   1,   2] <-- indexes
        --------------
        [ 0,   1,   5] <-- results
        [idx<0>=0, idx<1>=(1+0), idx<2>=(4+1+0)] <-- offsets calculation
        @endcode

        @code
        <i64, i32, i16, i8, i16, i32, i64> <-- types
        [  8,   4,   2,  1,   2,   4,   8] <-- sizeofs
        [  0,   1,   2,  3,   4,   5,   6] <-- indexes
        ----------------------------------
        [  0,   8,  12, 14,  15,  17,  21] <-- result offsets
        [idx<0>=0, idx<1>=(8+0), idx<2>=(4+8+0) idx<3>=(2+4+8+0), idx<4>=(1+2+4+8+0) ...] <-- offsets calculation
        @endcode
    */
    static constexpr std::size_t accumulate_sizeofs(const offsets_array_t& sizeofs_array, std::size_t curr_idx)
    {
        // First element (at index 0) always have offset 0
        // Offsets calculation is done recursively, backwards (from `curr_idx` intil 0)
        return (curr_idx == 0) ? 0 : ( sizeofs_array[curr_idx-1] + accumulate_sizeofs(sizeofs_array, curr_idx-1) );
    }

    template <int ... Indexes>
    static constexpr offsets_array_t calc_offsets_impl(const sizeofs_array_t& sizeofs_array, ct::ind_seq::index<Indexes...> )
    {
        return offsets_array_t{ accumulate_sizeofs(sizeofs_array, Indexes) ...};
    }

    static constexpr offsets_array_t calc_offsets(const sizeofs_array_t& sizeofs)
    {
        return calc_offsets_impl(sizeofs, ct::ind_seq::gen_seq<count>{});
    }
};

// -----------------------------------------------------------------------------

template <
        typename ... Types,

        // Descriptive aliases
        std::size_t SIZEOFS_COUNT = sizeof... (Types),
        typename offsets_maker_t = offsets_utils<SIZEOFS_COUNT>,
        typename offsets_array_t = typename offsets_maker_t::offsets_array_t
        >
static constexpr offsets_array_t get_offsets()
{
    return offsets_maker_t::calc_offsets( ct::bytes_utils<Types...>::sizeofs() );
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Test for POD-types offsets
    static_assert( ct::equal( get_offsets<std::int8_t,  std::int16_t, std::int32_t>(), std::array<std::size_t, 3>{0, 1, 3}), "Test failed");
    static_assert( ct::equal( get_offsets<std::int32_t, std::int16_t, std::int8_t >(), std::array<std::size_t, 3>{0, 4, 6}), "Test failed");

    // Test for std::array offsets
    static_assert( ct::equal( get_offsets<std::array<std::int8_t,  5>, std::int16_t, std::int32_t>(), std::array<std::size_t, 3>{0, (1*5), (1*5)+2}), "Test failed");
    static_assert( ct::equal( get_offsets<std::array<std::int32_t, 3>, std::int16_t, std::int8_t >(), std::array<std::size_t, 3>{0, (4*3), (4*3)+2}), "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace offsets

} // namespace utils

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__UTILS__OFFSETS_HPP
