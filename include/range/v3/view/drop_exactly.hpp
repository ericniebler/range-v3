/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_DROP_EXACTLY_HPP
#define RANGES_V3_VIEW_DROP_EXACTLY_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct drop_exactly_view
          : view_interface<drop_exactly_view<Rng>, is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private meta::if_<
                RandomAccessRange<Rng>,
                meta::nil_,
                box<optional<range_iterator_t<Rng>>, begin_tag>>
        {
        private:
            friend range_access;
            using difference_type_ = range_difference_t<Rng>;
            Rng rng_;
            difference_type_ n_;

            // RandomAccessRange == true
            range_iterator_t<Rng> get_begin_(std::true_type) const
            {
                return next(ranges::begin(rng_), n_);
            }
            // RandomAccessRange == false
            range_iterator_t<Rng> get_begin_(std::false_type)
            {
                auto &begin_ = ranges::get<begin_tag>(*this);
                if(!begin_)
                    begin_ = next(ranges::begin(rng_), n_);
                return *begin_;
            }
            // RandomAccessRange == true
            void dirty_(std::true_type) const
            {}
            // RandomAccessRange == false
            void dirty_(std::false_type)
            {
                auto &begin_ = ranges::get<begin_tag>(*this);
                begin_.reset();
            }
        public:
            drop_exactly_view() = default;
            drop_exactly_view(drop_exactly_view &&that)
              : rng_(std::move(that).rng_), n_(that.n_)
            {}
            drop_exactly_view(drop_exactly_view const &that)
              : rng_(that.rng_), n_(that.n_)
            {}
            drop_exactly_view(Rng rng, difference_type_ n)
              : rng_(std::move(rng)), n_(n)
            {
                RANGES_ASSERT(n >= 0);
            }
            drop_exactly_view& operator=(drop_exactly_view &&that)
            {
                rng_ = std::move(that).rng_;
                n_ = that.n_;
                this->dirty_(RandomAccessRange<Rng>{});
                return *this;
            }
            drop_exactly_view& operator=(drop_exactly_view const &that)
            {
                rng_ = that.rng_;
                n_ = that.n_;
                this->dirty_(RandomAccessRange<Rng>{});
                return *this;
            }
            range_iterator_t<Rng> begin()
            {
                return this->get_begin_(RandomAccessRange<Rng>{});
            }
            range_sentinel_t<Rng> end()
            {
                return ranges::end(rng_);
            }
            template<typename BaseRng = Rng,
                CONCEPT_REQUIRES_(RandomAccessRange<BaseRng const>())>
            range_iterator_t<BaseRng const> begin() const
            {
                return this->get_begin_(std::true_type{});
            }
            template<typename BaseRng = Rng,
                CONCEPT_REQUIRES_(RandomAccessRange<BaseRng const>())>
            range_sentinel_t<BaseRng const> end() const
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_t<Rng> size() const
            {
                return ranges::size(rng_) - static_cast<range_size_t<Rng>>(n_);
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size()
            {
                return ranges::size(rng_) - static_cast<range_size_t<Rng>>(n_);
            }
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct drop_exactly_fn
            {
            private:
                friend view_access;
                template<typename Int,
                    CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(drop_exactly_fn drop_exactly, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(drop_exactly, std::placeholders::_1, n))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Int,
                    CONCEPT_REQUIRES_(!Integral<Int>())>
                static detail::null_pipe bind(drop_exactly_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::drop_exactly must be Integral");
                    return {};
                }
            #endif
                template<typename Rng>
                static drop_exactly_view<all_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::InputRange*)
                {
                    return {all(std::forward<Rng>(rng)), n};
                }
                template<typename Rng, CONCEPT_REQUIRES_(!View<Rng>() && std::is_lvalue_reference<Rng>())>
                static iterator_range<range_iterator_t<Rng>, range_sentinel_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::RandomAccessRange*)
                {
                    return {next(begin(rng), n), end(rng)};
                }
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                auto operator()(Rng && rng, range_difference_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    drop_exactly_fn::invoke_(std::forward<Rng>(rng), n, range_concept<Rng>{})
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!(InputRange<Rng>() && Integral<T>()))>
                void operator()(Rng &&, T) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::drop_exactly must be a model of the InputRange concept");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::drop_exactly must be a model of the Integral concept");
                }
            #endif
            };

            /// \relates drop_exactly_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<drop_exactly_fn>, drop_exactly)
        }
        /// @}
    }
}

#endif
