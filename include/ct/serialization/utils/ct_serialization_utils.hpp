#ifndef CT__SERIALIZATION__UTILS_HPP
#define CT__SERIALIZATION__UTILS_HPP

#include "ct/ct_count_bytes.hpp"

#include "ct/ct_flatten_trait.hpp"
#include "ct/serialization/utils/ct_serialization_utils_offsets.hpp"

#include "ct/serialization/utils/ct_serialization_utils_memcpy_values_count.hpp"

#include "ct/ct_test_equal.hpp"


namespace ct {

namespace serialization {

// -----------------------------------------------------------------------------

template <typename T>
struct sizeofs_maker {};

template <typename ... Types>
struct sizeofs_maker< ct::utils::List<Types...> >
{
    using list_t = ct::utils::List<Types...>;
    static constexpr std::size_t count = list_t::count;
    using sizeofs_array_t = std::array<std::size_t, count>;

    static constexpr sizeofs_array_t get_sizeofs()
    {
        return sizeofs_array_t { ct::get_bytes_count<Types>() ... };
    }

};

#if defined(CT_ENABLE_TESTS)
namespace tests {

using input_t = std::tuple< std::pair<std::int16_t, std::int16_t>, std::int16_t, std::array<std::int32_t,4> >;
constexpr auto sizeofs = sizeofs_maker< ct::flattened< input_t >::type >::get_sizeofs();

static_assert( ct::equal( sizeofs, std::array<std::size_t, 4>{2,2,2, 16}), "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

// -----------------------------------------------------------------------------

template <typename ... Types>
struct flat_offsets_maker
{
    // Convenient shortcut-aliases
    using sizeofs_maker_t = sizeofs_maker< typename ct::flattened<Types...>::type >;

    static constexpr std::size_t count = sizeofs_maker_t::count;
    using offsets_array_t = typename sizeofs_maker_t::sizeofs_array_t; // same as sizeofs array

    static constexpr offsets_array_t get_flat_offsets()
    {
        return utils::offsets::offsets_utils<count>::calc_offsets( sizeofs_maker_t::get_sizeofs() );
    }
};

#if defined(CT_ENABLE_TESTS)
namespace tests {

static_assert( ct::equal( flat_offsets_maker<input_t>::get_flat_offsets(), std::array<std::size_t, 4>{0, 2, 4, 6}), "Test failed");

static_assert( ct::equal( flat_offsets_maker< std::array< std::pair<std::int16_t, std::int32_t>, 2> >::get_flat_offsets(), std::array<std::size_t, 4>{0, 2, 6, 8}), "Test failed");

static_assert( ct::equal( flat_offsets_maker< std::array< std::pair< std::array<std::int8_t,3>, std::int32_t>, 2> >::get_flat_offsets(), std::array<std::size_t, 4>{0, 3, 7, 10}), "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

// -----------------------------------------------------------------------------

namespace utils {

template <typename ... Types>
struct types_sizeofs_info
{
    using flat_offsets_maker_t = flat_offsets_maker<Types...>;

    using byte_t = std::int8_t;
    static constexpr std::size_t bytes_count = ct::get_bytes_count<Types...>();
    using byte_buffer_t = std::array<byte_t, bytes_count>;


    // Primary function, for getting flattened offsets for <Types...>
    static constexpr auto get_offsets()
        -> typename flat_offsets_maker_t::offsets_array_t
    {
        return flat_offsets_maker_t::get_flat_offsets();
    }

    // Utility function for debug puprose - for getting sizeofs for <Types...>
    static constexpr auto get_sizeofs()
        -> typename flat_offsets_maker_t::sizeofs_maker_t::sizeofs_array_t
    {
        return flat_offsets_maker_t::sizeofs_maker_t::get_sizeofs();
    }

};

} // namespace utils

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__UTILS_HPP
