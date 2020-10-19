#ifndef RT__SERIALIZATION__UNPACK__STL_HPP
#define RT__SERIALIZATION__UNPACK__STL_HPP

#include "rt/serialization/rt_serialization_unpack.hpp"

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
struct unpack_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::array<T, SIZE>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& array)
    {
        stl::collection_size_t size = 0; // TODO
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, size);

        constexpr std::size_t DATA_BYTES_COUNT = (sizeof(T) * SIZE);
        std::memcpy(array.data(), (src + offset), DATA_BYTES_COUNT);

        return offset += DATA_BYTES_COUNT;
    }
};

// Specialization for std::array (with non-scalar items)
template <typename T, std::size_t SIZE>
struct unpack_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::array<T, SIZE>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& array)
    {
        stl::collection_size_t size = 0; // TODO
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, size);

        for(T& item : array) {
            offset = unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::vector (with scalar items)
template <typename T>
struct unpack_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::vector<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& vec)
    {
        // Unpack size
        stl::collection_size_t vec_size = 0;
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, vec_size);

        // Resize vector for by a retreived size
        vec.resize(vec_size);

        // Copy data into vector
        const std::size_t DATA_BYTES_COUNT = (sizeof(T) * vec_size);
        std::memcpy(vec.data(), (src + offset), DATA_BYTES_COUNT);

        return offset += DATA_BYTES_COUNT;
    }
};

// Specialization for std::vector (with non-scalar items)
template <typename T>
struct unpack_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::vector<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& vec)
    {
        // Unpack size
        stl::collection_size_t vec_size = 0;
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, vec_size);

        // Resize vector by a retreived size
        vec.resize(vec_size);

        for(T& item : vec) {
            offset = unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::pair
template <typename First, typename Second>
struct unpack_trait< std::pair<First, Second> >
{
    using value_t = std::pair<First, Second>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& pair)
    {
        offset = unpack_trait<First >::unpack(src, offset, pair.first);
        offset = unpack_trait<Second>::unpack(src, offset, pair.second);
        return offset;
    }
};


// Specialization for std::tuple
template <typename ... Types>
struct unpack_trait< std::tuple<Types...> >
{
    using value_t = std::tuple<Types...>;

    template <int ... Indexes>
    static std::size_t unpack_impl(const std::int8_t* src, std::size_t offset, value_t& tuple, ct::ind_seq::index<Indexes...>)
    {
        return param_unpacker<Types...>::unpack(src, offset, std::get<Indexes>(tuple)...); // Unpack tuple items
    }

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& tuple)
    {
        return unpack_impl(src, offset, tuple, ct::ind_seq::gen_seq<sizeof...(Types)>{});
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::deque
template <typename T>
struct unpack_trait< std::deque<T> >
{
    using value_t = std::deque<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& deque)
    {
        // Unpack size
        stl::collection_size_t deque_size = 0;
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, deque_size);

        // Resize deque by a retreived size
        deque.resize(deque_size);

        for(T& item : deque) {
            offset = unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};

// Specialization for std::forward_list
template <typename T>
struct unpack_trait< std::forward_list<T> >
{
    using value_t = std::forward_list<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& list)
    {
        // Unpack size
        stl::collection_size_t list_size = 0;
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, list_size);

        // Resize vector a retreived size
        list.resize(list_size);

        for(T& item : list) {
            offset = unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};

// Specialization for std::list
template <typename T>
struct unpack_trait< std::list<T> >
{
    using value_t = std::list<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& list)
    {
        // Unpack size
        stl::collection_size_t list_size = 0;
        offset = unpack_trait<stl::collection_size_t>::unpack(src, offset, list_size);

        // Resize vector a retreived size
        list.resize(list_size);

        for(T& item : list) {
            offset = unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::initializer_list
/*
TODO: this is not a good idea, since std::initializer_list not constructible in
such way

template <typename T>
struct unpack_trait< std::initializer_list<T> >
{
    using value_t = std::initializer_list<T>;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& list)
    {
        // Unpack size (TODO: unused)
        stl::collection_size_t list_size = 0;
        offset += unpack_trait<stl::collection_size_t>::unpack(src, offset, list_size);

        for(const T& item : list) {
            offset += unpack_trait<T>::unpack(src, offset, item);
        }

        return offset;
    }
};
*/

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__UNPACK__STL_HPP
