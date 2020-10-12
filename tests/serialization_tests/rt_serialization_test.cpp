#include "catch.hpp"

#include "rt/serialization/rt_serialization_bytes_count.hpp"
#include "rt/serialization/rt_serialization_bytes_count_std.hpp"

#include "rt/serialization/rt_serialization_pack.hpp"
#include "rt/serialization/rt_serialization_pack_std.hpp"

#include "rt/serialization/rt_serialization_unpack.hpp"
#include "rt/serialization/rt_serialization_unpack_std.hpp"


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

        REQUIRE(size == (sizeof(std::int8_t)+sizeof(std::int16_t)+sizeof(std::int32_t)+sizeof(std::int64_t)+sizeof(float)+sizeof(double)));
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

        REQUIRE(size == (sizeof(std::int8_t)*3)+(sizeof(std::uint16_t)*4)+(sizeof(std::int32_t)*3)+(sizeof(std::int64_t)*4)+(sizeof(float)*3));
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

        REQUIRE(size ==
                    ((sizeof(int)*3) + (sizeof(int)*4)) +
                    ((sizeof(int)*2)+(sizeof(int)*3)+(sizeof(int)*2) + (sizeof(int)*3) + (sizeof(int)))
                );
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

TEST_CASE( "Run-time simple Serialization/Deserialization works", "[rt][ser/deser]"  )
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
