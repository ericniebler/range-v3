/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE

namespace ranges
{
    inline namespace v3
    {
        /// \brief An enum that denotes the supported subset of range concepts supported by a range.
        enum class category
        {
            none            =  0,  ///<\brief No concepts met.
            input           =  1,  ///<\brief Satisfies ranges::v3::concepts::InputRange
            forward         =  3,  ///<\brief Satisfies ranges::v3::concepts::ForwardRange
            bidirectional   =  7,  ///<\brief Satisfies ranges::v3::concepts::BidirectionalRange
            random_access   = 15,  ///<\brief Satisfies ranges::v3::concepts::RandomAccessRange
            mask            = random_access, ///<\brief Mask away any properties other than iterator category
            sized           = 16,  ///<\brief Satisfies ranges::v3::concepts::SizedRange
        };

        /** \name Binary operators for ranges::v3::category
         *  \relates ranges::v3::category
         *  \{
         */
        constexpr category operator&(category lhs, category rhs) noexcept
        {
            return static_cast<category>(
                static_cast<meta::_t<std::underlying_type<category>>>(lhs) &
                static_cast<meta::_t<std::underlying_type<category>>>(rhs));
        }

        constexpr category operator|(category lhs, category rhs) noexcept
        {
            return static_cast<category>(
                static_cast<meta::_t<std::underlying_type<category>>>(lhs) |
                static_cast<meta::_t<std::underlying_type<category>>>(rhs));
        }

        constexpr category operator^(category lhs, category rhs) noexcept
        {
            return static_cast<category>(
                static_cast<meta::_t<std::underlying_type<category>>>(lhs) ^
                static_cast<meta::_t<std::underlying_type<category>>>(rhs));
        }

        constexpr category operator~(category lhs) noexcept
        {
            return static_cast<category>(
                ~static_cast<meta::_t<std::underlying_type<category>>>(lhs));
        }

        RANGES_CXX14_CONSTEXPR category &operator&=(category &lhs, category rhs) noexcept
        {
            return (lhs = lhs & rhs);
        }

        RANGES_CXX14_CONSTEXPR category &operator|=(category &lhs, category rhs) noexcept
        {
            return (lhs = lhs | rhs);
        }

        RANGES_CXX14_CONSTEXPR category &operator^=(category &lhs, category rhs) noexcept
        {
            return (lhs = lhs ^ rhs);
        }
        //!\}

        /// \brief For a given range, return a ranges::v3::category enum with the satisfied concepts.
        template<typename Rng>
        constexpr category get_categories() noexcept
        {
            return (InputRange<Rng>()         ? category::input         : category::none) |
                   (ForwardRange<Rng>()       ? category::forward       : category::none) |
                   (BidirectionalRange<Rng>() ? category::bidirectional : category::none) |
                   (RandomAccessRange<Rng>()  ? category::random_access : category::none) |
                   (SizedRange<Rng>()         ? category::sized         : category::none);
        }

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

            template<typename Rng, typename Ref>
            using AnyCompatibleRange = ConvertibleTo<range_reference_t<Rng>, Ref>;

            template<typename Rng, typename = void>
            struct any_view_sentinel_impl
              : private box<sentinel_t<Rng>, any_view_sentinel_impl<Rng>>
            {
            private:
                using box_t = typename any_view_sentinel_impl::box;
            public:
                any_view_sentinel_impl() = default;
                any_view_sentinel_impl(Rng &rng)
                    noexcept(noexcept(box_t(ranges::end(rng))))
                  : box_t(ranges::end(rng))
                {}
                void init(Rng &rng) noexcept
                {
                    box_t::get() = ranges::end(rng);
                }
                sentinel_t<Rng> const &get(Rng const &) const noexcept
                {
                    return box_t::get();
                }
            };

            template<typename Rng>
            struct any_view_sentinel_impl<Rng, meta::void_<
                decltype(ranges::end(std::declval<Rng const &>()))>>
            {
                any_view_sentinel_impl() = default;
                any_view_sentinel_impl(Rng &) noexcept
                {}
                void init(Rng &) noexcept
                {}
                sentinel_t<Rng> get(Rng const &rng) const noexcept
                {
                    return ranges::end(rng);
                }
            };

            template<typename Ref, bool Sized = false>
            struct any_input_view_interface
            {
                virtual ~any_input_view_interface() = default;
                virtual void init() = 0;
                virtual bool done() const = 0;
                virtual Ref read() const = 0;
                virtual void next() = 0;
            };
            template<typename Ref>
            struct any_input_view_interface<Ref, true>
              : any_input_view_interface<Ref, false>
            {
                virtual std::size_t size() const = 0;
            };

            template<typename Ref>
            struct any_input_cursor
            {
                using single_pass = std::true_type;

                any_input_cursor() = default;
                constexpr any_input_cursor(any_input_view_interface<Ref> &view) noexcept
                  : view_{std::addressof(view)}
                {}
                Ref read() const { return view_->read(); }
                void next() { view_->next(); }
                bool equal(any_input_cursor const &) const noexcept
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

            template<typename Rng, typename Ref, bool Sized = false>
            struct any_input_view_impl
              : any_input_view_interface<Ref, Sized>
              , private tagged_compressed_tuple<tag::range(Rng),
                    tag::current(iterator_t<Rng>)>
              , private any_view_sentinel_impl<Rng>
            {
                CONCEPT_ASSERT(AnyCompatibleRange<Rng, Ref>());
                CONCEPT_ASSERT(!Sized || SizedRange<Rng>());

                explicit any_input_view_impl(Rng rng_)
                  : tagged_t{std::move(rng_), iterator_t<Rng>{}}
                {}
                any_input_view_impl(any_input_view_impl const &) = delete;
                any_input_view_impl &operator=(any_input_view_impl const &) = delete;

            private:
                using tagged_t = tagged_compressed_tuple<tag::range(Rng),
                    tag::current(iterator_t<Rng>)>;
                using tagged_t::range;
                using tagged_t::current;
                using sentinel_box_t = any_view_sentinel_impl<Rng>;

                virtual void init() override
                {
                    auto &rng = range();
                    sentinel_box_t::init(rng);
                    current() = ranges::begin(rng);
                }
                virtual bool done() const override
                {
                    return current() == sentinel_box_t::get(range());
                }
                virtual Ref read() const override { return *current(); }
                virtual void next() override { ++current(); }
                std::size_t size() const // override-ish
                {
                    return static_cast<std::size_t>(ranges::size(range()));
                }
            };

            template<typename Ref, category Cat = category::forward, typename enable = void>
            struct any_cursor_interface;

            template<typename Ref, category Cat>
            struct any_cursor_interface<Ref, Cat, meta::if_c<(Cat & category::mask) == category::forward>>
            {
                virtual ~any_cursor_interface() = default;
                virtual any_ref iter() const = 0; // returns a const ref to the cursor's wrapped iterator
                virtual Ref read() const = 0;
                virtual bool equal(any_cursor_interface const &) const = 0;
                virtual void next() = 0;
            };


            template<typename Ref, category Cat>
            struct any_cursor_interface<Ref, Cat, meta::if_c<(Cat & category::mask) == category::bidirectional>>
              : any_cursor_interface<Ref, (Cat & ~category::mask) | category::forward>
            {
                virtual void prev() = 0;
            };

            template<typename Ref, category Cat>
            struct any_cursor_interface<Ref, Cat, meta::if_c<(Cat & category::mask) == category::random_access>>
              : any_cursor_interface<Ref, (Cat & ~category::mask) | category::bidirectional>
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
                CONCEPT_ASSERT(ConvertibleTo<reference_t<I>, Ref>());
                CONCEPT_ASSERT((Cat & category::forward) == category::forward);

                any_cursor_impl() = default;
                any_cursor_impl(I it)
                  : it_{std::move(it)}
                {}
            private:
                using Forward = any_cursor_interface<Ref, (Cat & ~category::mask) | category::forward>;

                I it_;

                any_ref iter() const override
                {
                    return it_;
                }
                Ref read() const override
                {
                    return *it_;
                }
                bool equal(Forward const &that_) const override
                {
                    auto &that = polymorphic_downcast<any_cursor_impl const &>(that_);
                    return that.it_ == it_;
                }
                void next() override
                {
                    ++it_;
                }
                std::unique_ptr<any_cloneable_cursor_interface<Ref, Cat>> clone() const override
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

            struct fully_erased_view
            {
                virtual bool at_end(any_ref) const = 0; // any_ref is a const ref to a wrapped iterator
                                                        // to be compared to the erased view's end sentinel
            protected:
                ~fully_erased_view() = default;
            };

            struct any_sentinel
            {
                any_sentinel() = default;
                constexpr explicit any_sentinel(fully_erased_view const &view) noexcept
                  : view_{&view}
                {}
            private:
                template<typename, category> friend struct any_cursor;

                fully_erased_view const *view_ = nullptr;
            };

            template<typename Ref, category Cat>
            struct any_cursor
            {
            private:
                CONCEPT_ASSERT((Cat & category::forward) == category::forward);

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
                    RANGES_EXPECT(!ptr_ == !that.view_);
                    return !ptr_ || that.view_->at_end(ptr_->iter());
                }
                void next()
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->next();
                }
                CONCEPT_REQUIRES((Cat & category::bidirectional) == category::bidirectional)
                void prev()
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->prev();
                }
                CONCEPT_REQUIRES((Cat & category::random_access) == category::random_access)
                void advance(std::ptrdiff_t n)
                {
                    RANGES_EXPECT(ptr_);
                    ptr_->advance(n);
                }
                CONCEPT_REQUIRES((Cat & category::random_access) == category::random_access)
                std::ptrdiff_t distance_to(any_cursor const &that) const
                {
                    RANGES_EXPECT(!ptr_ == !that.ptr_);
                    return !ptr_ ? 0 : ptr_->distance_to(*that.ptr_);
                }
            };

            template<typename Ref, category Cat, bool = (Cat & category::sized) == category::sized>
            struct any_view_interface
              : fully_erased_view
            {
                CONCEPT_ASSERT((Cat & category::forward) == category::forward);

                virtual ~any_view_interface() = default;
                virtual any_cursor<Ref, Cat> begin_cursor() = 0;
            };
            template<typename Ref, category Cat>
            struct any_view_interface<Ref, Cat, true>
              : any_view_interface<Ref, Cat, false>
            {
                virtual std::size_t size() const = 0;
            };

            template<typename Ref, category Cat>
            using any_cloneable_view_interface =
                cloneable<any_view_interface<Ref, Cat>>;

            template<typename Rng, typename Ref, category Cat>
            struct any_view_impl
              : any_cloneable_view_interface<Ref, Cat>
              , private box<Rng, any_view_impl<Rng, Ref, Cat>>
              , private any_view_sentinel_impl<Rng>
            {
                CONCEPT_ASSERT((Cat & category::forward) == category::forward);
                CONCEPT_ASSERT(AnyCompatibleRange<Rng, Ref>());
                CONCEPT_ASSERT((Cat & category::sized) == category::none || SizedRange<Rng>());

                any_view_impl() = default;
                any_view_impl(Rng rng)
                  : range_box_t{std::move(rng)}
                  , sentinel_box_t{range_box_t::get()} // NB: initialization order dependence
                {}

            private:
                using range_box_t = box<Rng, any_view_impl>;
                using sentinel_box_t = any_view_sentinel_impl<Rng>;

                any_cursor<Ref, Cat> begin_cursor() override
                {
                    return any_cursor<Ref, Cat>{range_box_t::get()};
                }
                bool at_end(any_ref it_) const override
                {
                    auto &it = it_.get<iterator_t<Rng> const>();
                    return it == sentinel_box_t::get(range_box_t::get());
                }
                std::unique_ptr<any_cloneable_view_interface<Ref, Cat>> clone() const override
                {
                    return detail::make_unique<any_view_impl>(range_box_t::get());
                }
                std::size_t size() const // override-ish
                {
                    return static_cast<std::size_t>(ranges::size(range_box_t::get()));
                }
            };
        }
        /// \endcond

        /// \brief A type-erased view
        /// \ingroup group-views
        template<typename Ref, category Cat = category::input, typename enable = void>
        struct any_view
          : view_facade<any_view<Ref, Cat>, (Cat & category::sized) == category::sized ? finite : unknown>
        {
            friend range_access;
            CONCEPT_ASSERT((Cat & category::forward) == category::forward);

            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, any_view>>,
                    InputRange<Rng>,
                    meta::defer<detail::AnyCompatibleRange, Rng, Ref>>::value)>
            any_view(Rng &&rng)
              : any_view(static_cast<Rng &&>(rng),
                  meta::bool_<(get_categories<Rng>() & Cat) == Cat>{})
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

            CONCEPT_REQUIRES((Cat & category::sized) == category::sized)
            std::size_t size() const
            {
                return ptr_ ? ptr_->size() : 0;
            }
        private:
            template<typename Rng>
            using impl_t = detail::any_view_impl<view::all_t<Rng>, Ref, Cat>;
            template<typename Rng>
            any_view(Rng &&rng, std::true_type)
              : ptr_{detail::make_unique<impl_t<Rng>>(view::all(static_cast<Rng &&>(rng)))}
            {}
            template<typename Rng>
            any_view(Rng &&, std::false_type)
            {
                static_assert((get_categories<Rng>() & Cat) == Cat,
                    "The range passed to any_view() does not model the requested category");
            }

            detail::any_cursor<Ref, Cat> begin_cursor()
            {
                return ptr_ ? ptr_->begin_cursor() : detail::value_init{};
            }
            detail::any_sentinel end_cursor() const noexcept
            {
                return detail::any_sentinel{*ptr_};
            }

            std::unique_ptr<detail::any_cloneable_view_interface<Ref, Cat>> ptr_;
        };

        // input and not forward
        template<typename Ref, category Cat>
        struct any_view<Ref, Cat, meta::if_c<(Cat & category::forward) == category::input>>
          : view_facade<any_view<Ref, Cat, void>, (Cat & category::sized) == category::sized ? finite : unknown>
        {
            friend range_access;

            any_view() = default;
            template<typename Rng,
                CONCEPT_REQUIRES_(meta::and_<
                    meta::not_<Same<detail::decay_t<Rng>, any_view>>,
                    InputRange<Rng>,
                    meta::defer<detail::AnyCompatibleRange, Rng, Ref>>::value)>
            any_view(Rng &&rng)
              : ptr_{std::make_shared<impl_t<Rng>>(view::all(static_cast<Rng &&>(rng)))}
            {}

            CONCEPT_REQUIRES((Cat & category::sized) == category::sized)
            std::size_t size() const
            {
                return ptr_ ? ptr_->size() : 0;
            }
        private:
            template<typename Rng>
            using impl_t = detail::any_input_view_impl<view::all_t<Rng>, Ref,
                (Cat & category::sized) == category::sized>;

            detail::any_input_cursor<Ref> begin_cursor()
            {
                if (!ptr_)
                    return {};

                ptr_->init();
                return detail::any_input_cursor<Ref>{*ptr_};
            }

            std::shared_ptr<detail::any_input_view_interface<Ref,
                (Cat & category::sized) == category::sized>>
            ptr_;
        };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        template<typename Rng,
            CONCEPT_REQUIRES_(View<Rng>()),
            typename Ref = range_reference_t<Rng>,
            category Cat = get_categories<Rng>()>
        any_view(Rng &&) -> any_view<Ref, Cat>;
#endif

        template<typename Ref>
        using any_input_view RANGES_DEPRECATED("Use any_view<Ref, category::input> instead.")
            = any_view<Ref, category::input>;

        template<typename Ref>
        using any_forward_view RANGES_DEPRECATED("Use any_view<Ref, category::forward> instead.")
            = any_view<Ref, category::forward>;

        template<typename Ref>
        using any_bidirectional_view RANGES_DEPRECATED("Use any_view<Ref, category::bidirectional> instead.")
            = any_view<Ref, category::bidirectional>;

        template<typename Ref>
        using any_random_access_view RANGES_DEPRECATED("Use any_view<Ref, category::random_access> instead.")
            = any_view<Ref, category::random_access>;
    } // inline namespace v3
} // namepace ranges

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::any_view)

RANGES_DIAGNOSTIC_POP

#endif
