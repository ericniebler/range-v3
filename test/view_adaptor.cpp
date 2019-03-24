// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/delimit.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

template<typename BidiRange>
struct my_reverse_view
  : ranges::view_adaptor<my_reverse_view<BidiRange>, BidiRange>
{
private:
    CONCEPT_ASSERT(ranges::BidirectionalRange<BidiRange>());
    CONCEPT_ASSERT(ranges::BoundedRange<BidiRange>());
    friend ranges::range_access;
    using base_iterator_t = ranges::iterator_t<BidiRange>;

    struct adaptor : ranges::adaptor_base
    {
        template<class base_mixin>
        struct mixin : base_mixin
        {
            mixin() = default;
            using base_mixin::base_mixin;

            int mixin_int = 120;

            int base_plus_adaptor() const
            {
                int y = this->get().t;
                return *this->base() + y;
            }
        };

        int t = 20;

        // Cross-wire begin and end.
        base_iterator_t begin(my_reverse_view const &rng) const
        {
            return ranges::end(rng.base());
        }
        base_iterator_t end(my_reverse_view const &rng) const
        {
            return ranges::begin(rng.base());
        }
        void next(base_iterator_t &it) const
        {
            --it;
        }
        void prev(base_iterator_t &it) const
        {
            ++it;
        }
        ranges::range_reference_t<BidiRange> read(base_iterator_t it) const
        {
            return *ranges::prev(it);
        }
        CONCEPT_REQUIRES(ranges::RandomAccessRange<BidiRange>())
        void advance(base_iterator_t &it, ranges::range_difference_type_t<BidiRange> n) const
        {
            it -= n;
        }
        CONCEPT_REQUIRES(ranges::SizedSentinel<base_iterator_t, base_iterator_t>())
        ranges::range_difference_type_t<BidiRange>
        distance_to(base_iterator_t const &here, base_iterator_t const &there) const
        {
            return here - there;
        }
    };
    adaptor begin_adaptor() const
    {
        return {};
    }
    adaptor end_adaptor() const
    {
        return {};
    }
public:
    using my_reverse_view::view_adaptor::view_adaptor;
};

struct my_delimited_range
  : ranges::view_adaptor<
        my_delimited_range,
        ranges::delimit_view<ranges::istream_range<int>, int>>
{
    using view_adaptor::view_adaptor;

    struct adaptor : ranges::adaptor_base
    {
        template<class base_mixin>
        struct mixin : base_mixin
        {
            mixin() = default;
            using base_mixin::base_mixin;

            int mixin_int = 120;

            int adaptor_access_test() const
            {
                int y = this->get().t;
                return y;
            }
        };

        int t = 20;
    };
    adaptor begin_adaptor() const
    {
        return {};
    }
    adaptor end_adaptor() const
    {
        return {};
    }
};

int main()
{
    using namespace ranges;
    std::vector<int> v{1, 2, 3, 4};
    my_reverse_view<std::vector<int>&> retro{v};
    ::models<concepts::BoundedView>(aux::copy(retro));
    ::models<concepts::RandomAccessIterator>(retro.begin());
    ::check_equal(retro, {4, 3, 2, 1});

    // test cursor mixin
    CHECK( retro.begin().mixin_int == 120 );
    CHECK( *((retro.begin()+1).base()) == 4 );
    CHECK( (retro.begin()+1).base_plus_adaptor() == 24 );

    std::list<int> l{1, 2, 3, 4};
    my_reverse_view<std::list<int>& > retro2{l};
    ::models<concepts::BoundedView>(aux::copy(retro2));
    ::models<concepts::BidirectionalIterator>(retro2.begin());
    ::models_not<concepts::RandomAccessIterator>(retro2.begin());
    ::check_equal(retro2, {4, 3, 2, 1});

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    my_delimited_range r{view::delimit(istream<int>(sinx), 42)};
    ::models<concepts::View>(aux::copy(r));
    ::models_not<concepts::BoundedView>(aux::copy(r));
    ::models<concepts::InputIterator>(r.begin());
    ::models_not<concepts::ForwardIterator>(r.begin());
    ::check_equal(r, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});

    // test sentinel mixin
    CHECK(r.end().mixin_int == 120);
    CHECK(r.end().adaptor_access_test() == 20);

    return ::test_result();
}
