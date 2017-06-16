/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
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
#include <range/v3/view/all.hpp>
#include <range/v3/utility/memory.hpp>

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

            template<typename Ref, category Cat = category::input>
            struct any_cursor_interface
            {
                virtual ~any_cursor_interface() = default;
                virtual any_ref iter() const = 0;
                virtual Ref read() const = 0;
                virtual bool equal(any_cursor_interface const &) const = 0;
                virtual void next() = 0;
                virtual std::unique_ptr<any_cursor_interface> clone_() const = 0;
                std::unique_ptr<any_cursor_interface> clone() const
                {
                    return clone_();
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::forward>
              : any_cursor_interface<Ref, category::input>
            {
                std::unique_ptr<any_cursor_interface> clone() const
                {
                    return std::unique_ptr<any_cursor_interface>{
                        polymorphic_downcast<any_cursor_interface *>(
                            this->clone_().release())};
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::bidirectional>
              : any_cursor_interface<Ref, category::forward>
            {
                virtual void prev() = 0;
                std::unique_ptr<any_cursor_interface> clone() const
                {
                    return std::unique_ptr<any_cursor_interface>{
                        polymorphic_downcast<any_cursor_interface *>(
                            this->clone_().release())};
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::random_access>
              : any_cursor_interface<Ref, category::bidirectional>
            {
                virtual void advance(std::ptrdiff_t) = 0;
                virtual std::ptrdiff_t distance_to(any_cursor_interface const &) const = 0;
                std::unique_ptr<any_cursor_interface> clone() const
                {
                    return std::unique_ptr<any_cursor_interface>{
                        polymorphic_downcast<any_cursor_interface *>(
                            this->clone_().release())};
                }
            };

            template<typename S, typename I>
            struct any_sentinel_impl;

            template<typename I, typename Ref, category Cat>
            struct any_cursor_impl
              : any_cursor_interface<Ref, Cat>
            {
            private:
                template<typename, typename>
                friend struct any_sentinel_impl;
                CONCEPT_ASSERT(ConvertibleTo<reference_t<I>, Ref>());
                using Input = any_cursor_interface<Ref, category::input>;

                I it_;

                CONCEPT_REQUIRES(EqualityComparable<I>())
                bool equal_(Input const &that_) const
                {
                    auto &that = polymorphic_downcast<any_cursor_impl const &>(that_);
                    return that.it_ == it_;
                }
                CONCEPT_REQUIRES(!EqualityComparable<I>())
                bool equal_(Input const &) const
                {
                    return true;
                }

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
                bool equal(Input const &that) const // override
                {
                    return equal_(that);
                }
                void next() // override
                {
                    ++it_;
                }
                std::unique_ptr<Input> clone_() const // override
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
                virtual ~any_sentinel_interface() = default;
                virtual bool equal(any_ref) const = 0;
                virtual std::unique_ptr<any_sentinel_interface> clone() const = 0;
            };

            template<typename S, typename I>
            struct any_sentinel_impl
              : any_sentinel_interface
            {
            private:
                S s_;
            public:
                any_sentinel_impl() = default;
                any_sentinel_impl(S s)
                  : s_(std::move(s))
                {}
                bool equal(any_ref that_) const override
                {
                    return that_.get<I const>() == s_;
                }
                std::unique_ptr<any_sentinel_interface> clone() const override
                {
                    return detail::make_unique<any_sentinel_impl>(s_);
                }
            };

            template<typename Ref, category Cat>
            struct any_cursor;

            struct any_sentinel
            {
            private:
                template<typename, category>
                friend struct any_cursor;
                std::unique_ptr<any_sentinel_interface> ptr_;
                template<class Rng>
                using impl_t = any_sentinel_impl<sentinel_t<Rng>, iterator_t<Rng>>;
            public:
                any_sentinel() = default;
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_sentinel>()),
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                any_sentinel(Rng &&rng, end_tag)
                  : ptr_{detail::make_unique<impl_t<Rng>>(end(rng))}
                {}
                any_sentinel(any_sentinel &&) = default;
                any_sentinel(any_sentinel const &that)
                  : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
                {}
                any_sentinel &operator=(any_sentinel &&) = default;
                any_sentinel &operator=(any_sentinel const &that)
                {
                    ptr_ = (that.ptr_ ? that.ptr_->clone() : nullptr);
                    return *this;
                }
            };

            template<typename Ref, category Cat>
            struct any_cursor
            {
            private:
                friend any_sentinel;
                std::unique_ptr<any_cursor_interface<Ref, Cat>> ptr_;
                template<typename Rng>
                using impl_t = any_cursor_impl<iterator_t<Rng>, Ref, Cat>;
            public:
                using single_pass = meta::bool_<Cat == category::input>;
                any_cursor() = default;
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_cursor>()),
                    CONCEPT_REQUIRES_(InputRange<Rng>() &&
                                      ConvertibleTo<range_reference_t<Rng>, Ref>())>
                any_cursor(Rng &&rng, begin_tag)
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
                    return (!ptr_ && !that.ptr_) || ptr_->equal(*that.ptr_);
                }
                bool equal(any_sentinel const &that) const
                {
                    RANGES_EXPECT(!ptr_ == !that.ptr_);
                    return (!ptr_ && !that.ptr_) || that.ptr_->equal(ptr_->iter());
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
            {
                virtual ~any_view_interface() = default;
                virtual any_cursor<Ref, Cat> begin_cursor() = 0;
                virtual any_sentinel end_cursor() = 0;
                virtual std::unique_ptr<any_view_interface> clone() const = 0;
            };

            template<typename Interface, typename Rng, typename Ref, category Cat>
            struct any_view_impl
              : Interface
            {
            private:
                CONCEPT_ASSERT(ConvertibleTo<range_reference_t<Rng>, Ref>());
                Rng rng_;
            public:
                any_view_impl() = default;
                any_view_impl(Rng rng)
                  : rng_(std::move(rng))
                {}
                any_cursor<Ref, Cat> begin_cursor() // override
                {
                    return {rng_, begin_tag{}};
                }
                any_sentinel end_cursor() // override
                {
                    return {rng_, end_tag{}};
                }
                std::unique_ptr<Interface> clone() const // sometimes override
                {
                    return detail::make_unique<any_view_impl>(rng_);
                }
            };

            constexpr category to_cat_(concepts::InputRange *) { return category::input; }
            constexpr category to_cat_(concepts::ForwardRange *) { return category::forward; }
            constexpr category to_cat_(concepts::BidirectionalRange *) { return category::bidirectional; }
            constexpr category to_cat_(concepts::RandomAccessRange *) { return category::random_access; }
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

            std::unique_ptr<detail::any_view_interface<Ref, Cat>> ptr_;

            detail::any_cursor<Ref, Cat> begin_cursor()
            {
                return ptr_ ? ptr_->begin_cursor() : detail::value_init{};
            }
            detail::any_sentinel end_cursor()
            {
                return ptr_ ? ptr_->end_cursor() : detail::value_init{};
            }
            template<typename Rng>
            using impl_t = detail::any_view_impl<
                detail::any_view_interface<Ref, Cat>, view::all_t<Rng>, Ref, Cat>;
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
            template<typename Rng>
            using CompatibleRange = ConvertibleTo<range_reference_t<Rng>, Ref>;
        public:
            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, any_view>>,
                    InputRange<Rng>,
                    meta::defer<CompatibleRange, Rng>>::value)>
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
        using any_input_view = any_view<Ref, category::input>;

        template<typename Ref>
        using any_forward_view = any_view<Ref, category::forward>;

        template<typename Ref>
        using any_bidirectional_view = any_view<Ref, category::bidirectional>;

        template<typename Ref>
        using any_random_access_view = any_view<Ref, category::random_access>;

        /// \cond
        namespace detail
        {
            template<typename Ref>
            struct unique_any_view_interface
            {
                virtual ~unique_any_view_interface() = default;
                virtual any_cursor<Ref, category::input> begin_cursor() = 0;
                virtual any_sentinel end_cursor() = 0;
            };
        } // namespace detail
        /// \endcond

        template<typename Ref>
        struct unique_any_view
          : view_facade<unique_any_view<Ref>, unknown>
        {
        private:
            friend range_access;

            std::unique_ptr<detail::unique_any_view_interface<Ref>> ptr_;

            detail::any_cursor<Ref, category::input> begin_cursor()
            {
                return ptr_ ? ptr_->begin_cursor() : detail::value_init{};
            }
            detail::any_sentinel end_cursor()
            {
                return ptr_ ? ptr_->end_cursor() : detail::value_init{};
            }
            template<typename Rng>
            using impl_t = detail::any_view_impl<
                detail::unique_any_view_interface<Ref>, view::all_t<Rng>, Ref, category::input>;
            template<typename Rng>
            using CompatibleRange = ConvertibleTo<range_reference_t<Rng>, Ref>;
        public:
            unique_any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, unique_any_view>>,
                    InputRange<Rng>,
                    meta::defer<CompatibleRange, Rng>>::value)>
            unique_any_view(Rng &&rng)
              : ptr_{detail::make_unique<impl_t<Rng>>(view::all(static_cast<Rng &&>(rng)))}
            {}
        };

        template<typename Ref>
        using unique_any_input_view = unique_any_view<Ref>;
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::any_view)

#endif
