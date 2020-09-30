#ifndef CT__SERIALIZATION__UNPACK_HPP
#define CT__SERIALIZATION__UNPACK_HPP

#include "ct/serialization/utils/ct_serialization_utils.hpp"

#include "ct/utils/typelist/ct_utils_typelist_to_tuple.hpp"
#include "ct/utils/typelist/ct_utils_typelist_first_types.hpp"

#include <cstring> // for std::memcpy()

namespace ct {

namespace serialization {

/**
    Written as bunch of nested specialization traits, to make this extendable
    by custom types.

    @note For detailed description see packer_trait, since, historically,
    unpacking was written after it, and strictly mirrored/opposite to packing.

    # Extending by custom types

    namespace ct {
    namespace serialization {

    @code{.cpp}
    template<typename ... Types>
    template< GENERIC_TYPES_FOR YOUR_TYPE >
    struct unpacker_trait<Types...>::specialized_for< YOUR_TYPE , (OPTIONAL) RESTRICTION_EXPRESSION_FOR YOUR_TYPE >
    {
        using info_t = unpacker_trait<Types...>::info_t;
        using byte_t = typename info_t::byte_t;

        using value_t = YOUR_TYPE;

        template <std::size_t OFFSET_IDX>
        static void unpack(const byte_t* src, value_t& value)
        {
            // ... implementation ...
        }
    };

    } // namespace serialization
    } // namespace ct
    @endcode
*/

template <typename ... Types>
struct unpacker_trait
{
    using info_t = ct::serialization::utils::types_sizeofs_info<Types...>;

    /*
        We need to store all `specialized_for<T>::unpack()`-functions here, inside
        the template struct with parameter pack (paker_trait<Types...>), because
        we need to retreive compile-time offsets for all passed types.

        Unfortunately, due to usage of `std::memcpy()` (which, is not constexpr
        function in c++11), we cannot mark all that recursive `unpack()` functions
        like `constexpr` too :C
    */
    template <typename T, typename Enabled = void>
    struct specialized_for {};
};

// Specialization for: Single scalar type
template<typename ... Types>
template<typename T>
struct unpacker_trait<Types...>::specialized_for<T, typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T;

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& value)
    {
        constexpr std::size_t OFFSET = std::get<OFFSET_IDX>( info_t::get_offsets() );
        std::memcpy(&value, (src + OFFSET), sizeof(T) );
    }
};

namespace impl {

// Utility/common implementation for array types
template <typename ... Types>
struct same_items_unpacker
{
    using info_t = typename unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    template <std::size_t OFFSET_IDX, typename T, std::size_t COUNT>
    static auto unpack_scalar_array(const byte_t* src, T* items)
        -> typename std::enable_if< std::is_scalar<T>::value, void>::type
    {
        constexpr std::size_t OFFSET = std::get<OFFSET_IDX>( info_t::get_offsets() );
        std::memcpy(items, (src + OFFSET), (COUNT * sizeof(T)) );
    }

    template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE, int ... Indexes>
    static void unpack_non_scalar_array(const byte_t* src, T* item, ct::ind_seq::index<Indexes...>)
    {
        using dummy_t = int[];
        (void) dummy_t {
            ( unpacker_trait<Types...>::template specialized_for<T>::template unpack<OFFSET_IDX + (Indexes * utils::get_memcpy_values_count<T>()) >(src, item[Indexes]), /* for making dummy_t: */ 0) ...
        };
    }
};

} // namespace impl

// Specialization for: std::array with scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct unpacker_trait<Types...>::specialized_for< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::array<T, SIZE>;

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& array)
    {
        impl::same_items_unpacker<Types...>::template unpack_scalar_array<OFFSET_IDX, T, SIZE>(src, array.data());
    }
};

// Specialization for: raw array with scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct unpacker_trait<Types...>::specialized_for< T[SIZE], typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T[SIZE];

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& array)
    {
        impl::same_items_unpacker<Types...>::template unpack_scalar_array<OFFSET_IDX, T, SIZE>(src, &array, ct::ind_seq::gen_seq<SIZE>{});
    }
};

// Specialization for: std::array with non-scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct unpacker_trait<Types...>::specialized_for< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == false >::type>
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::array<T, SIZE>;

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& array)
    {
        impl::same_items_unpacker<Types...>::template unpack_non_scalar_array<OFFSET_IDX, T, SIZE>(src, array.data(), ct::ind_seq::gen_seq<SIZE>{});
    }
};

// Specialization for: raw array with no-scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct unpacker_trait<Types...>::specialized_for< T[SIZE], typename std::enable_if< std::is_scalar<T>::value == false >::type>
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T[SIZE];

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& array)
    {
        impl::same_items_unpacker<Types...>::template unpack_non_scalar_array<OFFSET_IDX, T, SIZE>(src, &array, ct::ind_seq::gen_seq<SIZE>{});
    }
};

// -----------------------------------------------------------------------------

// Specialization for: std::pair
template<typename ... Types>
template<typename First, typename Second>
struct unpacker_trait<Types...>::specialized_for< std::pair<First, Second> >
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::pair<First, Second>;

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& pair)
    {
        unpacker_trait<Types...>::template specialized_for<First >::template unpack<OFFSET_IDX +                              0>(src, pair.first);
        unpacker_trait<Types...>::template specialized_for<Second>::template unpack<OFFSET_IDX + utils::get_memcpy_values_count<First>()>(src, pair.second);
    }
};

// -----------------------------------------------------------------------------

// Convenient & most smart trait for pack (still recursive & nested) template
// parameter pack
template <typename ... Types>
struct values_unpacker
{
    using info_t = typename unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    template <std::size_t OFFSET_IDX, typename ... Args, int ... Indexes>
    static void unpack_values_impl(const byte_t* src, Args& ... values, ct::ind_seq::index<Indexes...>)
    {
        using dummy_t = int[];
        (void) dummy_t {
            ( unpacker_trait<Types...>::template specialized_for<Args>::template unpack
              <OFFSET_IDX + ( Indexes == 0 ? 0 :
                   // See description of it inside similar packer trait :)
                   (utils::get_memcpy_values_count<
                        typename ct::utils::list_to_tuple< typename ct::utils::first_types<Indexes, ct::utils::List<Args...>>::type >::type
                   >())
              )>(src, /* Item= */ values), /* for making dummy_t: */ 0) ...
        };
    }

    template <std::size_t OFFSET_IDX, typename ... Args>
    static void unpack_values(const byte_t* src, Args& ... values)
    {
        constexpr std::size_t ARGS_COUNT = sizeof...(Args);
        unpack_values_impl<OFFSET_IDX, Args...>(src, values..., ct::ind_seq::gen_seq<ARGS_COUNT>{});
    }

};

// Specialization for: std::tuple
template<typename ... Types>
template<typename ... TupleTypes>
struct unpacker_trait<Types...>::specialized_for< std::tuple<TupleTypes...> >
{
    using info_t = unpacker_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::tuple<TupleTypes...>;

    template <std::size_t OFFSET_IDX, int ... Indexes>
    static void unpack_tuple_impl(const byte_t* src, value_t& tuple, ct::ind_seq::index<Indexes...>)
    {
        values_unpacker<Types...>::template unpack_values<OFFSET_IDX>(src, /* unpack tuple items: */ std::get<Indexes>(tuple) ...);
    }

    template <std::size_t OFFSET_IDX>
    static void unpack(const byte_t* src, value_t& tuple)
    {
        constexpr std::size_t TUPLE_ITEMS_COUNT = sizeof...(TupleTypes);
        unpack_tuple_impl<OFFSET_IDX>(src, tuple, ct::ind_seq::gen_seq<TUPLE_ITEMS_COUNT>{});
    }
};

// -----------------------------------------------------------------------------
// Convenient functon with implicit automatic types deduction

template <typename ... Args,

          // Deduced types
          typename unpacker_t = values_unpacker<Args...>,
          typename byte_t = typename unpacker_t::byte_t>
inline void unpack_from(const byte_t* bytes, Args& ... args)
{
    unpacker_t::template unpack_values<0>(bytes, args...);
}

template <typename ... Args,

          // Deduced types
          typename unpacker_t = values_unpacker<Args...>,
          typename byte_buffer_t = typename unpacker_t::byte_buffer_t
          >
inline void unpack(const byte_buffer_t& buffer, Args& ... args)
{
    unpack_from(buffer.data(), args...);
}

// TODO: this is experimental. Possibly can be removed in future
template <typename ... Args,

          // Deduced types
          typename tuple_t = std::tuple<Args...>,
          typename unpacker_t = values_unpacker<Args...>,
          typename byte_t = typename unpacker_t::byte_t
          >
tuple_t unpack_as_tuple(const byte_t* bytes)
{
    tuple_t tuple;
    unpack_from(bytes, tuple);
    return tuple;
}

template <typename ... Args,

          // Deduced types
          typename tuple_t = std::tuple<Args...>,
          typename unpacker_t = values_unpacker<Args...>,
          typename byte_buffer_t = typename unpacker_t::byte_buffer_t
          >
tuple_t unpack_as_tuple(const byte_buffer_t& buffer)
{
    return unpack_as_tuple<Args...>(buffer.data());
}

// -----------------------------------------------------------------------------

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__UNPACK_HPP
