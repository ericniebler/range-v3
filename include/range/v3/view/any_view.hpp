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

#include <memory>
#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
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
            struct any_object
            {
                virtual ~any_object() = default;
            };

            template<class T>
            struct object : any_object
            {
            private:
                T obj;
            public:
                object() = default;
                object(T o) : obj(std::move(o)) {}
                T &get() { return obj; }
                T const &get() const { return obj; }
            };

            template<typename Ref, category Cat = category::input>
            struct any_cursor_interface
            {
                virtual ~any_cursor_interface() = default;
                virtual any_object const &iter() const = 0;
                virtual Ref get() const = 0;
                virtual bool equal(any_cursor_interface const &) const = 0;
                virtual void next() = 0;
                virtual any_cursor_interface *clone_() const = 0;
                any_cursor_interface *clone() const
                {
                    return this->clone_();
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::forward>
              : any_cursor_interface<Ref, category::input>
            {
                any_cursor_interface *clone() const
                {
                    return static_cast<any_cursor_interface *>(this->clone_());
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::bidirectional>
              : any_cursor_interface<Ref, category::forward>
            {
                virtual void prev() = 0;
                any_cursor_interface *clone() const
                {
                    return static_cast<any_cursor_interface *>(this->clone_());
                }
            };

            template<typename Ref>
            struct any_cursor_interface<Ref, category::random_access>
              : any_cursor_interface<Ref, category::bidirectional>
            {
                virtual void advance(std::ptrdiff_t) = 0;
                virtual std::ptrdiff_t distance_to(any_cursor_interface const &) const = 0;
                any_cursor_interface *clone() const
                {
                    return static_cast<any_cursor_interface *>(this->clone_());
                }
            };

            template<typename S, typename I>
            struct any_sentinel_impl;

            template<typename I, category Cat>
            struct any_cursor_impl
              : any_cursor_interface<iterator_reference_t<I>, Cat>
            {
            private:
                template<typename S, typename II>
                friend struct any_sentinel_impl;
                using Ref = iterator_reference_t<I>;
                using Input = any_cursor_interface<Ref, category::input>;
                object<I> it_;

                CONCEPT_REQUIRES(EqualityComparable<I>())
                bool equal_(Input const &that) const
                {
                    any_cursor_impl const *pthat =
                        dynamic_cast<any_cursor_impl const *>(&that);
                    RANGES_ASSERT(pthat != nullptr);
                    return pthat->it_.get() == it_.get();
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
                object<I> const &iter() const
                {
                    return it_;
                }
                Ref get() const
                {
                    return *it_.get();
                }
                bool equal(Input const &that) const
                {
                    return equal_(that);
                }
                void next()
                {
                    ++it_.get();
                }
                any_cursor_impl *clone_() const
                {
                    return new any_cursor_impl{it_.get()};
                }
                void prev()
                {
                    --it_.get();
                }
                void advance(std::ptrdiff_t n)
                {
                    it_.get() += n;
                }
                std::ptrdiff_t distance_to(
                    any_cursor_interface<Ref, category::random_access> const &that) const
                {
                    any_cursor_impl const *pthat =
                        dynamic_cast<any_cursor_impl const *>(&that);
                    RANGES_ASSERT(pthat != nullptr);
                    return pthat->it_.get() - it_.get();
                }
            };

            struct any_sentinel_interface
            {
                virtual ~any_sentinel_interface() = default;
                virtual bool equal(any_object const &) const = 0;
                virtual any_sentinel_interface *clone() const = 0;
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
                bool equal(any_object const &that) const
                {
                    object<I> const *pthat = dynamic_cast<object<I> const *>(&that);
                    RANGES_ASSERT(pthat != nullptr);
                    return s_ == pthat->get();
                }
                any_sentinel_impl *clone() const
                {
                    return new any_sentinel_impl{s_};
                }
            };

            struct any_sentinel;

            template<typename Ref, category Cat>
            struct any_cursor
            {
            private:
                friend struct any_sentinel;
                std::unique_ptr<any_cursor_interface<Ref, Cat>> ptr_;
            public:
                using single_pass = meta::bool_<Cat == category::input>;
                any_cursor() = default;
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_cursor>()),
                    CONCEPT_REQUIRES_(InputRange<Rng>() &&
                                      ConvertibleTo<range_reference_t<Rng>, Ref>())>
                any_cursor(Rng &&rng, begin_tag)
                  : ptr_{new any_cursor_impl<range_iterator_t<Rng>, Cat>{begin(rng)}}
                {}
                any_cursor(any_cursor &&) = default;
                any_cursor(any_cursor const &that)
                  : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
                {}
                any_cursor &operator=(any_cursor &&) = default;
                any_cursor &operator=(any_cursor const &that)
                {
                    ptr_.reset(that.ptr_ ? that.ptr_->clone() : nullptr);
                    return *this;
                }
                Ref get() const
                {
                    RANGES_ASSERT(ptr_);
                    return ptr_->get();
                }
                bool equal(any_cursor const &that) const
                {
                    RANGES_ASSERT(!ptr_ == !that.ptr_);
                    return (!ptr_ && !that.ptr_) || ptr_->equal(*that.ptr_);
                }
                void next()
                {
                    RANGES_ASSERT(ptr_);
                    ptr_->next();
                }
                CONCEPT_REQUIRES(Cat >= category::bidirectional)
                void prev()
                {
                    RANGES_ASSERT(ptr_);
                    ptr_->prev();
                }
                CONCEPT_REQUIRES(Cat >= category::random_access)
                void advance(std::ptrdiff_t n)
                {
                    RANGES_ASSERT(ptr_);
                    ptr_->advance(n);
                }
                CONCEPT_REQUIRES(Cat >= category::random_access)
                std::ptrdiff_t distance_to(any_cursor const &that) const
                {
                    RANGES_ASSERT(ptr_ && that.ptr_);
                    return ptr_->distance_to(*that.ptr_);
                }
            };

            struct any_sentinel
            {
            private:
                std::unique_ptr<any_sentinel_interface> ptr_;
            public:
                any_sentinel() = default;
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_sentinel>()),
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                any_sentinel(Rng &&rng, end_tag)
                  : ptr_{new any_sentinel_impl<range_sentinel_t<Rng>, range_iterator_t<Rng>>{
                        end(rng)}}
                {}
                any_sentinel(any_sentinel &&) = default;
                any_sentinel(any_sentinel const &that)
                  : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
                {}
                any_sentinel &operator=(any_sentinel &&) = default;
                any_sentinel &operator=(any_sentinel const &that)
                {
                    ptr_.reset(that.ptr_ ? that.ptr_->clone() : nullptr);
                    return *this;
                }
                template<typename Ref, category Cat>
                bool equal(any_cursor<Ref, Cat> const &that) const
                {
                    RANGES_ASSERT(!ptr_ == !that.ptr_);
                    return (!ptr_ && !that.ptr_) || ptr_->equal(that.ptr_->iter());
                }
            };

            template<typename Ref, category Cat>
            struct any_view_interface
            {
                virtual ~any_view_interface() = default;
                virtual any_cursor<Ref, Cat> begin_cursor() = 0;
                virtual any_sentinel end_cursor() = 0;
                virtual any_view_interface *clone() const = 0;
            };

            template<typename Rng, category Cat>
            struct any_view_impl
              : any_view_interface<range_reference_t<Rng>, Cat>
            {
            private:
                using Ref = range_reference_t<Rng>;
                Rng rng_;
            public:
                any_view_impl() = default;
                any_view_impl(Rng rng)
                  : rng_(std::move(rng))
                {}
                any_cursor<Ref, Cat> begin_cursor()
                {
                    return {rng_, begin_tag{}};
                }
                any_sentinel end_cursor()
                {
                    return {rng_, end_tag{}};
                }
                any_view_interface<Ref, Cat> *clone() const
                {
                    return new any_view_impl<Rng, Cat>{rng_};
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
            any_view(Rng && rng, std::true_type)
              : ptr_{new detail::any_view_impl<view::all_t<Rng>, Cat>{
                    view::all(std::forward<Rng>(rng))}}
            {}
            template<typename Rng>
            any_view(Rng &&, std::false_type)
              : ptr_{}
            {
                static_assert(detail::to_cat_(range_concept<Rng>{}) >= Cat,
                    "The range passed to any_view() does not model the requested category");
            }
        public:
            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(!Same<detail::decay_t<Rng>, any_view>()),
                CONCEPT_REQUIRES_(InputRange<Rng>() &&
                                  ConvertibleTo<range_reference_t<Rng>, Ref>())>
            any_view(Rng && rng)
              : any_view(std::forward<Rng>(rng),
                  meta::bool_<detail::to_cat_(range_concept<Rng>{}) >= Cat>{})
            {}
            any_view(any_view &&) = default;
            any_view(any_view const &that)
              : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
            {}
            any_view &operator=(any_view &&) = default;
            any_view &operator=(any_view const &that)
            {
                ptr_.reset(that.ptr_ ? that.ptr_->clone() : nullptr);
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
    }
}

#endif
