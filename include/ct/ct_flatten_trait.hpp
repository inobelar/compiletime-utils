#ifndef CT__FLATTEN_TRAIT_HPP
#define CT__FLATTEN_TRAIT_HPP

#include "ct/utils/typelist/ct_utils_typelist_flatten.hpp"
#include "ct/utils/typelist/ct_utils_typelist_repeat.hpp"

#include <array>

namespace ct {

// Specialization for flatten'ing parameter pack: <Types...> -> List<Types...>
template <typename ... Types>
struct flatten_trait
        : ct::utils::flatten< ct::utils::List<Types ...> >
{};

// -----------------------------------------------------------------------------

// Specialization for flatten'ing: std::pair<First, Second> --> List<First, Second>
template <typename First, typename Second>
struct flatten_trait< std::pair<First, Second> >
        : flatten_trait< typename flatten_trait<First>::type, typename flatten_trait<Second>::type>
{};

// Specialization for flatten'ing: std::tuple<Types...> --> List<Types...>
template <typename ... Types>
struct flatten_trait< std::tuple<Types...> >
        : flatten_trait< typename flatten_trait<Types>::type ...>
{};

// -----------------------------------------------------------------------------

// Specialization for flatten'ing: std::array<T, SIZE> --> List<T ... SIZE>
template <typename T, std::size_t SIZE>
struct flatten_trait< std::array<T, SIZE> > :
        std::conditional<
            (std::is_scalar<T>::value == true), // Note: here is used std::is_scalar, instead of std::is_pod, because otherwise nested arrays-in-arrays not unpacked into flat form of arrays

            ct::utils::flatten< std::array<T,SIZE> >, // Not flatten_trait here (but ct::utils::flatten) to not make circular dependency

            flatten_trait< typename ct::utils::repeat< typename flatten_trait<T>::type, SIZE>::type >

        >::type
{};

// Specialization for flatten'ing raw array: T[SIZE] --> List<T ... SIZE>
template <typename T, std::size_t SIZE>
struct flatten_trait< T[SIZE] > :
        std::conditional<
            (std::is_scalar<T>::value == true), // Note: here is used std::is_scalar, instead of std::is_pod, because otherwise nested arrays-in-arrays not unpacked into flat form of arrays

            ct::utils::flatten< T[SIZE] >, // Not flatten_trait here (but ct::utils::flatten) to not make circular dependency

            flatten_trait< typename ct::utils::repeat< typename flatten_trait<T>::type, SIZE>::type >

        >::type
{};

// -----------------------------------------------------------------------------

namespace tests {

    // Pod types flatten'ing
    static_assert( std::is_same< flatten_trait<bool, int, float>::type, ct::utils::List<bool, int, float> >::value, "Test failed");

    // std::arrays flatten'ing
    static_assert( std::is_same< flatten_trait< std::array<int, 3> >::type, ct::utils::List< std::array<int, 3> > >::value, "Test failed");  // Arrays of POD-types non-unpacked
    static_assert( std::is_same< flatten_trait< std::array< std::pair<int,int>, 3> >::type, ct::utils::List<int,int,int,int,int,int> >::value, "Test failed"); // Arrays of Non-POD types unpacked
    static_assert( std::is_same< flatten_trait< std::array< std::array<int, 3>, 2> >::type, ct::utils::List<std::array<int,3>, std::array<int,3>> >::value, "Test failed"); // Nested arrays unpacked
    static_assert( std::is_same< flatten_trait< std::tuple< std::array<int, 2>, std::array<int, 3> > >::type, ct::utils::List<std::array<int,2>, std::array<int,3>> >::value, "Test failed"); // Nested arrays unpacked
    static_assert( std::is_same< flatten_trait< std::tuple< std::array<int, 1>, std::array<int, 2>, std::array<int, 3> > >::type, ct::utils::List< std::array<int, 1>, std::array<int, 2>, std::array<int, 3> > >::value, "asd"); // Arrays of POD-types non-unpacked

    // std::pair and std::tuple flatten'ing
    static_assert( std::is_same< flatten_trait< std::pair<int, float> >::type, ct::utils::List<int, float> >::value, "Test failed");
    static_assert( std::is_same< flatten_trait< std::tuple<bool, int, float> >::type, ct::utils::List<bool, int, float> >::value, "Test failed");
    static_assert( std::is_same< flatten_trait< std::pair<std::pair<bool, int>, float> >::type, ct::utils::List<bool, int, float> >::value, "Test failed");
    static_assert( std::is_same< flatten_trait< std::tuple<std::tuple<bool, int>, float> >::type, ct::utils::List<bool, int, float> >::value, "Test failed");

    // Some extreme nesting test
    static_assert ( std::is_same< flatten_trait< std::tuple< std::pair<bool, int>, float, std::array<int,4>> >::type, ct::utils::List<bool, int, float, std::array<int,4>> >::value, "asd");

} // namespace tests

} // namespace ct

#endif // CT__FLATTEN_TRAIT_HPP
