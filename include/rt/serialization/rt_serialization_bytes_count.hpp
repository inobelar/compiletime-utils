#ifndef RT__SERIALIZATION__BYTES_COUNT_HPP
#define RT__SERIALIZATION__BYTES_COUNT_HPP

#include <type_traits> // for std::enable_if<T>::type, std::is_scalar<T>::value

namespace rt {

namespace serialization {

/**
    @attention The next trait (`bytes_count_trait`) must return \b packed \b
    bytes count. For example, for `std::vector<int>` it returns not:
        `vector.size() * sizeof(int)`
    but:
        `sizeof(collection_size_t) + ( vector.size() * sizeof(int) )`

    Be careful - this trait used for calculating necessary bytes buffer size,
    with all needed EXTRA space for meta-data (like collection sizes). This is
    not simply "sum of types-sizeofs".

    ----------------------------------------------------------------------------

    Separated from packing/unpacking traits, because:
        - to not duplicate same code in both 2 traits
        - it is actually not related for memory copying, only memory size calc.
*/

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
        std::size_t count = 0;

        using dummy_t = std::size_t[];
        (void) dummy_t {
            count += bytes_count_trait<Types>::bytes_count(values) ...
        };

        return count;
    }
};

// -----------------------------------------------------------------------------
// Convenient function with implicit types deduction

template <typename ... Args>
inline std::size_t bytes_count(const Args& ... args) {
    return byte_count_params<Args...>::bytes_count(args...);
}

// -----------------------------------------------------------------------------

} // namespace serialization

} // namespace rt

#endif // RT__SERIALIZATION__BYTES_COUNT_HPP
