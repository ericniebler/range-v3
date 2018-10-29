/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_COMMON_ITERATOR_HPP
#define RANGES_V3_UTILITY_COMMON_ITERATOR_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/basic_iterator.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/detail/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, typename S>
            variant<I, S> &cidata(common_iterator<I, S> &that)
            {
                return that.data_;
            }

            template<typename I, typename S>
            variant<I, S> const &cidata(common_iterator<I, S> const &that)
            {
                return that.data_;
            }
        }

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _common_iterator_ { template <typename> struct adl_hook {}; }
#endif

        template<typename I, typename S>
        struct common_iterator
#if RANGES_BROKEN_CPO_LOOKUP
          : private _common_iterator_::adl_hook<common_iterator<I, S>>
#endif
        {
        private:
            CONCEPT_ASSERT(Iterator<I>());
            CONCEPT_ASSERT(Sentinel<S, I>());
            CONCEPT_ASSERT(!Same<I, S>());
            variant<I, S> data_;

            friend variant<I, S> &detail::cidata<>(common_iterator<I, S> &);
            friend variant<I, S> const &detail::cidata<>(common_iterator<I, S> const &);
            struct emplace_fn
            {
                variant<I, S> *data_;
                template<typename T, std::size_t N>
                void operator()(indexed_element<T, N> t) const
                {
                    ranges::emplace<N>(*data_, t.get());
                }
            };
            struct arrow_proxy_
            {
            private:
                friend common_iterator;
                value_type_t<I> keep_;
                arrow_proxy_(reference_t<I>&& x)
                  : keep_(std::move(x))
                {}
            public:
                const value_type_t<I>* operator->() const noexcept
                {
                    return std::addressof(keep_);
                }
            };
            template<typename T>
            static T *operator_arrow_(T *p, int) noexcept
            {
                return p;
            }
            template<typename J, typename = detail::arrow_type_<J const>>
            static J operator_arrow_(J const &j, int) noexcept(noexcept(J(j)))
            {
                return j;
            }
            template<typename J, typename R = reference_t<J>,
                CONCEPT_REQUIRES_(std::is_reference<R>::value)>
            static meta::_t<std::add_pointer<R>> operator_arrow_(J const &j, long) noexcept
            {
                auto &&r = *j;
                return std::addressof(r);
            }
            template<typename J, typename V = value_type_t<J>,
                typename R = reference_t<J>,
                CONCEPT_REQUIRES_(Constructible<V, R>())>
            static arrow_proxy_ operator_arrow_(J const &j, ...) noexcept(noexcept(V(V(*j))))
            {
                return arrow_proxy_(*j);
            }
        public:
            using difference_type = difference_type_t<I>;

            common_iterator() = default;
            common_iterator(I i)
              : data_(emplaced_index<0>, std::move(i))
            {}
            common_iterator(S s)
              : data_(emplaced_index<1>, std::move(s))
            {}
            template<typename I2, typename S2,
                CONCEPT_REQUIRES_(ConvertibleTo<I2, I>() && ConvertibleTo<S2, S>())>
            common_iterator(common_iterator<I2, S2> const &that)
              : data_(detail::variant_core_access::make_empty<I, S>())
            {
                detail::cidata(that).visit_i(emplace_fn{&data_});
            }
            template<typename I2, typename S2,
                CONCEPT_REQUIRES_(ConvertibleTo<I2, I>() && ConvertibleTo<S2, S>())>
            common_iterator& operator=(common_iterator<I2, S2> const &that)
            {
                detail::cidata(that).visit_i(emplace_fn{&data_});
                return *this;
            }
            reference_t<I> operator*()
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                *ranges::get<0>(data_)
            )
            template<typename I2 = I, CONCEPT_REQUIRES_(Readable<I2 const>())>
            reference_t<I> operator*() const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                *static_cast<I2 const &>(ranges::get<0>(data_))
            )
            template<typename J = I, CONCEPT_REQUIRES_(Readable<J>())>
            auto operator->() const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                common_iterator::operator_arrow_((J const &) ranges::get<0>(data_), 42)
            )
            common_iterator& operator++()
            {
                ++ranges::get<0>(data_);
                return *this;
            }
#ifdef RANGES_WORKAROUND_MSVC_677925
            template<typename I2 = I, CONCEPT_REQUIRES_(!ForwardIterator<I2>())>
            auto operator++(int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ((I2 &) ranges::get<0>(data_))++
            )
#else // ^^^ workaround ^^^ / vvv no workaround vvv
            CONCEPT_REQUIRES(!ForwardIterator<I>())
            auto operator++(int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::get<0>(data_)++
            )
#endif // RANGES_WORKAROUND_MSVC_677925
            CONCEPT_REQUIRES(ForwardIterator<I>())
            common_iterator operator++(int)
            {
                return common_iterator(ranges::get<0>(data_)++);
            }

#if !RANGES_BROKEN_CPO_LOOKUP
            CONCEPT_REQUIRES(InputIterator<I>())
            friend RANGES_CXX14_CONSTEXPR
            auto iter_move(const common_iterator& i)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_move(ranges::get<0>(detail::cidata(i)))
            )
            template<typename I2, typename S2,
                CONCEPT_REQUIRES_(IndirectlySwappable<I2, I>())>
            friend auto iter_swap(
                const common_iterator& x, common_iterator<I2, S2> const &y)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_swap(
                    ranges::get<0>(detail::cidata(x)),
                    ranges::get<0>(detail::cidata(y)))
            )
#endif
        };

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _common_iterator_
        {
            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>())>
            RANGES_CXX14_CONSTEXPR
            auto iter_move(common_iterator<I, S> const &i)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_move(ranges::get<0>(detail::cidata(i)))
            )
            template<typename I1, typename S1, typename I2, typename S2,
                CONCEPT_REQUIRES_(IndirectlySwappable<I2, I1>())>
            auto iter_swap(common_iterator<I1, S1> const &x, common_iterator<I2, S2> const &y)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_swap(
                    ranges::get<0>(detail::cidata(x)),
                    ranges::get<0>(detail::cidata(y)))
            )
        }
#endif

        template<typename I1, typename I2, typename S1, typename S2,
            CONCEPT_REQUIRES_(Sentinel<S1, I2>() && Sentinel<S2, I1>() &&
                !EqualityComparable<I1, I2>())>
        bool operator==(common_iterator<I1, S1> const &x, common_iterator<I2, S2> const &y)
        {
            return detail::cidata(x).index() == 1u ?
                (detail::cidata(y).index() == 1u || ranges::get<0>(detail::cidata(y)) == ranges::get<1>(detail::cidata(x))) :
                (detail::cidata(y).index() != 1u || ranges::get<0>(detail::cidata(x)) == ranges::get<1>(detail::cidata(y)));
        }

        template<typename I1, typename I2, typename S1, typename S2,
            CONCEPT_REQUIRES_(Sentinel<S1, I2>() && Sentinel<S2, I1>() &&
                EqualityComparable<I1, I2>())>
        bool operator==(common_iterator<I1, S1> const &x, common_iterator<I2, S2> const &y)
        {
            return detail::cidata(x).index() == 1u ?
                (detail::cidata(y).index() == 1u || ranges::get<0>(detail::cidata(y)) == ranges::get<1>(detail::cidata(x))) :
                (detail::cidata(y).index() == 1u ?
                    ranges::get<0>(detail::cidata(x)) == ranges::get<1>(detail::cidata(y)) :
                    ranges::get<0>(detail::cidata(x)) == ranges::get<0>(detail::cidata(y)));
        }

        template<typename I1, typename I2, typename S1, typename S2,
            CONCEPT_REQUIRES_(Sentinel<S1, I2>() && Sentinel<S2, I1>())>
        bool operator!=(common_iterator<I1, S1> const &x, common_iterator<I2, S2> const &y)
        {
            return !(x == y);
        }

        template<typename I1, typename I2, typename S1, typename S2,
            CONCEPT_REQUIRES_(SizedSentinel<I1, I2>() && SizedSentinel<S1, I2>() &&
                SizedSentinel<S2, I1>())>
        difference_type_t<I2> operator-(
            common_iterator<I1, S1> const &x, common_iterator<I2, S2> const &y)
        {
            return detail::cidata(x).index() == 1u ?
                (detail::cidata(y).index() == 1u ? 0 : ranges::get<1>(detail::cidata(x)) - ranges::get<0>(detail::cidata(y))) :
                (detail::cidata(y).index() == 1u ?
                    ranges::get<0>(detail::cidata(x)) - ranges::get<1>(detail::cidata(y)) :
                    ranges::get<0>(detail::cidata(x)) - ranges::get<0>(detail::cidata(y)));
        }

        template<typename I, typename S>
        struct value_type<common_iterator<I, S>>
          : meta::if_<
                Readable<I>,
                meta::defer<value_type_t, I>,
                meta::nil_>
        {};

        template<typename I, typename S>
        struct iterator_category<common_iterator<I, S>>
          : meta::if_<
                InputIterator<I>,
                meta::if_<
                    ForwardIterator<I>,
                    meta::id<forward_iterator_tag>,
                    meta::id<input_iterator_tag>>,
                meta::nil_>
        {};

        /// \cond
        namespace detail
        {
            template<typename I, bool = (bool) InputIterator<I>()>
            struct common_iterator_std_traits
            {
                using iterator_category =
                    meta::if_c<
                        (bool) ForwardIterator<I>() &&
                            std::is_reference<reference_t<I>>::value,
                        std::forward_iterator_tag,
                        std::input_iterator_tag>;
                using difference_type = difference_type_t<I>;
                using value_type = value_type_t<I>;
                using reference = reference_t<I>;
                using pointer = meta::_t<detail::pointer_type_<I>>;
            };

            template<typename I>
            struct common_iterator_std_traits<I, false>
            {
                using iterator_category = std::output_iterator_tag;
                using difference_type = difference_type_t<I>;
                using value_type = void;
                using reference = void;
                using pointer = void;
            };
        }
        /// \endcond
    }
}

/// \cond
namespace std
{
    template<typename I, typename S>
    struct iterator_traits< ::ranges::common_iterator<I, S>>
      : ::ranges::detail::common_iterator_std_traits<I>
    {};
}
/// \endcond

#endif
