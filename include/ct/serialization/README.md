# CT :: Serialization

Examples of usage: [**in tests**](../../../tests/serialization_tests/ct_serialization_test.cpp)

The main goal of `ct::serialization` sub-section to provide convenient and safe zero-cost `std::memcpy()`-generator functions (`pack()` and `unpack()`), which do strictly the same as carefully-written-by-hand code (explicit boilerplait) - without any extra runtime overhead (even without simple additions for offsets calculation - everything guarantely computed in compile-time). The next goal - to provide convenient (recursive) solution for packing complex nested compsite-types (for which, writing such optimized code manually may be complete nightmare - like set of tuples, which contains arrays of pairs).

Implementation uses only known at compile-time information, like types (and `sizeof` for each of them), without touching values. That's why here is supported specialization for `std::array<T, SIZE>` but not `std::vector<T>`, size of which known only in run-time.
Based on next features:
- `sizeof(T)`      - currently single (in c++11) way of static introspection/reflection
- `<type_traits>`  - [link](https://en.cppreference.com/w/cpp/header/type_traits)
- `SFINAE`         - convenient way of writing extendable traits [link](https://en.cppreference.com/w/cpp/language/sfinae)
- `parameter pack` - [link](https://en.cppreference.com/w/cpp/language/parameter_pack)

Currently `packing`/`unpacking` traits specialized only for the next types (but may be relatively-easy extended for your custom types):
- `scalars` - anything which detected by [`std::is_scalar<T>`](https://en.cppreference.com/w/cpp/types/is_scalar)
- raw arrays (`T[SIZE]`) and `std::array<T, SIZE>`
  - Important note: since this is `homo-iconic sequences` (all elements the same type, ordered, placed continuously in memory), here is applied the next optimization: if such sequence contains only `scalar` data - copying done only once (like for single item, since it is safe), otherwise (non-`scalar` types) - processing perfomed for each item.
- `std::pair<FirstT, SecondT>` and `std::tuple<Types...>`
  - Since this is `hetero-iconic collection` (all elements ordered, but not placed continuosly in memory, may be various types) - processing always perfomed independently for each item.

Strictly speaking this is not fully compile-time, since `std::memcpy()` is not `constexpr`, but everything else (offset's, sizes) computed in compile-time.

<details>
  <summary>Data packing</summary>

  ## Data packing - manually written way
  ```c++
  const std::int32_t v0{123};
  const std::int64_t v1{234};
  const float        v2{345.f};
  const double       v3{456.0};

  constexpr std::size_t BYTES_COUNT = sizeof(std::int32_t) + sizeof(std::int64_t) + sizeof(float) + sizeof(double);
  static std::array<std::int8_t, BYTES_COUNT> buffer;

  constexpr std::size_t v0_offset = 0;
  constexpr std::size_t v1_offset = v0_offset + sizeof(std::int32_t);
  constexpr std::size_t v2_offset = v1_offset + sizeof(std::int64_t);
  constexpr std::size_t v3_offset = v2_offset + sizeof(float);

  std::memcpy(buffer.data() + v0_offset, &v0, sizeof(std::int32_t));
  std::memcpy(buffer.data() + v1_offset, &v1, sizeof(std::int64_t));
  std::memcpy(buffer.data() + v2_offset, &v2, sizeof(float));
  std::memcpy(buffer.data() + v3_offset, &v3, sizeof(double));
  ```

  -----

  ## Data packing by using library (case #1)
  ```c++
  const std::int32_t v0{123};
  const std::int64_t v1{234};
  const float        v2{345.0f};
  const double       v3{456.0};

  using buffer_t = typename ct::serialization::packer_trait<
      std::int32_t, std::int64_t, float, double
  >::info_t::byte_buffer_t;

  // Perfomance case - packing into static (once-allocated, reusable) buffer
  static buffer_t buffer;

  ct::serialization::pack_into(buffer.data(),
      v0, v1, v2, v3
  );
  ```

  ## Data packing by using library (case #2)
  ```c++
  const std::int32_t v0{123};
  const std::int64_t v1{234};
  const float        v2{345.0f};
  const double       v3{456.0};

  // Common case (implicit types deduction) - packing into created buffer
  const auto buffer = ct::serialization::pack(
      v0, v1, v2, v3
  );
  ```
</details>


<details>
  <summary>Data unpacking</summary>

  ## Data unpacking - manually written way
  ```c++
  std::int32_t v0;
  std::int64_t v1;
  float        v2;
  double       v3;

  constexpr std::size_t BYTES_COUNT = sizeof(std::int32_t) + sizeof(std::int64_t) + sizeof(float) + sizeof(double);
  std::vector<std::int8_t> buffer = { /* received from somewhere, for example over network */ };
  if(buffer.size() >= BYTES_COUNT)
  {
      constexpr std::size_t v0_offset = 0;
      constexpr std::size_t v1_offset = v0_offset + sizeof(std::int32_t);
      constexpr std::size_t v2_offset = v1_offset + sizeof(std::int64_t);
      constexpr std::size_t v3_offset = v2_offset + sizeof(float);

      std::memcpy(&v0, buffer.data() + v0_offset, sizeof(std::int32_t));
      std::memcpy(&v1, buffer.data() + v1_offset, sizeof(std::int64_t));
      std::memcpy(&v2, buffer.data() + v2_offset, sizeof(float));
      std::memcpy(&v3, buffer.data() + v3_offset, sizeof(double));
  }
  ```

  -----

  ## Data unpacking by using library (case #1)
  ```c++
  std::int32_t v0;
  std::int64_t v1;
  float        v2;
  double       v3;

  std::vector<std::int8_t> buffer = { /* received from somewhere, for example over network */ };

  ct::serialization::unpack_from(buffer.data(),
      v0, v1, v2, v3
  );
  ```

  ## Data unpacking by using library (case #2)
  ```c++
  const std::int32_t v0;
  const std::int64_t v1;
  const float        v2;
  const double       v3;

  using buffer_t = typename ct::serialization::packer_trait<
      std::int32_t, std::int64_t, float, double
  >::info_t::byte_buffer_t;

  // Strange case, when provided const-size buffer :)
  buffer_t buffer = { /* received from somewhere, for example over network */ };

  const auto buffer = ct::serialization::unpack(
      v0, v1, v2, v3
  );
  ```
</details>

<details>
  <summary>Debug printing example</summary>

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

</details>
