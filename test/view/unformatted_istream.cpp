// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Google LLC 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
#include <range/v3/view/unformatted_istream.hpp>

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/stride.hpp>
#include <string>
#include <sstream>
#include <vector>

#include <range/v3/iterator.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace rv = ranges::views;

namespace std {
    template<typename T>
    std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
    {
        os << std::hex;
        ranges::copy(v, ::ranges::ostream_iterator<>(os, " "));
        return os;
    }
}

int main()
{
    {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        auto const input = std::vector<std::string>(3, "\x21\x22\x23\x24");
#else
        auto const input = std::vector<std::string>{
            "\x21\x22\x23\x24",
            "\x22\x21\x24\x23",
            "\x24\x23\x22\x21",
        };
#endif // endianness
        {
            auto in = std::istringstream(input[0]);
            auto const expected = std::vector<std::uint8_t>{0x21, 0x22, 0x23, 0x24};
            auto const actual = ranges::unformatted_istream_view<std::uint8_t>(in) | ranges::to<std::vector>;
            CHECK(actual == expected);
        }

        {
            auto in = std::istringstream(input[1]);
            auto const expected = std::vector<std::uint16_t>{0x2122, 0x2324};
            auto const actual = ranges::unformatted_istream_view<std::uint16_t>(in) | ranges::to<std::vector>;
            CHECK(actual == expected);
        }
        {
            auto in = std::istringstream(input[2]);
            auto const expected = std::vector<std::uint32_t>{0x21222324};
            auto const actual = ranges::unformatted_istream_view<std::uint32_t>(in) | ranges::to<std::vector>;
            CHECK(actual == expected);
        }
        {
            auto in = std::istringstream(input[2]);
            auto const actual = ranges::unformatted_istream_view<std::uint64_t>(in) | ranges::to<std::vector>;
            CHECK(in.fail());
        }
#if __cplusplus >= 201703L
        {
            auto in = std::istringstream(input[2]);
            auto const expected = std::vector<float>{0x1.4446480000000000179Dp-61f};
            auto const actual = ranges::unformatted_istream_view<float>(in) | ranges::to<std::vector>;
            CHECK(actual == expected);
        }
#endif // __cplusplus >= 201703L
    }

    return ::test_result();
}
