#ifndef CT__UTILS__NTH_TYPE_OF_HPP
#define CT__UTILS__NTH_TYPE_OF_HPP

#if defined(CT_ENABLE_TESTS)
    #include <type_traits> // for std::is_same<T, U>::value
#endif

namespace ct {

namespace utils {

/**
    via: https://stackoverflow.com/a/20163490/

    Renamed from original `tuple_element` name into `nth_type_of`, since it not
    about tuples, but about type-lists.

    Note: this is kinda-lighweight version of `std::tuple_element`, and may be
    implemented much simpler (https://stackoverflow.com/a/29753388/):
    @code{.cpp}
        template<int N, typename... Types>
        using NthTypeOf
            = typename std::tuple_element< N, std::tuple<Types...> >::type;
    @endcode
*/

template <int N, typename ... Types>
struct nth_type_of {};

// Specialization for 0 index - end recursion ang give first type
template <typename T0, typename... Types>
struct nth_type_of<0, T0, Types...> {
    using type = T0 ;
};

template <int N, typename T0, typename... Types>
struct nth_type_of<N, T0, Types...> {
    using type = typename nth_type_of<(N-1), Types...>::type;
};

#if defined(CT_ENABLE_TESTS)
namespace tests {

    static_assert(std::is_same< nth_type_of<0, bool, int, double>::type,   bool>::value, "Test failed");
    static_assert(std::is_same< nth_type_of<1, bool, int, double>::type,    int>::value, "Test failed");
    static_assert(std::is_same< nth_type_of<2, bool, int, double>::type, double>::value, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace utils

} // namespace ct

#endif // CT__UTILS__NTH_TYPE_OF_HPP
