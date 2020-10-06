#ifndef RT__SERIALIZATION__PACK_HPP
#define RT__SERIALIZATION__PACK_HPP

#include <type_traits> // for std::enable_if<T>::type, std::is_trivially_copyable<T>::value

#include <cstdint> // for std::int8_t
#include <cstring> // for std::memcpy()

namespace rt {

namespace serialization {

template <typename T, typename Enabled = void>
struct pack_trait {};

// Note - std::is_scalar<T> here, instead of std::is_trivially_copyable<T>, to
// not make this specialization also for std::array<T,SIZE>
template <typename T>
struct pack_trait<T, typename std::enable_if< std::is_scalar<T>::value == true >::type >
{
    using value_t = T;

    static std::size_t pack(std::int8_t* dest, std::size_t offset, const value_t& value)
    {
        constexpr std::size_t BYTES_COUNT = sizeof(value_t);
        std::memcpy( (dest + offset), &value, BYTES_COUNT );
        return offset + BYTES_COUNT;
    }
};

template <typename ... Types>
struct param_packer
{
    static std::size_t pack(std::int8_t* dest, std::size_t offset, const Types& ... values)
    {
        using dummy_t = std::size_t[];
        (void) dummy_t {
            offset += pack_trait<Types>::pack(dest, offset, values) ...
        };

        return offset;
    }
};

// -----------------------------------------------------------------------------
// Convenient function with implicit types deduction

template <typename ... Args>
inline std::size_t pack(std::int8_t* bytes, const Args& ... args)
{
    return param_packer<Args...>::pack(bytes, 0, args...);
}

// -----------------------------------------------------------------------------

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__PACK_HPP
