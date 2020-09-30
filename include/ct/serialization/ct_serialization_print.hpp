#ifndef CT__SERIALIZATION__PRINT_HPP
#define CT__SERIALIZATION__PRINT_HPP

#include <iostream> // for std::cout, std::endl

#include <array>  // for std::array<T, SIZE>
#include <vector> // for std::vector<T>
#include <tuple>  // for std::tuple<Types>

#include <typeinfo> // for typeid(T).name()

#include "ct/utils/ct_utils_index_sequence.hpp"

namespace ct {

namespace serialization {

namespace impl {

// Utility function for making offset-string, from indentation level
inline std::string o(std::size_t offset) {
    return (offset == 0) ? "" : std::string((offset * 4), ' ');
}

template <typename T>
constexpr const char* type_name() {
    return typeid(T).name();
}

} // namespace impl

template <typename T, typename Enabled = void>
struct printer_trait;

// Specialization for printing single scalar value
template <typename T>
struct printer_trait< T, typename std::enable_if< std::is_scalar<T>::value == true>::type>
{
    using value_t = T;

    // Specialization for printing non-enums - as-is
    template <typename ValueType>
    static auto print_value_impl(const ValueType& value, const std::size_t indent)
        -> typename std::enable_if< std::is_enum<ValueType>::value == false, void>::type
    {
        std::cout << impl::o(indent) << "<T = " << impl::type_name<ValueType>() << "> { ";

        std::cout << value;

        std::cout << " }";
        std::cout << std::endl;
    }

    // Specialization for printing enums - as integral values
    template <typename ValueType>
    static auto print_value_impl(const ValueType& value, const std::size_t indent)
        -> typename std::enable_if< std::is_enum<ValueType>::value == true, void >::type
    {
        using enum_int_type = typename std::underlying_type<ValueType>::type;
        const enum_int_type enum_int = static_cast<enum_int_type>(value);
        // ------------------------------------------------------------------

        std::cout << impl::o(indent) << "<EnumT = " << impl::type_name<ValueType>() << ", IntType = " << impl::type_name<enum_int_type>() << "> { ";

        std::cout << enum_int;

        std::cout << " }";
        std::cout << std::endl;
    }

    // -------------------------------------------------------------------------

    static void print(const value_t& value, const std::size_t indent = 0)
    {
        print_value_impl<value_t>(value, indent);
    }
};

namespace impl {

template <typename T>
inline void print_scalar_array_items(const T* items, const std::size_t size)
{
    for(std::size_t i = 0; i < size; ++i) {
        std::cout << items[i] << ", ";
    }
}

template <typename T>
inline void print_non_scalar_array_items(const T* items, const std::size_t size, const std::size_t indent)
{
    for(std::size_t i = 0; i < size; ++i) {
        printer_trait<T>::print(items[i], (indent) );
    }
}

// -----------------------------------------------------------------------------

template <typename T, std::size_t SIZE, typename F>
inline void print__std_array__wrap(F&& callback, const std::size_t indent)
{
    std::cout << impl::o(indent) << "std::array<T = " << impl::type_name<T>() << ", SIZE = " << SIZE << "> {";
    std::cout << std::endl;

    callback();

    std::cout << impl::o(indent) << "}";
    std::cout << std::endl;
}

template <typename T, typename F>
inline void print__std_vector__wrap(F&& callback, const std::size_t size, const std::size_t indent)
{
    std::cout << impl::o(indent) << "std::vector<T = " << impl::type_name<T>() << ", size = " << size << "> {";
    std::cout << std::endl;

    callback();

    std::cout << impl::o(indent) << "}";
    std::cout << std::endl;
}

template <typename T, typename F>
inline void print__std_initializer_list__wrap(F&& callback, const std::size_t size, const std::size_t indent)
{
    std::cout << impl::o(indent) << "std::initializer_list<T = " << impl::type_name<T>() << ", size = " << size << "> {";
    std::cout << std::endl;

    callback();

    std::cout << impl::o(indent) << "}";
    std::cout << std::endl;
}

} // namespace impl

// -----------------------------------------------------------------------------


// Specialization for printing scalar std::array
template <typename T, std::size_t SIZE>
struct printer_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == true>::type>
{
    using value_t = std::array<T, SIZE>;

    static void print(const value_t& array, const std::size_t indent = 0)
    {
        impl::print__std_array__wrap<T, SIZE>([&]
        {
            std::cout << impl::o(indent + 1);
            impl::print_scalar_array_items(array.data(), SIZE);
            std::cout << std::endl;

        }, indent);
    }
};

// Specialization for printing non-scalar std::array
template <typename T, std::size_t SIZE>
struct printer_trait< std::array<T, SIZE>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::array<T, SIZE>;

    static void print(const value_t& array, const std::size_t indent = 0)
    {
        impl::print__std_array__wrap<T, SIZE>([&]
        {
            impl::print_non_scalar_array_items(array.data(), SIZE, (indent + 1));

        }, indent);
    }
};

// -----------------------------------------------------------------------------

// Specialization for printing scalar std::vector
template <typename T>
struct printer_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::vector<T>;

    static void print(const value_t& vector, const std::size_t indent = 0)
    {
        const std::size_t vec_size = vector.size();

        impl::print__std_vector__wrap<T>([&]
        {
            std::cout << impl::o(indent + 1);
            impl::print_scalar_array_items(vector.data(), vec_size);
            std::cout << std::endl;

        }, vec_size, indent);
    }
};

// Specialization for printing non-scalar std::vector
template <typename T>
struct printer_trait< std::vector<T>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::vector<T>;

    static void print(const value_t& vector, const std::size_t indent = 0)
    {
        const std::size_t vec_size = vector.size();

        impl::print__std_vector__wrap<T>([&]
        {
            impl::print_non_scalar_array_items(vector.data(), vec_size);

        }, vec_size, indent);
    }
};

// -----------------------------------------------------------------------------

// Specialization for printing scalar std::initializer_list
template <typename T>
struct printer_trait< std::initializer_list<T>, typename std::enable_if< std::is_scalar<T>::value == true>::type >
{
    using value_t = std::initializer_list<T>;

    static void print(const value_t& list, const std::size_t indent = 0)
    {
        const std::size_t list_size = list.size();

        impl::print__std_initializer_list__wrap<T>([&]
        {
            // Similar to impl::print_scalar_array_items(), except: value given
            // not by using operator[], but `*(list.begin() + i)`
            std::cout << impl::o(indent + 1);
            for(std::size_t i = 0; i < list_size; ++i) {
                std::cout << *(list.begin() + i) << ", ";
            }
            std::cout << std::endl;

        }, list_size, indent);
    }
};

// Specialization for printing non-scalar std::initializer_list
template <typename T>
struct printer_trait< std::initializer_list<T>, typename std::enable_if< std::is_scalar<T>::value == false>::type >
{
    using value_t = std::initializer_list<T>;

    static void print(const value_t& list, const std::size_t indent = 0)
    {
        const std::size_t list_size = list.size();

        impl::print__std_initializer_list__wrap<T>([&]
        {
            // Similar to impl::print_non_scalar_array_items(), except: value
            // given not by using operator[], but `*(list.begin() + i)
            for(std::size_t i = 0; i < list_size; ++i) {
                printer_trait<T>::print( *(list.begin() + i), (indent +1));
            }

        }, list_size, indent);
    }
};

// -----------------------------------------------------------------------------

// Specialization for printing std::pair
template <typename First, typename Second>
struct printer_trait< std::pair<First, Second> >
{
    using value_t = std::pair<First, Second>;

    static void print(const value_t& pair, const std::size_t indent = 0)
    {
        std::cout << impl::o(indent) << "std::pair<First = " << impl::type_name<First>() << ", Second = " << impl::type_name<Second>() << "> {";
        std::cout << std::endl;

        printer_trait<First >::print(pair.first,  indent + 1);
        printer_trait<Second>::print(pair.second, indent + 1);

        std::cout << impl::o(indent) << "}";
        std::cout << std::endl;
    }
};

// Specialization for printing std::tuple
template <typename ... Types>
struct printer_trait< std::tuple<Types...> >
{
    using value_t = std::tuple<Types...>;

    template <int ... Indexes>
    static void print_impl(const value_t& tuple, const std::size_t indent, ct::ind_seq::index<Indexes...>)
    {
        std::cout << impl::o(indent) << "std::tuple<Types... = ";

        // Print tuple types
        {
            using dummy_t = int[];
            (void) dummy_t {
                ( std::cout << impl::type_name<Types>()  << ", ", /* zero for making dummy_t: */ 0) ...
            };
        }

        std::cout << "> {" << std::endl;

        // Print values
        {
            using dummy_t = int[];
            (void) dummy_t {
                ( printer_trait<Types>::print( std::get<Indexes>(tuple), (indent + 1)), /* zero for making dummy_t: */ 0) ...
            };
        }

        std::cout << impl::o(indent) << "}";
        std::cout << std::endl;
    }

    static void print(const value_t& tuple, const std::size_t indent = 0)
    {
        print_impl(tuple, indent, ct::ind_seq::gen_seq<sizeof... (Types)>{});
    }
};

// -----------------------------------------------------------------------------

/**
    Convenient helper-function for printing multiple values, without packing
    them into extra unnecessary tuple-wrapper.

    Important note: you can implement priting for custom types, by implementing:
        `static void printer_trait< YOUR_TYPE >::print(const YOUR_TYPE& value, const std::size_t indent)`.
    For example, simple implementation for printing Qt's <QVector>:

    @code{.cpp}
    #include <QVector>

    namespace ct {
    namespace serialization {
    namespace utils {

    // Specialization for printing Qt::QVector
    template <typename T>
    struct printer_trait< QVector<T> >
    {
        using value_t = QVector<T>;

        static void print(const value_t& vector, const std::size_t indent = 0)
        {
            std::cout << impl::o(indent) << "QVector<T = " << impl::type_name<T>() << "> {";
            std::cout << std::endl;

            // Print values
            for(const T& item : vector) {
                printer_trait<T>::print(item, (indent+1));
            }

            std::cout << impl::o(indent) << "}";
            std::cout << std::endl;
        }
    };

    } // namespace utils
    } // namespace serialization
    } // namespace ct

    int main() {
        const QVector<int> vec1{1, 2, 3};
        const QVector< std::pair<int, int> > vec2{ {4,5}, {6,7} };

        ct::serialization::utils::print(vec1, vec2);
        return 0;
    }
    @endcode
*/

template <typename ... Types>
inline void print(const Types& ... values)
{
    using dummy_t = int[];
    (void) dummy_t {
        ( printer_trait<Types>::print(values), /* zero for making dummy_t: */ 0) ...
    };
}

} // namespace serialization

} // namespace ct

#endif // CT__SERIALIZATION__PRINT_HPP
