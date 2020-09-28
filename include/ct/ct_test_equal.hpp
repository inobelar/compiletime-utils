#ifndef CT__TEST__EQUAL_HPP
#define CT__TEST__EQUAL_HPP

#include <array> // for std::array<T, SIZE>

namespace ct {

namespace impl {

// TODO: must be implemented reverse-type comparation, line in `operator ==`
// For example: single implementation [std::array == std::initializer_list]
// must work also for [std::_initializer_list == std::array]

template <typename T_A, typename T_B>
struct equal_trait {
    static constexpr bool equal(const T_A& a, const T_B& b) {
        return (a == b);
    }
};

template <typename ITEM_TYPE_A, typename COLLECTION_TYPE_A,
          typename ITEM_TYPE_B, typename COLLECTION_TYPE_B>
struct collection_equal_utils
{
    using collection_a_t = COLLECTION_TYPE_A;
    using collection_b_t = COLLECTION_TYPE_B;

    using item_a_t = ITEM_TYPE_A;
    using item_b_t = ITEM_TYPE_B;

    // Function pointers for getting collection items
    using a_item_getter = item_a_t const& (*) (const collection_a_t& a, std::size_t idx);
    using b_item_getter = item_b_t const& (*) (const collection_b_t& b, std::size_t idx);

    static constexpr bool items_equal(
            const collection_a_t& a, const collection_b_t& b,
            const std::size_t SIZE_A, const std::size_t SIZE_B,
            const a_item_getter& get_a, const b_item_getter& get_b,
            std::size_t i = 0)
    {
        return (i < SIZE_A) ? ( equal_trait<item_a_t, item_b_t>::equal(get_a(a, i), get_b(b, i)) && items_equal(a, b, SIZE_A, SIZE_B, get_a, get_b, i+1) ) : true;
    }

    static constexpr bool equal(
            const collection_a_t& a, const collection_b_t& b,
            const std::size_t SIZE_A, const std::size_t SIZE_B,
            const a_item_getter& get_a, const b_item_getter& get_b
            )
    {
        return (SIZE_A != SIZE_B) ? false : items_equal(a, b, SIZE_A, SIZE_B, get_a, get_b);
    }
};

// Specialization for comparation: raw array <--> raw array
template <typename ITEM_TYPE_A, std::size_t SIZE_A,
          typename ITEM_TYPE_B, std::size_t SIZE_B>
class equal_trait< ITEM_TYPE_A[SIZE_A], ITEM_TYPE_B[SIZE_B]>
{
public:

    using type_a_t = ITEM_TYPE_A[SIZE_A];
    using type_b_t = ITEM_TYPE_B[SIZE_B];

private:

    static constexpr ITEM_TYPE_A const& get_a(const type_a_t& arr, std::size_t idx) {
        return arr[idx];
    }

    static constexpr ITEM_TYPE_B const& get_b(const type_b_t& arr, std::size_t idx) {
        return arr[idx];
    }

public:

    static constexpr bool equal(const type_a_t& a, const type_b_t& b)
    {
        return collection_equal_utils<ITEM_TYPE_A, type_a_t, ITEM_TYPE_B, type_b_t>::equal(
                    a, b, SIZE_A, SIZE_B,
                    get_a, get_b
        );
    }
};

// Specialization for comparation: std::array <--> std::array
template <typename ITEM_TYPE_A, std::size_t SIZE_A,
          typename ITEM_TYPE_B, std::size_t SIZE_B>
class equal_trait< std::array<ITEM_TYPE_A, SIZE_A>, std::array<ITEM_TYPE_B, SIZE_B>>
{
public:

    using type_a_t = std::array<ITEM_TYPE_A, SIZE_A>;
    using type_b_t = std::array<ITEM_TYPE_B, SIZE_B>;

private:

    static constexpr ITEM_TYPE_A const& get_a(const type_a_t& arr, std::size_t idx) {
        return arr[idx];
    }

    static constexpr ITEM_TYPE_B const& get_b(const type_b_t& arr, std::size_t idx) {
        return arr[idx];
    }

public:

    static constexpr bool equal(const type_a_t& a, const type_b_t& b)
    {
        return collection_equal_utils<ITEM_TYPE_A, type_a_t, ITEM_TYPE_B, type_b_t>::equal(
                    a, b, SIZE_A, SIZE_B,
                    get_a, get_b
        );
    }
};

// Specialization for comparation: std::array <--> std::initializer_list
template <typename T_A, std::size_t SIZE_A, typename T_L>
class equal_trait< std::array<T_A, SIZE_A>, std::initializer_list<T_L> >
{
public:

    using type_a_t = std::array<T_A, SIZE_A>;
    using type_b_t = std::initializer_list<T_L>;

private:

    static constexpr T_A const& get_arr_item(const type_a_t& arr, std::size_t idx) {
        return arr[idx];
    }

    static constexpr T_L const& get_list_item(const type_b_t& list, std::size_t idx) {
        return *(list.begin() + idx);
    }

public:

    static constexpr bool equal(const type_a_t& a, const type_b_t& b) {
        return collection_equal_utils<T_A, type_a_t, T_L, type_b_t>::equal(
                    a, b, SIZE_A, b.size(),
                    get_arr_item, get_list_item
        );
    }
};

} // namespace impl

// -----------------------------------------------------------------------------

template <typename T, typename U>
constexpr bool equal(const T& a, const U& b) {
    return impl::equal_trait<T, U>::equal(a, b);
}

// -----------------------------------------------------------------------------

#if defined(CT_ENABLE_TESTS)
namespace tests {

    // Check of comparation: POD <--> POD
    static_assert( equal(1, 1) == true, "Test failed");

    // Check of comparation: raw array <--> raw array
    // static_assert( equal( (std::int32_t[3]){1,2,3}, (std::int32_t[3]){1,2,3}) == true, "Test failed");

    // Check of comparation: std::array <--> std::array
    static_assert( equal( std::array<std::int32_t, 3>{1,2,3}, std::array<std::int32_t, 3>{1,2,3}) == true, "Test failed");

    // Check of comparation: std::array <--> std::initializer_list
    static_assert( equal( std::array<std::int32_t, 3>{1,2,3}, std::initializer_list<int>{1,2,3}) == true, "Test failed");

} // namespace tests
#endif // defined(CT_ENABLE_TESTS)

} // namespace ct

#endif // CT__TEST__EQUAL_HPP
