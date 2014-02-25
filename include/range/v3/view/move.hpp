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

#ifndef RANGES_V3_VIEW_MOVE_HPP
#define RANGES_V3_VIEW_MOVE_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_adaptor.hpp>
#include <range/v3/utility/debug_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Reference>
            struct move_iterator_reference
            {
                using type = Reference;
            };

            template<typename T>
            struct move_iterator_reference<T &>
            {
                using type = T &&;
            };

            template<typename T>
            using move_iterator_reference_t = typename move_iterator_reference<T>::type;
        }

        template<typename InputRange>
        struct move_range_view
        {
        private:
            InputRange rng_;

            template<bool Const>
            struct basic_iterator;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_adaptor<
                    basic_iterator<Const>
                  , range_iterator_t<detail::add_const_if_t<InputRange, Const>>
                  , use_default
                  , std::input_iterator_tag
                  , detail::move_iterator_reference_t<
                        range_reference_t<detail::add_const_if_t<InputRange, Const>>
                    >
                  , use_default
                  , range_iterator_t<detail::add_const_if_t<InputRange, Const>>
                >
            {
            private:
                friend struct move_range_view;
                friend struct ranges::iterator_core_access;
                using move_range_view_    = detail::add_const_if_t<move_range_view, Const>;
                using iterator_adaptor_   = typename basic_iterator::iterator_adaptor_;
                using base_range          = detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;

                explicit basic_iterator(base_range_iterator it)
                  : iterator_adaptor_(std::move(it))
                {}
                typename iterator_adaptor_::reference dereference() const
                {
                    return std::move(*this->base());
                }
                typename iterator_adaptor_::pointer arrow() const
                {
                    return this->base();
                }
            public:
                constexpr basic_iterator()
                  : iterator_adaptor_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : iterator_adaptor_(std::move(that).base_reference())
                {}
            };

        public:
            using iterator =
                RANGES_DEBUG_ITERATOR(move_range_view, basic_iterator<false>);
            using const_iterator =
                RANGES_DEBUG_ITERATOR(move_range_view const, basic_iterator<true>);

            explicit move_range_view(InputRange && rng)
              : rng_(std::forward<InputRange>(rng))
            {}
            iterator begin()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<false>{ranges::begin(rng_)});
            }
            const_iterator begin() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<true>{ranges::begin(rng_)});
            }
            iterator end()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<false>{ranges::end(rng_)});
            }
            const_iterator end() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    basic_iterator<true>{ranges::end(rng_)});
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange & base()
            {
                return rng_;
            }
            InputRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct mover : bindable<mover>, pipeable<mover>
            {
                template<typename InputRange>
                static move_range_view<InputRange>
                invoke(mover, InputRange && rng)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return move_range_view<InputRange>{std::forward<InputRange>(rng)};
                }
            };

            RANGES_CONSTEXPR mover move {};
        }
    }
}

#endif
