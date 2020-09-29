# WIP: Compile-Time utilities library

<p align="center">
  <a href="http://hits.dwyl.com/inobelar/compiletime-utils">
    <img title="HitCount" src="http://hits.dwyl.com/inobelar/compiletime-utils.svg"/>
  </a>
</p>

Written with type-traits (structures, with specialization for types) with **extentibility by custom types** in mind.

Written for **c++11**, without such useful things like:
- `fold expression` - [since c++17](https://en.cppreference.com/w/cpp/language/fold)
- `variable declaration` & `if/switch/for` in `constexpr` functions
- `std::index_sequence` - [since c++14](https://en.cppreference.com/w/cpp/utility/integer_sequence)

Add `CT_ENABLE_TESTS` for enabling inplace compile-time tests during hacking. 

## Serialization

Example of usage: [**in tests**](./tests/ct_serialization_tests/ct_serialization_test.cpp)

Based on next features
- `sizeof(T)`
- `<type_traits>`
- `SFINAE`
- `parameter pack` - [link](https://en.cppreference.com/w/cpp/language/parameter_pack)

### Debug printing example

```cpp
#include <ct/serialization/ct_serialization_pack.hpp>
#include <ct/serialization/ct_serialization_unpack.hpp>
#include <ct/serialization/ct_serialization_print.hpp>
#include <iostream>

template <typename ... Types>
void print_bytes(const Types& ... values)
{
    using packer_t = ct::serialization::packer_trait<Types...>;
    using bytes_buffer_t = typename packer_t::info_t::byte_buffer_t;

    std::cout << "----------------------------------------" << std::endl;

    // Print debug values representation
    ct::serialization::print(values...);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    // Print sizeofs
    {
        constexpr auto sizeofs = packer_t::info_t::get_sizeofs();

        std::cout << "sizeofs (" << sizeofs.size() << "): ";
        for(std::size_t i = 0; i < sizeofs.size(); ++i)
            std::cout << '[' << i << "]=" << sizeofs[i] << ", ";
        std::cout << std::endl;
    }

    // Print offsets
    {
        constexpr auto offsets = packer_t::info_t::get_offsets();

        std::cout << "offsets (" << offsets.size() << "): ";
        for(std::size_t i = 0; i < offsets.size(); ++i)
            std::cout << '[' << i << "]=" << offsets[i] << ", ";
        std::cout << std::endl;
    }

    // Make byte array and pack values into it
    bytes_buffer_t buff;
    ct::serialization::pack_into(buff.data(), values...);

    // Print bytes
    {
        std::cout << "bytes (" << buff.size() << "): ";
        for(const auto& b : buff)
            std::cout << (std::size_t)(b) << ", ";
        std::cout << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
}
```
