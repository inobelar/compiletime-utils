#include "catch.hpp"

#include "rt/serialization/rt_serialization_bytes_count.hpp"
#include "rt/serialization/rt_serialization_bytes_count_stl.hpp"

#include "rt/serialization/rt_serialization_pack.hpp"
#include "rt/serialization/rt_serialization_pack_stl.hpp"

#include "rt/serialization/rt_serialization_unpack.hpp"
#include "rt/serialization/rt_serialization_unpack_stl.hpp"


TEST_CASE( "Run-time buffer size calculation works", "[rt][ser/deser]")
{
    SECTION( "Simple types check" )
    {
        const std::size_t size = rt::serialization::bytes_count(
                    std::int8_t{1},
                    std::int16_t{2},
                    std::int32_t{3},
                    std::int64_t{4},
                    float{5.f},
                    double{6.0}
        );

        constexpr std::size_t BYTES_COUNT =
                sizeof(std::int8_t)
                + sizeof(std::int16_t)
                + sizeof(std::int32_t)
                + sizeof(std::int64_t)
                + sizeof(float)
                + sizeof(double);
        REQUIRE( size == BYTES_COUNT );
    }

    SECTION( "Containers types check" )
    {
        const std::size_t size = rt::serialization::bytes_count(
                    std::array<std::uint8_t, 3>{1, 2, 3},
                    std::vector<std::int16_t>{4, 5, 6, 7},
                    std::deque<std::int32_t>{8, 9, 10},
                    std::forward_list<std::int64_t>{11, 12, 13, 14},
                    std::list<float>{15, 16, 17}
        );

        constexpr std::size_t BYTES_COUNT =
                  ((sizeof(std::int8_t)  *3) + sizeof(std::uint32_t) )
                + ((sizeof(std::uint16_t)*4) + sizeof(std::uint32_t) )
                + ((sizeof(std::int32_t) *3) + sizeof(std::uint32_t) )
                + ((sizeof(std::int64_t) *4) + sizeof(std::uint32_t) )
                + ((sizeof(float)        *3) + sizeof(std::uint32_t) );
        REQUIRE( size == BYTES_COUNT );
    }

    SECTION( "Nested types check" )
    {
        const std::size_t size = rt::serialization::bytes_count(
                    std::pair<
                        std::array<int, 3>,
                        std::vector<int>
                    > { {1, 2, 3}, {4, 5, 6, 7} },

                    std::tuple<
                        std::list< std::vector<int> >,
                        std::deque<int>,
                        int
                    > { { {8, 9}, {9, 10, 11}, {11, 12} }, {13, 14, 15}, 16}
        );

        constexpr std::size_t BYTES_COUNT =
                  ((sizeof(int)*3) + sizeof(std::uint32_t) )
                + ((sizeof(int)*4) + sizeof(std::uint32_t) )
                +
                (
                      ((sizeof(int)*2) + sizeof(std::uint32_t) )
                    + ((sizeof(int)*3) + sizeof(std::uint32_t) )
                    + ((sizeof(int)*2) + sizeof(std::uint32_t) )
                    + (sizeof(std::uint32_t) )
                )
                +
                  ((sizeof(int)*3) + sizeof(std::uint32_t) )
                + (sizeof(int));
        REQUIRE( size == BYTES_COUNT );
    }
}

template <typename ... Args>
inline std::vector<std::int8_t> pack_into_bytes(const Args& ... args)
{
    const std::size_t bytes_count = rt::serialization::bytes_count(args...);
    std::vector<std::int8_t> bytes(bytes_count);

    rt::serialization::pack(bytes.data(), args...);

    return bytes;
}

TEST_CASE( "Run-time Simple Serialization/Deserialization works", "[rt][ser/deser]"  )
{
    const auto bytes = pack_into_bytes
    (
        std::uint16_t{42},
        std::uint32_t{254},
        std::uint64_t{1337},
        float{3.14},
        double{9.81},
        std::array<int, 3>{1, 2, 3}
    );

    SECTION( "Byte array unpacking is correct" )
    {
        std::uint16_t v0 = 0;
        std::uint32_t v1 = 0;
        std::uint64_t v2 = 0;
        float         v3 = 0;
        double        v4 = 0;
        std::array<int, 3> v5 = {0, 0, 0};

        rt::serialization::unpack(bytes.data(), v0, v1, v2, v3, v4, v5);

        REQUIRE( v0 ==   42 );
        REQUIRE( v1 ==  254 );
        REQUIRE( v2 == 1337 );
        REQUIRE( v3 == Approx(3.14) );
        REQUIRE( v4 == Approx(9.81) );
        REQUIRE( ((v5[0] == 1) && (v5[1] == 2) && (v5[2] == 3)) );
    }

}

// Utility (non-perfomance) functions only for testing purpose
inline namespace utils
{
    template <typename T>
    inline T get_item(const std::forward_list<T>& fwd_list, std::size_t N) {
        auto it = fwd_list.begin();
        std::advance(it, N);
        return *it;
    }

    template <typename T>
    inline T get_item(const std::list<T>& list, std::size_t N) {
        auto it = list.begin();
        std::advance(it, N);
        return *it;
    }
} // namespace utils


TEST_CASE( "Run-time Complex Serialization/Deserialization works", "[rt][ser/deser]"  )
{
    const auto bytes = pack_into_bytes
    (
        std::vector<std::int16_t>{1, 2, 3, 4},
        std::deque<std::int32_t>{5, 6, 7},
        std::forward_list<std::int64_t>{8, 9, 10, 11},
        std::list<float>{12, 13, 14}
    );

    SECTION( "Byte array unpacking is correct" )
    {
        std::vector<std::int16_t>       v0;
        std::deque<std::int32_t>        v1;
        std::forward_list<std::int64_t> v2;
        std::list<float>                v3;

        rt::serialization::unpack(bytes.data(), v0, v1, v2, v3);

        REQUIRE( ((v0[0] == 1) && (v0[1] == 2) && (v0[2] == 3) && (v0[3] == 4)) );
        REQUIRE( ((v1[0] == 5) && (v1[1] == 6) && (v1[2] == 7)) );
        REQUIRE( ( (get_item(v2,0) == 8) && (get_item(v2,1) == 9) && (get_item(v2,2) == 10) && (get_item(v2,3) == 11) ) );
        REQUIRE( ( (get_item(v3,0) == 12) && (get_item(v3,1) == 13) && (get_item(v3,2) == 14) ) );
    }

}

SCENARIO( "Run-time Nested Serialization/Deserialization works", "[rt][ser/deser]" )
{
    enum class dummy_enum { first = 0, second = 1, third = 2 };

    using nested_value_t = std::tuple
    <
        dummy_enum,
        int,

        std::array<int, 3>, // Array of pod-types
        std::array< std::pair<int, short>, 3>, // Array of non-pod types

        std::pair<short, int>, // Pair of pod-types
        std::pair< // Nested pairs
            short,
            std::pair<
                std::array<int, 3>,
                int
            >
        >,

        std::tuple< // Sub-tuple (nested)
            int,
            std::array<short, 3>,
            std::pair<int, int>
        >
    >;

    using array_t = std::array<int, 4>;
    using value_t = int;

    // -------------------------------------------------------------------------

    GIVEN( "Bytes buffer created before packing" )
    {
        // For extreme nested cases, it is really un-convenient to retreive
        // bytes count by duplication of currenlty unknown types
        std::vector<std::int8_t> bytes;

        WHEN( "Data being packed" )
        {
            // Visibility scope reduced
            {
                const nested_value_t nested_value
                {
                    dummy_enum::second,
                    1,

                    {2, 3, 4},
                    { std::pair<int, short>{5, 6}, {7, 8}, {9, 10} },

                    {11, 12},
                    {13, { {14,15,16}, 17}},

                    {18, {19,20,21}, {22, 23}},
                };

                const array_t array {24, 25, 26};

                const value_t value = 27;

                // Data packing
                bytes = pack_into_bytes(nested_value, array, value);
            }

            THEN( "Unpacking produces the same data")
            {
                nested_value_t nested_value;
                array_t array;
                value_t value;

                rt::serialization::unpack(bytes.data(), nested_value, array, value);

                // -------------------------------------------------------------

                REQUIRE( std::get<0>(nested_value) == dummy_enum::second );
                REQUIRE( std::get<1>(nested_value) == 1 );

                REQUIRE( std::get<2>(nested_value)[0] == 2 );
                REQUIRE( std::get<2>(nested_value)[1] == 3 );
                REQUIRE( std::get<2>(nested_value)[2] == 4 );

                REQUIRE( std::get<3>(nested_value)[0].first  == 5 );
                REQUIRE( std::get<3>(nested_value)[0].second == 6 );
                REQUIRE( std::get<3>(nested_value)[1].first  == 7 );
                REQUIRE( std::get<3>(nested_value)[1].second == 8 );
                REQUIRE( std::get<3>(nested_value)[2].first  == 9 );
                REQUIRE( std::get<3>(nested_value)[2].second == 10 );

                REQUIRE( std::get<4>(nested_value).first  == 11 );
                REQUIRE( std::get<4>(nested_value).second == 12 );

                REQUIRE( std::get<5>(nested_value).first  == 13 );
                REQUIRE( std::get<5>(nested_value).second.first[0] == 14 );
                REQUIRE( std::get<5>(nested_value).second.first[1] == 15 );
                REQUIRE( std::get<5>(nested_value).second.first[2] == 16 );
                REQUIRE( std::get<5>(nested_value).second.second   == 17 );

                REQUIRE( std::get<0>( std::get<6>(nested_value) ) == 18 );
                REQUIRE( std::get<1>( std::get<6>(nested_value) )[0] == 19 );
                REQUIRE( std::get<1>( std::get<6>(nested_value) )[1] == 20 );
                REQUIRE( std::get<1>( std::get<6>(nested_value) )[2] == 21 );
                REQUIRE( std::get<2>( std::get<6>(nested_value) ).first  == 22);
                REQUIRE( std::get<2>( std::get<6>(nested_value) ).second == 23);

                // -------------------------------------------------------------

                REQUIRE( ((array[0] == 24) && (array[1] == 25) && (array[2] == 26)) );
                REQUIRE( value == 27 );
            }
        }
    }
}
