#include "catch.hpp"

#include "ct/serialization/ct_serialization_pack.hpp"
#include "ct/serialization/ct_serialization_unpack.hpp"

TEST_CASE( "Compile-time offsets calculation works", "[ct][ser/deser]")
{
    SECTION( "Flat types offsets check" )
    {
        constexpr auto offsets = ct::serialization::utils::offsets::get_offsets
        <
            std::int8_t,
            std::int32_t,
            std::int32_t,
            std::array<std::int32_t, 3>,
            std::int64_t
        >();

        REQUIRE( offsets.size() == 5 );
        REQUIRE( offsets[0] == 0 );
        REQUIRE( offsets[1] == (sizeof(std::int8_t)) );
        REQUIRE( offsets[2] == (sizeof(std::int8_t)+sizeof(std::int32_t)) );
        REQUIRE( offsets[3] == (sizeof(std::int8_t)+sizeof(std::int32_t)+sizeof(std::int32_t)) );
        REQUIRE( offsets[4] == (sizeof(std::int8_t)+sizeof(std::int32_t)+sizeof(std::int32_t)+(sizeof(std::int32_t)*3)) );
    }

    SECTION( "Nested types offsets check" )
    {
        constexpr auto offsets = ct::serialization::utils::offsets::get_offsets
        <
            std::pair<std::int32_t, std::int64_t>,
            std::tuple<std::int8_t, std::array<std::int16_t, 3>, std::int32_t>,
            std::int64_t
        >();

        REQUIRE( offsets.size() == 3 );
        REQUIRE( offsets[0] == 0 );
        REQUIRE( offsets[1] == (sizeof(std::int32_t)+sizeof(std::int64_t)) );
        REQUIRE( offsets[2] == (sizeof(std::int32_t)+sizeof(std::int64_t))+(sizeof(std::int8_t)+(sizeof(std::int16_t)*3)+sizeof(std::int32_t)) );
    }

}

TEST_CASE( "Compile-time simple Serialization/Deserialization works", "[ct][ser/deser]" )
{
    // Packing multiple values into fixed-size bytes array (which size known at
    // compile-time, deducted from arguments types)
    const auto bytes = ct::serialization::pack
    (
        std::uint16_t{42},
        std::uint32_t{254},
        std::uint64_t{1337},
        float{3.14},
        double{9.81},
        std::array<int, 3>{1, 2, 3}
    );

    SECTION( "Packed bytes count is correct" )
    {
        constexpr std::size_t BYTES_COUNT
                = sizeof(std::uint16_t)
                + sizeof(std::uint32_t)
                + sizeof(std::uint64_t)
                + sizeof(float)
                + sizeof(double)
                + (sizeof(int) * 3);

        REQUIRE( bytes.size() == BYTES_COUNT );
    }

    SECTION( "Byte array unpacking is correct" )
    {
        std::uint16_t v0 = 0;
        std::uint32_t v1 = 0;
        std::uint64_t v2 = 0;
        float         v3 = 0;
        double        v4 = 0;
        std::array<int, 3> v5 = {0, 0, 0};

        ct::serialization::unpack(bytes, v0, v1, v2, v3, v4, v5);

        REQUIRE( v0 ==   42 );
        REQUIRE( v1 ==  254 );
        REQUIRE( v2 == 1337 );
        REQUIRE( v3 == Approx(3.14) );
        REQUIRE( v4 == Approx(9.81) );
        REQUIRE( ((v5[0] == 1) && (v5[1] == 2) && (v5[2] == 3)) );
    }

    SECTION( "Extra: byte array unpacking (as tuple) is correct" )
    {
        std::uint16_t v0 = 0;
        std::uint32_t v1 = 0;
        std::uint64_t v2 = 0;
        float         v3 = 0;
        double        v4 = 0;
        std::array<int, 3> v5 = {0, 0, 0};

        std::tie(v0, v1, v2, v3, v4, v5)
                = ct::serialization::unpack_as_tuple
                <
                    std::uint16_t,
                    std::uint32_t,
                    std::uint64_t,
                    float,
                    double,
                    std::array<int, 3>
                >(bytes);

        REQUIRE( v0 ==   42 );
        REQUIRE( v1 ==  254 );
        REQUIRE( v2 == 1337 );
        REQUIRE( v3 == Approx(3.14) );
        REQUIRE( v4 == Approx(9.81) );
        REQUIRE( ((v5[0] == 1) && (v5[1] == 2) && (v5[2] == 3)) );
    }

}

SCENARIO( "Nested Serialization/Deserialization works", "[ct][ser/deser]" )
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
        using byte_array_t = ct::serialization::byte_buffer_t<nested_value_t, array_t, value_t>;
        byte_array_t bytes {0}; // Filled by zeroes, why not

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
                ct::serialization::pack_into(bytes.data(), nested_value, array, value);
            }

            THEN( "Unpacking produces the same data")
            {
                nested_value_t nested_value;
                array_t array;
                value_t value;

                ct::serialization::unpack_from(bytes.data(), nested_value, array, value);

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

TEST_CASE( "Structures Serialization/Deserialization works", "[ct][ser/deser]" )
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Custom structures definition

    struct Vec3 {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        Vec3() = default;
        Vec3(float x_, float y_, float z_)
            : x(x_), y(y_), z(z_)
        {}
    };

    struct Color {
        int r = 0, g = 0, b = 0;
        Color() = default;
        Color(int r_, int g_, int b_)
            : r(r_), g(g_), b(b_)
        {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Custom structures packing-functions

    static const auto pack = [](const Vec3& vec, const Color& col)
    {
        return ct::serialization::pack(
            vec.x, vec.y, vec.z,
            col.r, col.g, col.b
        );
    };

    static const auto unpack = [](const signed char* bytes, Vec3& vec, Color& col)
    {
        ct::serialization::unpack_from(bytes,
            vec.x, vec.y, vec.z,
            col.r, col.g, col.b
        );
    };

    // -------------------------------------------------------------------------

    const Vec3  vec = {1.2f, 3.4f, 5.6f};
    const Color col = {165, 178, 254};

    const auto bytes = pack(vec, col);

    // -------------------------------------------------------------------------

    SECTION( "Unpacking test" )
    {
        Vec3 vec_unpacked;
        Color col_unpacked;

        unpack(bytes.data(), vec_unpacked, col_unpacked);

        REQUIRE(vec_unpacked.x == Approx(1.2) );
        REQUIRE(vec_unpacked.y == Approx(3.4) );
        REQUIRE(vec_unpacked.z == Approx(5.6) );

        REQUIRE(col_unpacked.r == 165 );
        REQUIRE(col_unpacked.g == 178 );
        REQUIRE(col_unpacked.b == 254 );
    }
}
