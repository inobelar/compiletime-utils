#ifndef CT__SERIALIZATION__PACK_HPP
#define CT__SERIALIZATION__PACK_HPP

#include "ct/serialization/utils/ct_serialization_utils.hpp"

#include "ct/utils/typelist/ct_utils_typelist_to_tuple.hpp"
#include "ct/utils/typelist/ct_utils_typelist_first_types.hpp"

//#include <iostream> // FIXME: for debug

#include <cstring> // for std::memcpy()

namespace ct {

namespace serialization {

/**
    Written as bunch of nested specialization traits, to make this extendable
    by custom types.

    # Extending by custom types

    namespace ct {
    namespace serialization {

    @code{.cpp}
    template<typename ... Types>
    template< GENERIC_TYPES_FOR YOUR_TYPE >
    struct packer_trait<Types...>::specialized_for< YOUR_TYPE , (OPTIONAL) RESTRICTION_EXPRESSION_FOR YOUR_TYPE >
    {
        using info_t = packer_trait<Types...>::info_t;
        using byte_t = typename info_t::byte_t;

        using value_t = YOUR_TYPE;

        template <std::size_t OFFSET_IDX>
        static void pack(byte_t* dest, const value_t& value)
        {
            // ... implementation ...
        }
    };

    } // namespace serialization
    } // namespace ct
    @endcode
*/

/**
    # Old code, stored here for inspirational description of that traits

    @code{.cpp}
    template <typename ... Types>
    struct packer
    {
        using byte_t = std::int8_t;
        using byte_buffer_t = std::array<byte_t, ct::get_bytes_count<Types...>()>;

        static constexpr auto get_offsets()
            -> decltype ( get_flat_offsets<Types ...>() )
        {
            return get_flat_offsets<Types ...>();
        }

        // ---------------------------------------------------------------------
        // Single scalar type

        template <std::size_t OFFSET_IDX, typename T>
        static auto pack(byte_t* dest, const T& value)
            -> typename std::enable_if< std::is_scalar<T>::value == true, void>::type
        {
            std::memcpy( (dest + std::get<OFFSET_IDX>(get_offsets())), &value, sizeof(T) );
        }

        // ---------------------------------------------------------------------
        // Arays: raw array T[SIZE] and std::array<T, SIZE>

        // std::array with scalar types
        template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE>
        static auto pack(byte_t* dest, const std::array<T, SIZE>& array)
            -> typename std::enable_if< std::is_scalar<T>::value == true, void>::type
        { ... }

        // raw array with scalar types
        template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE>
        static auto pack(byte_t* dest, const T(&array)[SIZE])
            -> typename std::enable_if< std::is_scalar<T>::value == true, void>::type
        { ... }

        // - - - -

        // std::array with non-scalar types
        template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE>
        static auto pack(byte_t* dest, const std::array<T, SIZE>& array)
            -> typename std::enable_if< std::is_scalar<T>::value == false, void>::type
        { ... }

        // raw array with no-scalar types
        template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE>
        static auto pack(byte_t* dest, const T(&array)[SIZE])
            -> typename std::enable_if< std::is_scalar<T>::value == false, void>::type
        { ... }

        // ---------------------------------------------------------------------
        // std::pair

        template <std::size_t OFFSET_IDX, typename First, typename Second>
        static void pack(byte_t* dest, const std::pair<First, Second>& pair)
        {
            pack<OFFSET_IDX +                             0>(dest, pair.first);
            pack<OFFSET_IDX + exp::get_values_count<First>()>(dest, pair.second);
        }

        // ---------------------------------------------------------------------
        // parameter pack

        template <std::size_t OFFSET_IDX, typename ... Args, int ... Indexes>
        static void pack_values_impl(byte_t* dest, const Args& ... values, ct::ind_seq::index<Indexes...>)
        {
            using dummy_t = int[];
            (void) dummy_t {
                (pack<OFFSET_IDX + ( Indexes == 0 ? 0 :
                    (exp::get_values_count<
                        typename ct::utils::list_to_tuple< typename ct::utils::first_types<Indexes, ct::utils::List<Args...>>::type >::type
                    >())
                )>(dest, values), 0) ... };
        }

        template <std::size_t OFFSET_IDX, typename ... Args>
        static void pack_values(byte_t* dest, const Args& ... values) {
            pack_values_impl<OFFSET_IDX, Args...>(dest, values..., ct::ind_seq::gen_seq< sizeof...(Args) >{});
        }

        // ---------------------------------------------------------------------
        // std::tuple

        template <std::size_t OFFSET_IDX, typename ... Args, int ... Indexes>
        static void pack_tuple_impl(byte_t* dest, const std::tuple<Args...>& tuple, ct::ind_seq::index<Indexes...>) {
            pack_values<OFFSET_IDX>(dest, std::get<Indexes>(tuple) ...);
        }

        template <std::size_t OFFSET_IDX, typename ... Args>
        static void pack(byte_t* dest, const std::tuple<Args...>& tuple)
        {
            pack_tuple_impl<OFFSET_IDX>(dest, tuple, ct::ind_seq::gen_seq< sizeof...(Args) >{});
        }
    };

    template <typename T>
    auto make_buffer(const T& value)
        -> typename ct::serialization::packer<T>::byte_buffer_t
    {
        using packer_t = ct::serialization::packer<T>;
        using byte_buffer_t = typename packer_t::byte_buffer_t;

        byte_buffer_t buffer;
        packer_t::template pack< 0 >(buffer.data(), value);

        return buffer;
    }
    @endcode

 */

template <typename ... Types>
struct packer_trait
{
    using info_t = ct::serialization::utils::types_sizeofs_info<Types...>;

    /*
        We need to store all `specialized_for<T>::pack()`-functions here, inside
        the template struct with parameter pack (paker_trait<Types...>), because
        we need to retreive compile-time offsets for all passed types.

        Unfortunately, due to usage of `std::memcpy()` (which, is not constexpr
        function in c++11), we cannot mark all that recursive `pack()` functions
        like `constexpr` too :C
    */
    template <typename T, typename Enabled = void>
    struct specialized_for {};
};

// Specialization for: Single scalar type
template<typename ... Types>
template<typename T>
struct packer_trait<Types...>::specialized_for<T, typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T;

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& value)
    {
        // std::cout << "pack <    T>: offset_idx= " << OFFSET_IDX << " value=" << value << std::endl;

        constexpr std::size_t OFFSET = std::get<OFFSET_IDX>( info_t::get_offsets() );
        std::memcpy( (dest + OFFSET), &value, sizeof(T) );
    }
};

namespace impl {

// Utility/common implementation for array types
template <typename ... Types>
struct same_items_packer
{
    using info_t = typename packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    template <std::size_t OFFSET_IDX, typename T, std::size_t COUNT>
    static auto pack_scalar_array(byte_t* dest, const T* items)
        -> typename std::enable_if< std::is_scalar<T>::value, void>::type
    {
        // std::cout << "pack <array>: offset_idx= " << OFFSET_IDX << " values= [";
        // for(std::size_t i = 0; i < COUNT; ++i) {
        //     std::cout << (*items) << ", ";
        // }
        // std::cout << ']' << std::endl;

        constexpr std::size_t OFFSET = std::get<OFFSET_IDX>( info_t::get_offsets() );
        std::memcpy( (dest + OFFSET), items, (COUNT * sizeof(T)) );
    }

    template <std::size_t OFFSET_IDX, typename T, std::size_t SIZE, int ... Indexes>
    static void pack_non_scalar_array(byte_t* dest, const T* item, ct::ind_seq::index<Indexes...>)
    {
        using dummy_t = int[];
        (void) dummy_t {
            ( packer_trait<Types...>::template specialized_for<T>::template pack<OFFSET_IDX + (Indexes * utils::get_memcpy_values_count<T>()) >(dest, item[Indexes]), /* for making dummy_t: */ 0) ...
        };
    }
};

} // namespace impl

// Specialization for: std::array with scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct packer_trait<Types...>::specialized_for< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::array<T, SIZE>;

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& array)
    {
        impl::same_items_packer<Types...>::template pack_scalar_array<OFFSET_IDX, T, SIZE>(dest, array.data());
    }
};

// Specialization for: raw array with scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct packer_trait<Types...>::specialized_for< T[SIZE], typename std::enable_if< std::is_scalar<T>::value == true >::type>
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T[SIZE];

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& array)
    {
        impl::same_items_packer<Types...>::template pack_scalar_array<OFFSET_IDX, T, SIZE>(dest, &array, ct::ind_seq::gen_seq<SIZE>{});
    }
};

// Specialization for: std::array with non-scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct packer_trait<Types...>::specialized_for< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == false >::type>
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::array<T, SIZE>;

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& array)
    {
        impl::same_items_packer<Types...>::template pack_non_scalar_array<OFFSET_IDX, T, SIZE>(dest, array.data(), ct::ind_seq::gen_seq<SIZE>{});
    }
};

// Specialization for: raw array with no-scalar types
template<typename ... Types>
template<typename T, std::size_t SIZE>
struct packer_trait<Types...>::specialized_for< T[SIZE], typename std::enable_if< std::is_scalar<T>::value == false >::type>
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = T[SIZE];

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& array)
    {
        impl::same_items_packer<Types...>::template pack_non_scalar_array<OFFSET_IDX, T, SIZE>(dest, &array, ct::ind_seq::gen_seq<SIZE>{});
    }
};

// -----------------------------------------------------------------------------

// Specialization for: std::pair
template<typename ... Types>
template<typename First, typename Second>
struct packer_trait<Types...>::specialized_for< std::pair<First, Second> >
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::pair<First, Second>;

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& pair)
    {
        // std::cout << "<T,T> pair: left:  offset_idx= " << OFFSET_IDX << " + " << 0 << std::endl;
        packer_trait<Types...>::template specialized_for<First >::template pack<OFFSET_IDX +                              0>(dest, pair.first);

        // std::cout << "<T,T> pair: right: offset_idx= " << OFFSET_IDX  << " + " << utils::get_memcpy_values_count<First>() << std::endl;
        packer_trait<Types...>::template specialized_for<Second>::template pack<OFFSET_IDX + utils::get_memcpy_values_count<First>()>(dest, pair.second);
    }
};

// -----------------------------------------------------------------------------

// Convenient & most smart trait for pack (still recursive & nested) template
// parameter pack
template <typename ... Types>
struct values_packer
{
    using info_t = typename packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    template <std::size_t OFFSET_IDX, typename ... Args, int ... Indexes>
    static void pack_values_impl(byte_t* dest, const Args& ... values, ct::ind_seq::index<Indexes...>)
    {
        using dummy_t = int[];
        (void) dummy_t {
            ( packer_trait<Types...>::template specialized_for<Args>::template pack
              <OFFSET_IDX + ( Indexes == 0 ? 0 :
               /*
                   Graphical description of per-item offsets calculation logic:

                   ------------------------------------------------------------
                   Input:
                       < i32, <i64, i16>, <i8, <i32, i16>> > <-- Nested types = <Args...>
                       [   4,   (8+2=10),  (1  + (4+2=8))=9] <-- sizeofs of each items (calculated recursviely), but unused
                       [   0,          1,                2 ] <-- Types indexes = <index<Indexes...>>

                   ------------------------------------------------------------
                   Flattening:

                       < i32,  i64, i16,   i8,  i32, i16   > <-- Flattened types
                       [   4,    8,   2,    1,    4,   2   ] <-- Flattened sizeofs
                       [   0,    1,   2,    3,    4,   5   ] <-- Flattened indexes
                       [   0,    4,  12,   14,   13,  17   ] <-- Flattened Offsets (calculated on Flattened sizeofs)

                   ------------------------------------------------------------
                   Getting offsets index:

                       < i32, <i64, i16>, <i8, <i32, i16>> > <-- Nested types = <Args...>
                                          ^^^^^^^^^^^^^^^^
                                                 |
                                                 +-- std::size_t ItemIdx = (Indexes==2);
                                                     Arg ItemValue = values[ItemIdx];


                       < i32, <i64, i16> > <-- types before ItemIdx (first_types<ItemIdx==2>)
                       ^^^^^^^^^^^^^^^^^^^
                                |
                                +-- items count = 3

                   ------------------------------------------------------------

                   So, for ItemIdx==2 --> Offset = Flattened_Offsets[3] = 12

                   Yes... I know how strange it is look :D
               */
               (utils::get_memcpy_values_count<
                    typename ct::utils::list_to_tuple< typename ct::utils::first_types<Indexes, ct::utils::List<Args...>>::type >::type
               >())
            )>(dest, /* Item= */ values), /* for making dummy_t: */ 0) ...
        };
    }

    template <std::size_t OFFSET_IDX, typename ... Args>
    static void pack_values(byte_t* dest, const Args& ... values)
    {
        constexpr std::size_t ARGS_COUNT = sizeof...(Args);
        pack_values_impl<OFFSET_IDX, Args...>(dest, values..., ct::ind_seq::gen_seq<ARGS_COUNT>{});
    }

};

// Specialization for: std::tuple
template<typename ... Types>
template<typename ... TupleTypes>
struct packer_trait<Types...>::specialized_for< std::tuple<TupleTypes...> >
{
    using info_t = packer_trait<Types...>::info_t;
    using byte_t = typename info_t::byte_t;

    using value_t = std::tuple<TupleTypes...>;

    template <std::size_t OFFSET_IDX, int ... Indexes>
    static void pack_tuple_impl(byte_t* dest, const value_t& tuple, ct::ind_seq::index<Indexes...>)
    {
        values_packer<Types...>::template pack_values<OFFSET_IDX>(dest, /* unpack tuple items: */ std::get<Indexes>(tuple) ...);
    }

    template <std::size_t OFFSET_IDX>
    static void pack(byte_t* dest, const value_t& tuple)
    {
        constexpr std::size_t TUPLE_ITEMS_COUNT = sizeof...(TupleTypes);
        pack_tuple_impl<OFFSET_IDX>(dest, tuple, ct::ind_seq::gen_seq<TUPLE_ITEMS_COUNT>{});
    }
};

// -----------------------------------------------------------------------------
// Convenient functon with implicit automatic types deduction

template <typename ... Args>
inline void pack_into(typename values_packer<Args...>::byte_t* bytes, const Args& ... args)
{
    values_packer<Args...>::template pack_values<0>(bytes, args...);
}

template <typename ... Args>
inline auto pack(const Args& ... args)
    -> typename values_packer<Args...>::info_t::byte_buffer_t
{
    using byte_buffer_t = typename values_packer<Args...>::info_t::byte_buffer_t; // Shortcut

    byte_buffer_t bytes;
    pack_into(bytes.data(), args...);
    return bytes;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Convenient shortcuts

template <typename ... Args>
using byte_buffer_t = typename values_packer<Args...>::info_t::byte_buffer_t;

template <typename ... Args>
constexpr std::size_t packed_bytes_count() {
    return values_packer<Args...>::info_t::bytes_count;
}

// -----------------------------------------------------------------------------

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__PACK_HPP
