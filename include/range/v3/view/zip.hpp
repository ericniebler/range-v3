// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_ZIP_HPP
#define RANGES_V3_VIEW_ZIP_HPP

#include <tuple>
#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            constexpr struct
            {
                template<typename T>
                bool operator()(T const &t, T const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            } equal_to {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const & t) const -> decltype(*t)
                {
                    return *t;
                }
            } deref {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    --t;
                }
            } dec {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    ++t;
                }
            } inc {};

            constexpr struct
            {
                template<typename T, typename D>
                void operator()(T & t, D d) const
                {
                    std::advance(t, d);
                }
            } advance {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const &t, T const &u) const ->
                    decltype(u - t)
                {
                    return u - t;
                }
            } distance_to {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? t : u)
                {
                    return t < u ? t : u;
                }
            } min_ {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? u : t)
                {
                    return t < u ? u : t;
                }
            } max_ {};
        } // namespace detail

        template<typename ...InputRanges>
        struct zip_range_view
        {
        private:
            std::tuple<InputRanges...> rngs_;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<false>,
                    std::tuple<range_value_t<InputRanges>...>,
                    detail::common_type_t<range_category_t<InputRanges>...>,
                    std::tuple<range_reference_t<detail::add_const_if_t<InputRanges, Const>>...>,
                    detail::common_type_t<range_difference_t<InputRanges>...>
                >
            {
                using reference = typename basic_iterator::reference;
                using difference_type = typename basic_iterator::difference_type;
            private:
                friend struct zip_range_view;
                friend struct ranges::iterator_core_access;
                using zip_range_view_ = detail::add_const_if_t<zip_range_view, Const>;

                std::tuple<range_iterator_t<detail::add_const_if_t<InputRanges, Const>>...> its_;

                basic_iterator(zip_range_view_ &rng, begin_tag)
                  : its_(tuple_transform(rng.rngs_, ranges::begin))
                {}
                basic_iterator(zip_range_view_ &rng, end_tag)
                  : its_(tuple_transform(rng.rngs_, ranges::end))
                {}
                reference dereference() const
                {
                    return tuple_transform(its_, detail::deref);
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                void increment()
                {
                    tuple_for_each(its_, detail::inc);
                }
                void decrement()
                {
                    tuple_for_each(its_, detail::dec);
                }
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    tuple_for_each(its_, std::bind(detail::advance, _1, n));
                }
                template<bool OtherConst>
                difference_type distance_to(basic_iterator<OtherConst> const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accomodate zippers of sequences of different length.
                    if(0 < std::get<0>(that.its_) - std::get<0>(its_))
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::max)(),
                            detail::min_);
                    else
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::min)(),
                            detail::max_);
                }
            public:
                constexpr basic_iterator()
                  : its_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : its_(std::move(that).its_)
                {}
            };
        public:
            using iterator =
                RANGES_DEBUG_ITERATOR(zip_range_view, basic_iterator<false>);
            using const_iterator =
                RANGES_DEBUG_ITERATOR(zip_range_view const, basic_iterator<true>);

            explicit zip_range_view(InputRanges &&...rngs)
              : rngs_{std::forward<InputRanges>(rngs)...}
            {}
            iterator begin()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<false>{*this, begin_tag{}});
            }
            iterator end()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<false>{*this, end_tag{}});
            }
            const_iterator begin() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<true>{*this, begin_tag{}});
            }
            const_iterator end() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<true>{*this, end_tag{}});
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            std::tuple<InputRanges...> & base()
            {
                return rngs_;
            }
            std::tuple<InputRanges...> const & base() const
            {
                return rngs_;
            }
        };

        namespace view
        {
            struct zipper : bindable<zipper>
            {
                template<typename...InputRanges>
                static zip_range_view<InputRanges...> invoke(zipper, InputRanges &&... rngs)
                {
                    return zip_range_view<InputRanges...>{std::forward<InputRanges>(rngs)...};
                }
            };

            RANGES_CONSTEXPR zipper zip {};
        }
    }
}

#endif
