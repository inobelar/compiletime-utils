# RT :: Serialization

Examples of usage: [**in tests**](../../../tests/serialization_tests/rt_serialization_test.cpp)

This library basically the same as `ct::serialization`, except not too strict & works with larger count of types (size of which known only in run-time, like `std::vector<T>`). Unlike compile-time version, which being written first, it contains extra run-time overhead:
- size of containers packed (as `std::uint32_t`)
- offsets calculation done in run-time