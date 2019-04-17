// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <array>

#include <concepts/concepts.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/collapse_if.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/reverse.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

struct is_below_5
{
    bool operator()(int i) const
    {
        return i < 5;
    }
};

int main()
{
    using namespace ranges;

    auto collapse_below_5 = view::collapse_if(is_below_5());

    auto && rng = view::empty<int> | collapse_below_5;
    static_assert(range_cardinality<decltype(rng)>{} == cardinality(0), "");
    CPP_assert(Same<int &, range_reference_t<decltype((rng))>>);
    models<BidirectionalViewConcept>(aux::copy(rng));
    CPP_assert(!RandomAccessIterator<iterator_t<decltype((rng))>>);
    models<CommonRangeConcept>(aux::copy(rng));
    models<SizedRangeConcept>(aux::copy(rng));

    const std::array<int, 0> ia{};

    auto && rng2 = ia | collapse_below_5;
    static_assert(range_cardinality<decltype(rng2)>{} == cardinality(0), "");
    CPP_assert(Same<int const &, range_reference_t<decltype((rng2))>>);
    models<BidirectionalViewConcept>(aux::copy(rng2));
    CPP_assert(!RandomAccessIterator<iterator_t<decltype((rng2))>>);
    models<CommonRangeConcept>(aux::copy(rng2));
    models<SizedRangeConcept>(aux::copy(rng2));

    auto && rng3 = view::single(42) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng3)>{} == cardinality(1), "");
    has_type<int &>(*begin(rng3));
    models<BidirectionalViewConcept>(aux::copy(rng3));
    models_not<RandomAccessIteratorConcept>(begin(rng3));
    models<CommonRangeConcept>(aux::copy(rng3));
    CPP_assert(SizedRange<decltype(rng2)>);
    check_equal(rng3, {42});

    int const ib[] = {42};

    auto && rng4 = ib | collapse_below_5;
    static_assert(range_cardinality<decltype(rng4)>{} == cardinality(1), "");
    has_type<int const &>(*begin(rng4));
    models<BidirectionalViewConcept>(aux::copy(rng4));
    models_not<RandomAccessIteratorConcept>(begin(rng4));
    models<CommonRangeConcept>(aux::copy(rng4));
    models<SizedRangeConcept>(aux::copy(rng4));
    check_equal(rng4, {42});
    check_equal(&front(rng4), &ib[0]);

    auto && rng5 = ib | view::reverse | collapse_below_5;
    static_assert(range_cardinality<decltype(rng5)>{} == cardinality(1), "");
    has_type<int const &>(*begin(rng5));
    models<BidirectionalViewConcept>(aux::copy(rng5));
    models_not<RandomAccessIteratorConcept>(begin(rng5));
    models<CommonRangeConcept>(aux::copy(rng5));
    models<SizedRangeConcept>(aux::copy(rng5));
    check_equal(rng5, {42});
    check_equal(&front(rng5), &ib[0]);

    int const ic[] = {0};

    auto && rng6 = ic | collapse_below_5;
    check_equal(rng6, {0});
    check_equal(&front(rng6), &ic[0]);

    auto && rng7 = ic | view::reverse | collapse_below_5;
    check_equal(rng7, {0});
    check_equal(&front(rng7), &ic[0]);

    int const id[] = {42, 1337, 1729};

    auto && rng8 = id | collapse_below_5;
    static_assert(range_cardinality<decltype(rng8)>{} == finite, "");
    models_not<SizedRangeConcept>(aux::copy(rng8));
    check_equal(view::addressof(rng8), view::addressof(id));

    auto && rng9 = id | view::reverse | collapse_below_5;
    static_assert(range_cardinality<decltype(rng9)>{} == finite, "");
    models_not<SizedRangeConcept>(aux::copy(rng9));
    check_equal(view::addressof(rng9), view::addressof(rng9.base()));

    int const ie[] = {0, 1, 2, 3, 4};

    auto && rng10 = ie | collapse_below_5;
    check_equal(rng10, {0});
    check_equal(&front(rng10), &ie[0]);

    auto && rng11 = ie | view::reverse | collapse_below_5;
    check_equal(rng11, {4});
    check_equal(&front(rng11), &ie[4]);

    int const ig[] = {0, 5, 1, 6, 2, 7, 3, 8, 4, 9};

    auto && rng12 = ig | collapse_below_5;
    check_equal(view::addressof(rng12), view::addressof(ig));

    auto ih = view::reverse(ig);

    auto && rng13 = ih | collapse_below_5;
    check_equal(view::addressof(rng13), view::addressof(ih));

    int const ii[] = {0, 1, 5, 6, 2, 3, 7, 8};

    auto && rng14 = ii | collapse_below_5;
    check_equal(rng14, {0, 5, 6, 2, 7, 8});
    check_equal(view::addressof(rng14), {&ii[0], &ii[2], &ii[3], &ii[4], &ii[6], &ii[7]});

    auto ij = view::reverse(ii);

    auto && rng15 = ij | collapse_below_5;
    check_equal(rng15, {8, 7, 3, 6, 5, 1});
    check_equal(view::addressof(rng15), {&ii[7], &ii[6], &ii[5], &ii[3], &ii[2], &ii[1]});

    int const ik[] = {0, 1, 2, 3, 4, 5};
    constexpr auto ks = distance(ik);

    auto && rng16 = make_subrange(bidirectional_iterator<int const*, true>(ik),
        bidirectional_iterator<int const*, true>(ik + ks)) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng16)>{} == unknown, "");
    has_type<int const &>(*begin(rng16));
    models<BidirectionalViewConcept>(aux::copy(rng16));
    models_not<RandomAccessIteratorConcept>(begin(rng16));
    models<CommonRangeConcept>(aux::copy(rng16));
    models_not<SizedRangeConcept>(aux::copy(rng16));
    check_equal(view::addressof(rng16), {&ik[0], &ik[ks - 1]});

    auto && rng17 = make_subrange(bidirectional_iterator<int const*>(ik),
        sentinel<int const*>(ik + ks)) | view::reverse | collapse_below_5;
    static_assert(range_cardinality<decltype(rng17)>{} == unknown, "");
    has_type<int const &>(*begin(rng17));
    models<BidirectionalViewConcept>(aux::copy(rng17));
    models_not<RandomAccessIteratorConcept>(begin(rng17));
    models<CommonRangeConcept>(aux::copy(rng17));
    models_not<SizedRangeConcept>(aux::copy(rng17));
    check_equal(view::addressof(rng17), {&ik[ks - 1], &ik[ks - 2]});

    int const il[] = {5, 4, 3, 2, 1, 0};
    constexpr auto ls = distance(il);

    auto && rng18 = make_subrange(forward_iterator<int const*>(il),
        forward_iterator<int const*>(il + ls)) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng18)>{} == unknown, "");
    has_type<int const &>(*begin(rng18));
    models<ForwardViewConcept>(aux::copy(rng18));
    models_not<BidirectionalIteratorConcept>(begin(rng18));
    models<CommonRangeConcept>(aux::copy(rng18));
    models_not<SizedRangeConcept>(aux::copy(rng18));
    check_equal(view::addressof(rng18), {&il[0], &il[1]});

    int const im[] = {2, 3, 4, 5, 6, 5, 4, 3, 2};
    constexpr auto ms = distance(im);

    auto && rng19 = make_subrange(forward_iterator<int const*, true>(im),
        sentinel<int const*, true>(im + ms)) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng19)>{} == unknown, "");
    has_type<int const &>(*begin(rng19));
    models<ForwardViewConcept>(aux::copy(rng19));
    models_not<BidirectionalIteratorConcept>(begin(rng19));
    models_not<CommonRangeConcept>(aux::copy(rng19));
    models_not<SizedRangeConcept>(aux::copy(rng19));
    check_equal(rng19, {2, 5, 6, 5, 4});
    check_equal(view::addressof(rng19), {&im[0], &im[3], &im[4], &im[5], &im[6]});

    int const in[] = {6, 5, 4, 3, 2, 3, 4, 5, 6};
    constexpr auto ns = distance(in);

    auto && rng20 = make_subrange(input_iterator<int const*>(in),
        input_iterator<int const*>(in + ns)) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng20)>{} == unknown, "");
    has_type<int const &>(*begin(rng20));
    models<InputViewConcept>(aux::copy(rng20));
    models_not<ForwardIteratorConcept>(begin(rng20));
    models<CommonRangeConcept>(aux::copy(rng20));
    models_not<SizedRangeConcept>(aux::copy(rng20));
    check_equal(rng20, {6, 5, 4, 5, 6});
    check_equal(view::addressof(rng20), {&in[0], &in[1], &in[2], &in[ns - 2], &in[ns - 1]});

    int const is[] = {6, 5, 4, 5, 6};
    constexpr auto ss = distance(is);

    auto && rng21 = make_subrange(input_iterator<int const*>(is),
        sentinel<int const*>(is + ss)) | collapse_below_5;
    static_assert(range_cardinality<decltype(rng21)>{} == unknown, "");
    has_type<int const &>(*begin(rng21));
    models<InputViewConcept>(aux::copy(rng21));
    models_not<ForwardIteratorConcept>(begin(rng21));
    models_not<CommonRangeConcept>(aux::copy(rng21));
    models_not<SizedRangeConcept>(aux::copy(rng21));
    check_equal(view::addressof(rng21), view::addressof(is));

    int const it[] = {3, 4, 5, 4, 3};

    auto && rng22 = it | collapse_below_5;
    check_equal(rng22, {3, 5, 4});
    check_equal(view::addressof(rng22), {&it[0], &it[2], &it[3]});

    int const iu[] = {6, 5, 4, 3};

    auto && rng23 = iu | collapse_below_5;
    check_equal(rng23, {6, 5, 4});
    check_equal(view::addressof(rng23), {&iu[0], &iu[1], &iu[2]});

    int const iv[] = {3, 4, 5, 6};

    auto && rng24 = iv | collapse_below_5;
    check_equal(rng24, {3, 5, 6});
    check_equal(view::addressof(rng24), {&iv[0], &iv[2], &iv[3]});

    return test_result();
}
