#ifndef RT__SERIALIZATION__BYTES_COUNT__STD_HPP
#define RT__SERIALIZATION__BYTES_COUNT__STD_HPP

#include "rt/serialization/rt_serialization_bytes_count.hpp"

#include "ct/utils/ct_utils_index_sequence.hpp"

// ---------------------------------------------------------
// Sequence containers

#include <array>
#include <vector>

#include <deque>
#include <forward_list>
#include <list>

// ---------------------------------------------------------

// TODO: add associative containers

#include <tuple>

namespace rt {

namespace serialization {

// -----------------------------------------------------------------------------

// Specialization for std::array
template <typename T, std::size_t SIZE>
struct bytes_count_trait< std::array<T, SIZE> >
{
    using value_t = std::array<T, SIZE>;

    static std::size_t bytes_count(const value_t& array)
    {
        std::size_t count = 0;
        for(const T& item : array) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

// Specialization for std::vector
template <typename T>
struct bytes_count_trait< std::vector<T> >
{
    using value_t = std::vector<T>;

    static std::size_t bytes_count(const value_t& vector)
    {
        std::size_t count = 0;
        for(const T& item : vector) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

// Specialization for std::deque
template <typename T>
struct bytes_count_trait< std::deque<T> >
{
    using value_t = std::deque<T>;

    static std::size_t bytes_count(const value_t& deque)
    {
        std::size_t count = 0;
        for(const T& item : deque) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

// Specialization for std::forward_list
template <typename T>
struct bytes_count_trait< std::forward_list<T> >
{
    using value_t = std::forward_list<T>;

    static std::size_t bytes_count(const value_t& list)
    {
        std::size_t count = 0;
        for(const T& item : list) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

// Specialization for std::list
template <typename T>
struct bytes_count_trait< std::list<T> >
{
    using value_t = std::list<T>;

    static std::size_t bytes_count(const value_t& list)
    {
        std::size_t count = 0;
        for(const T& item : list) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::pair
template <typename First, typename Second>
struct bytes_count_trait< std::pair<First, Second> >
{
    using value_t = std::pair<First, Second>;

    static std::size_t bytes_count(const value_t& pair) {
        return bytes_count_trait<First >::bytes_count(pair.first) +
               bytes_count_trait<Second>::bytes_count(pair.second);
    }
};

// Specialization for std::tuple
template <typename ... Types>
struct bytes_count_trait< std::tuple<Types...> >
{
    using value_t = std::tuple<Types...>;

    template <int ... Indexes>
    static std::size_t bytes_count_impl(const value_t& tuple, ct::ind_seq::index<Indexes...>)
    {
        return byte_count_params<Types...>::bytes_count( std::get<Indexes>(tuple) ... );
    }

    static std::size_t bytes_count(const value_t& tuple)
    {
        return bytes_count_impl(tuple, ct::ind_seq::gen_seq<sizeof... (Types)>{});
    }
};

// -----------------------------------------------------------------------------

// Specialization for std::initializer_list
template <typename T>
struct bytes_count_trait< std::initializer_list<T> >
{
    using value_t = std::initializer_list<T>;

    static std::size_t bytes_count(const value_t& list)
    {
        std::size_t count = 0;
        for(const T& item : list) {
            count += bytes_count_trait<T>::bytes_count(item);
        }
        return count;
    }
};

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__BYTES_COUNT__STD_HPP
