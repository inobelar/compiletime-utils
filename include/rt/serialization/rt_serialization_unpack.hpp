#ifndef RT__SERIALIZATION__UNPACK_HPP
#define RT__SERIALIZATION__UNPACK_HPP

#include <type_traits> // for std::enable_if<T>::type, std::is_trivially_copyable<T>::value

#include <cstdint> // for std::int8_t
#include <cstring> // for std::memcpy()

namespace rt {

namespace serialization {

template <typename T, typename Enabled = void>
struct unpack_trait {};

// Note - std::is_scalar<T> here, instead of std::is_trivially_copyable<T>, to
// not make this specialization also for std::array<T,SIZE>
template <typename T>
struct unpack_trait<T, typename std::enable_if< std::is_scalar<T>::value == true >::type >
{
    using value_t = T;

    static std::size_t unpack(const std::int8_t* src, std::size_t offset, value_t& value)
    {
        constexpr std::size_t BYTES_COUNT = sizeof(value_t);
        std::memcpy(&value, (src + offset), BYTES_COUNT);
        return offset + BYTES_COUNT;
    }
};

template <typename ... Types>
struct param_unpacker
{
    static std::size_t unpack(const std::int8_t* src, std::size_t offset, Types& ... values)
    {
        constexpr std::size_t VALUES_COUNT = sizeof...(Types);
        using offsets_array_t = std::size_t[VALUES_COUNT];

        const offsets_array_t offsets {
            offset += unpack_trait<Types>::unpack(src, offset, values) ...
        };

        return offsets[VALUES_COUNT-1]; // last offset
    }
};

// -----------------------------------------------------------------------------
// Convenient function with implicit types deduction

template <typename ... Args>
inline std::size_t unpack(const std::int8_t* bytes, Args& ... args)
{
    return param_unpacker<Args...>::unpack(bytes, 0, args...);
}

// -----------------------------------------------------------------------------

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__UNPACK_HPP
