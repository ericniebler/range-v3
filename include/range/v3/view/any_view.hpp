/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_ANY_VIEW_HPP
#define RANGES_V3_VIEW_ANY_VIEW_HPP

#include <typeinfo>
#include <type_traits>
#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/memory.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        enum class category
        {
            input,
            forward,
            bidirectional,
            random_access
        };

        /// \cond
        namespace detail
        {
            // workaround the fact that typeid ignores cv-qualifiers
            template<typename> struct rtti_tag {};

            struct any_ref
            {
                any_ref() = default;
                template<class T>
                constexpr any_ref(T &obj) noexcept
                  : obj_{std::addressof(obj)}
#ifndef NDEBUG
                  , info_{&typeid(rtti_tag<T>)}
#endif
                {}
                template<class T>
                T &get() const noexcept
                {
                    RANGES_ASSERT(obj_ && info_ && *info_ == typeid(rtti_tag<T>));
                    return *const_cast<T *>(static_cast<T const volatile *>(obj_));
                }
            private:
                void const volatile *obj_ = nullptr;
#ifndef NDEBUG
                std::type_info const *info_ = nullptr;
#endif
            };

            template<typename Base>
            struct cloneable
              : Base
            {
                using Base::Base;
                virtual ~cloneable() = default;
                cloneable() = default;
                cloneable(cloneable const &) = delete;
                cloneable &operator=(cloneable const &) = delete;
                virtual std::unique_ptr<cloneable> clone() const = 0;
            };

            constexpr category to_cat_(concepts::InputRange *) { return category::input; }
            constexpr category to_cat_(concepts::ForwardRange *) { return category::forward; }
            constexpr category to_cat_(concepts::BidirectionalRange *) { return category::bidirectional; }
            constexpr category to_cat_(concepts::RandomAccessRange *) { return category::random_access; }

            template<typename Rng, typename Ref>
            using AnyCompatibleRange = ConvertibleTo<range_reference_t<Rng>, Ref>;

            template<typename Ref>
            struct any_input_view_interface
            {
                virtual ~any_input_view_interface() = default;
                virtual void init() = 0;
                virtual bool done() const = 0;
                virtual Ref read() const = 0;
                virtual void next() = 0;
            };

            template<typename Ref>
            struct any_input_view_cursor
            {
                using single_pass = std::true_type;

                any_input_view_cursor() = default;
                constexpr any_input_view_cursor(any_input_view_interface<Ref> &view) noexcept
                  : view_{std::addressof(view)}
                {}
                Ref read() const { return view_->read(); }
                void next() { view_->next(); }
                bool equal(any_input_view_cursor const &) const noexcept
                {
                    return true;
                }
                bool equal(default_sentinel) const
                {
                    return !view_ || view_->done();
                }
            private:
                any_input_view_interface<Ref> *view_ = nullptr;
            };

            template<typename Rng, typename Ref>
            struct any_input_view_impl
              : any_input_view_interface<Ref>
              , tagged_compressed_tuple<tag::range(Rng),
                    tag::current(iterator_t<Rng>), tag::end(sentinel_t<Rng>)>
            {
            private:
                CONCEPT_ASSERT(AnyCompatibleRange<Rng, Ref>());

                using tagged_t = tagged_compressed_tuple<tag::range(Rng),
                    tag::current(iterator_t<Rng>), tag::end(sentinel_t<Rng>)>;
                using tagged_t::range;
                using tagged_t::current;
                using tagged_t::end;

                virtual void init() override
                {
                    auto &rng = range();
                    end() = ranges::end(rng);
                    current() = ranges::begin(rng);
                }
                virtual bool done() const override { return current() == end(); }
                virtual Ref read() const override { return *current(); }
                virtual void next() override { ++current(); }
            public:
                explicit any_input_view_impl(Rng rng_)
                  : tagged_t{std::move(rng_), iterator_t<Rng>{}, sentinel_t<Rng>{}}
                {}
                any_input_view_impl(any_input_view_impl const &) = delete;
                any_input_view_impl &operator=(any_input_view_impl const &) = delete;
            };

            template<typename Ref, category Cat = category::forward>
            struct any_cursor_interface
            {
                virtual ~any_cursor_interface() = default;
                virtual any_ref iter() const = 0;
                virtual Ref read() const = 0;
                virtual bool equal(any_cursor_interface const &) const = 0;
                virtual void next() = 0;
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::bidirectional>
              : any_cursor_interface<Ref, category::forward>
            {
                virtual void prev() = 0;
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::random_access>
              : any_cursor_interface<Ref, category::bidirectional>
            {
                virtual void advance(std::ptrdiff_t) = 0;
                virtual std::ptrdiff_t distance_to(any_cursor_interface const &) const = 0;
            };

            template<typename Ref, category Cat>
            using any_cloneable_cursor_interface =
                cloneable<any_cursor_interface<Ref, Cat>>;

            template<typename I, typename Ref, category Cat>
            struct any_cursor_impl
              : any_cloneable_cursor_interface<Ref, Cat>
            {
            private:
                CONCEPT_ASSERT(ConvertibleTo<reference_t<I>, Ref>());
                CONCEPT_ASSERT(Cat >= category::forward);
                using Forward = any_cursor_interface<Ref, category::forward>;

                I it_;

            public:
                any_cursor_impl() = default;
                any_cursor_impl(I it)
                  : it_{std::move(it)}
                {}
                any_ref iter() const // override
                {
                    return it_;
                }
                Ref read() const // override
                {
                    return *it_;
                }
                bool equal(Forward const &that_) const // override
                {
                    auto &that = polymorphic_downcast<any_cursor_impl const &>(that_);
                    return that.it_ == it_;
                }
                void next() // override
                {
                    ++it_;
                }
                std::unique_ptr<any_cloneable_cursor_interface<Ref, Cat>> clone() const // override
                {
                    return detail::make_unique<any_cursor_impl>(it_);
                }
                void prev() // override (sometimes; it's complicated)
                {
                    --it_;
                }
                void advance(std::ptrdiff_t n) // override-ish
                {
                    it_ += n;
                }
                std::ptrdiff_t distance_to(
                    any_cursor_interface<Ref, Cat> const &that_) const // override-ish
                {
                    auto &that = polymorphic_downcast<any_cursor_impl const &>(that_);
                    return static_cast<std::ptrdiff_t>(that.it_ - it_);
                }
            };

            struct any_sentinel_interface
            {
                virtual bool at_end(any_ref) const = 0;
            protected:
                ~any_sentinel_interface() = default;
            };

            struct any_sentinel
            {
                any_sentinel() = default;
                constexpr explicit any_sentinel(any_sentinel_interface const &view) noexcept
                  : view_{&view}
                {}

                any_sentinel_interface const &view() const noexcept
                {
                    RANGES_EXPECT(view_);
                    return *view_;
                }
            private:
                any_sentinel_interface const *view_ = nullptr;
            };

            template<typename Ref, category Cat>
            struct any_cursor
            {
            private:
                CONCEPT_ASSERT(Cat >= category::forward);

                std::unique_ptr<any_cloneable_cursor_interface<Ref, Cat>> ptr_;

                template<typename Rng>
                using impl_t = any_cursor_impl<iterator_t<Rng>, Ref, Cat>;
            public:
                any_cursor() = default;
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_cursor>()),
                    CONCEPT_REQUIRES_(ForwardRange<Rng>() &&
                                      AnyCompatibleRange<Rng, Ref>())>
                explicit any_cursor(Rng &&rng)
                  : ptr_{detail::make_unique<impl_t<Rng>>(begin(rng))}
                {}
                any_cursor(any_cursor &&) = default;
                any_cursor(any_cursor const &that)
                  : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
                {}
                any_cursor &operator=(any_cursor &&) = default;
                any_cursor &operator=(any_cursor const &that)
                {
                    ptr_ = (that.ptr_ ? that.ptr_->clone() : nullptr);
                    return *this;
                }
                Ref read() const
                {
                    RANGES_EXPECT(ptr_);
                    return ptr_->read();
                }
                bool equal(any_cursor const &that) const
                {
                    RANGES_EXPECT(!ptr_ == !that.ptr_);
                    return !ptr_ || ptr_->equal(*that.ptr_);
                }
                bool equal(any_sentinel const &that) const
                {
                    return !ptr_ || that.view().at_end(ptr_->iter());
                }
                void next()
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->next();
                }
                CONCEPT_REQUIRES(Cat >= category::bidirectional)
                void prev()
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->prev();
                }
                CONCEPT_REQUIRES(Cat >= category::random_access)
                void advance(std::ptrdiff_t n)
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->advance(n);
                }
                CONCEPT_REQUIRES(Cat >= category::random_access)
                std::ptrdiff_t distance_to(any_cursor const &that) const
                {
                    RANGES_EXPECT(!ptr_ == !that.ptr_);
                    return !ptr_ ? 0 : ptr_->distance_to(*that.ptr_);
                }
            };

            template<typename Ref, category Cat>
            struct any_view_interface
              : any_sentinel_interface
            {
                CONCEPT_ASSERT(Cat >= category::forward);

                virtual ~any_view_interface() = default;
                virtual any_cursor<Ref, Cat> begin_cursor() = 0;
            };

            template<typename Ref, category Cat>
            using any_cloneable_view_interface =
                cloneable<any_view_interface<Ref, Cat>>;

            template<typename Rng, typename Ref, category Cat>
            struct any_view_impl
              : any_cloneable_view_interface<Ref, Cat>
              , tagged_compressed_tuple<tag::range(Rng), tag::end(sentinel_t<Rng>)>
            {
            private:
                CONCEPT_ASSERT(Cat >= category::forward);
                CONCEPT_ASSERT(AnyCompatibleRange<Rng, Ref>());

                using tagged_t = tagged_compressed_tuple<
                    tag::range(Rng), tag::end(sentinel_t<Rng>)>;
                using tagged_t::range;
                using tagged_t::end;

            public:
                any_view_impl() = default;
                any_view_impl(Rng rng)
                  : tagged_t{std::move(rng), sentinel_t<Rng>{}}
                {
                    end() = ranges::end(range());
                }
                any_cursor<Ref, Cat> begin_cursor() override
                {
                    return any_cursor<Ref, Cat>{range()};
                }
                bool at_end(any_ref it_) const override
                {
                    auto &it = it_.get<iterator_t<Rng> const>();
                    return it == end();
                }
                std::unique_ptr<any_cloneable_view_interface<Ref, Cat>> clone() const override
                {
                    return detail::make_unique<any_view_impl>(range());
                }
            };
        }
        /// \endcond

        /// \brief A type-erased view
        /// \ingroup group-views
        template<typename Ref, category Cat = category::input>
        struct any_view
          : view_facade<any_view<Ref, Cat>, unknown>
        {
        private:
            friend range_access;
            CONCEPT_ASSERT(Cat >= category::forward);

            std::unique_ptr<detail::any_cloneable_view_interface<Ref, Cat>> ptr_;

            detail::any_cursor<Ref, Cat> begin_cursor()
            {
                return ptr_ ? ptr_->begin_cursor() : detail::value_init{};
            }
            detail::any_sentinel end_cursor() const noexcept
            {
                return detail::any_sentinel{*ptr_};
            }
            template<typename Rng>
            using impl_t = detail::any_view_impl<view::all_t<Rng>, Ref, Cat>;
            template<typename Rng>
            any_view(Rng &&rng, std::true_type)
              : ptr_{detail::make_unique<impl_t<Rng>>(view::all(static_cast<Rng &&>(rng)))}
            {}
            template<typename Rng>
            any_view(Rng &&, std::false_type)
            {
                static_assert(detail::to_cat_(range_concept<Rng>{}) >= Cat,
                    "The range passed to any_view() does not model the requested category");
            }
        public:
            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, any_view>>,
                    InputRange<Rng>,
                    meta::defer<detail::AnyCompatibleRange, Rng, Ref>>::value)>
            any_view(Rng &&rng)
              : any_view(static_cast<Rng &&>(rng),
                  meta::bool_<detail::to_cat_(range_concept<Rng>{}) >= Cat>{})
            {}
            any_view(any_view &&) = default;
            any_view(any_view const &that)
              : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
            {}
            any_view &operator=(any_view &&) = default;
            any_view &operator=(any_view const &that)
            {
                ptr_ = (that.ptr_ ? that.ptr_->clone() : nullptr);
                return *this;
            }
        };

        template<typename Ref>
        struct any_view<Ref, category::input>
          : view_facade<any_view<Ref, category::input>, unknown>
        {
        private:
            friend range_access;
            template<typename Rng>
            using impl_t = detail::any_input_view_impl<view::all_t<Rng>, Ref>;

            std::shared_ptr<detail::any_input_view_interface<Ref>> ptr_;

            detail::any_input_view_cursor<Ref> begin_cursor()
            {
                if (!ptr_)
                    return {};

                ptr_->init();
                return detail::any_input_view_cursor<Ref>{*ptr_};
            }
        public:
            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, any_view>>,
                    InputRange<Rng>,
                    meta::defer<detail::AnyCompatibleRange, Rng, Ref>>::value)>
            any_view(Rng &&rng)
              : ptr_{std::make_shared<impl_t<Rng>>(view::all(static_cast<Rng &&>(rng)))}
            {}
        };

        template<typename Ref>
        using any_input_view = any_view<Ref, category::input>;

        template<typename Ref>
        using any_forward_view = any_view<Ref, category::forward>;

        template<typename Ref>
        using any_bidirectional_view = any_view<Ref, category::bidirectional>;

        template<typename Ref>
        using any_random_access_view = any_view<Ref, category::random_access>;
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::any_view)

#endif
