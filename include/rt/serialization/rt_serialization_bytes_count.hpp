#ifndef RT__SERIALIZATION__BYTES_COUNT_HPP
#define RT__SERIALIZATION__BYTES_COUNT_HPP

#include <type_traits> // for std::enable_if<T>::type, std::is_stirivally_copyable<T>::value

namespace rt {

namespace serialization {

template <typename T, typename Enabled = void>
struct bytes_count_trait {};

// Note - std::is_scalar<T> here, instead of std::is_trivially_copyable<T>, to
// not make this specialization also for std::array<T,SIZE>
template <typename T>
struct bytes_count_trait<T, typename std::enable_if< std::is_scalar<T>::value == true >::type >
{
    using value_t = T;

    static constexpr std::size_t bytes_count(const value_t& ) {
        return sizeof(T);
    }
};

template <typename ... Types>
struct byte_count_params
{
    static std::size_t bytes_count(const Types& ... values)
    {
        constexpr std::size_t VALUES_COUNT = sizeof...(Types);
        using array_t = std::size_t[VALUES_COUNT];

        // Note: used `count += bytes_count()` for getting total bytes count
        // simply from `sizeofs[VALUES_COUNT-1]`. It is faster, then std::accumulate(sizeofs)

        std::size_t count = 0;
        const array_t sizeofs = {
            count += bytes_count_trait<Types>::bytes_count(values) ...
        };
        return sizeofs[VALUES_COUNT-1]; // last item - total count
    }
};

template <typename ... Args>
inline std::size_t bytes_count(const Args& ... args) {
    return byte_count_params<Args...>::bytes_count(args...);
}

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__BYTES_COUNT_HPP
