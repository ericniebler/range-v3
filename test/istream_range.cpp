#include <range/v3/istream_range.hpp>
#include <range/v3/range.hpp>
#include <sstream>
#include "simple_test.hpp"
#include "test_utils.hpp"

struct moveonly
{
    char c;

    moveonly() = default;
    moveonly(moveonly &&) = default;
    moveonly& operator=(moveonly &&) & = default;

    operator char() const
    {
        return c;
    }
    friend std::istream &operator>>(std::istream &is, moveonly &m)
    {
        is.get(m.c);
        return is;
    }
};

int main()
{
    static const char test[] = "abcd3210";
    std::istringstream ss{test};
    ::check_equal(ranges::istream<moveonly>(ss),
                  ranges::make_range(test, test + sizeof(test) - 1));
    return ::test_result();
}
