#ifndef RT__SERIALIZATION__PACK__STL_HPP
#define RT__SERIALIZATION__PACK__STL_HPP

#include "rt/serialization/rt_serialization_pack.hpp"

#include "ct/utils/ct_utils_index_sequence.hpp"

#include "rt/serialization/rt_serialization_stl_collection_size.hpp"

#include <array>
#include <vector>
#include <tuple>

#include <deque>
#include <forward_list>
#include <list>

namespace rt {
    
namespace serialization {

// -----------------------------------------------------------------------------

// Specialization for std::array (with scalar items)
template <typename T, std::size_t SIZE>
struct pack_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::array<T, SIZE>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& array)
    {
        // Pack size (well, this is not needed, but for better strictness during unpacking)
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, SIZE);

        const std::size_t DATA_BYTES_COUNT = (sizeof(T) * SIZE);
        std::memcpy( (dest + offset), array.data(), DATA_BYTES_COUNT);

        return offset += DATA_BYTES_COUNT;
    }
};

// Specialization for std::array (with non-scalar items)
template <typename T, std::size_t SIZE>
struct pack_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::array<T, SIZE>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& array)
    {
        // Pack size (well, this is not needed, but for better strictness during unpacking)
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, array.size());

        for(const T& item : array) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::vector (with scalar items)
template <typename T>
struct pack_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::vector<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& vec)
    {
        // Pack size
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, vec.size());

        const std::size_t DATA_BYTES_COUNT = (sizeof(T) * vec.size());
        std::memcpy( (dest + offset), vec.data(), DATA_BYTES_COUNT);

        return offset += DATA_BYTES_COUNT;
    }
};

// Specialization for std::vector (with non-scalar items)
template <typename T>
struct pack_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::vector<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& vec)
    {
        // Pack size
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, vec.size());

        for(const T& item : vec) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::pair
template <typename First, typename Second>
struct pack_trait< std::pair<First, Second> >
{
    using value_t = std::pair<First, Second>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& pair)
    {
        offset = pack_trait<First >::pack(dest, offset, pair.first);
        offset = pack_trait<Second>::pack(dest, offset, pair.second);
        return offset;
    }
};


// Specialization for std::tuple
template <typename ... Types>
struct pack_trait< std::tuple<Types...> >
{
    using value_t = std::tuple<Types...>;

    template <int ... Indexes>
    static std::size_t pack_impl(std::int8_t* dest, std::size_t offset, const value_t& tuple, ct::ind_seq::index<Indexes...>)
    {
        return param_packer<Types...>::pack(dest, offset, std::get<Indexes>(tuple)...); // Unpack tuple items
    }

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& tuple)
    {
        return pack_impl(dest, offset, tuple, ct::ind_seq::gen_seq<sizeof...(Types)>{});
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::deque
template <typename T>
struct pack_trait< std::deque<T> >
{
    using value_t = std::deque<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& deque)
    {
        // Pack size
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, deque.size());

        for(const T& item : deque) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};

// Specialization for std::forward_list
template <typename T>
struct pack_trait< std::forward_list<T> >
{
    using value_t = std::forward_list<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& list)
    {
        // Note:: std::forward_list has no `.size()` method, that's why was used
        // `std::distance()` here.
        //   References:
        //     - https://stackoverflow.com/questions/31822494/
        //     - https://stackoverflow.com/questions/16279936/
        //     - https://stackoverflow.com/a/27881899/

        // Pack size
        const std::size_t list_size = std::distance(list.begin(), list.end());
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, list_size);

        for(const T& item : list) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};

// Specialization for std::list
template <typename T>
struct pack_trait< std::list<T> >
{
    using value_t = std::list<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& list)
    {
        // Pack size
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, list.size());

        for(const T& item : list) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::initializer_list
template <typename T>
struct pack_trait< std::initializer_list<T> >
{
    using value_t = std::initializer_list<T>;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& list)
    {
        offset = pack_trait<stl::collection_size_t>::pack(dest, offset, list.size());

        for(const T& item : list) {
            offset = pack_trait<T>::pack(dest, offset, item);
        }

        return offset;
    }
};
    
} // namespace serialization
    
} // namespace rt

#endif // RT__SERIALIZATION__PACK__STL_HPP
