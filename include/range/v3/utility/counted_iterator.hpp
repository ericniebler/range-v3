/// \file
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
//
#ifndef RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP
#define RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace _counted_iterator_
        {
            template<typename I, typename /*= void*/>
            struct counted_iterator
            {
            private:
                CONCEPT_ASSERT(Iterator<I>());
                template<typename, typename> friend struct counted_iterator;

                I current_{};
                difference_type_t<I> cnt_{0};

                void post_increment_(std::true_type)
                {
                    CONCEPT_ASSERT(std::is_void<decltype(current_++)>());
                    ++current_;
                }
                auto post_increment_(std::false_type) -> decltype(current_++)
                {
                    CONCEPT_ASSERT(!std::is_void<decltype(current_++)>());
                    auto&& tmp = current_++;
                    --cnt_;
                    return static_cast<decltype(tmp) &&>(tmp);
                }
            public:
                using iterator_type = I;
                using difference_type = difference_type_t<I>;

                counted_iterator() = default;

                counted_iterator(I x, difference_type_t<I> n)
                  : current_(std::move(x)), cnt_(n)
                {
                    RANGES_EXPECT(n >= 0);
                }

                template<typename I2,
                    CONCEPT_REQUIRES_(ConvertibleTo<I2, I>())>
                counted_iterator(const counted_iterator<I2>& i)
                  : current_(i.current_), cnt_(i.cnt_)
                {}

                template<typename I2,
                    CONCEPT_REQUIRES_(ConvertibleTo<I2, I>())>
                counted_iterator& operator=(const counted_iterator<I2>& i)
                {
                    current_ = i.current_;
                    cnt_ = i.cnt_;
                }

                I base() const
                {
                    return current_;
                }

                difference_type_t<I> count() const
                {
                    return cnt_;
                }

                reference_t<I> operator*()
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    *current_
                )
                template<typename I2 = I, CONCEPT_REQUIRES_(Readable<I2 const>())>
                reference_t<I> operator*() const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    *static_cast<I2 const &>(current_)
                )

                counted_iterator& operator++()
                {
                    RANGES_EXPECT(cnt_ > 0);
                    ++current_;
                    --cnt_;
                    return *this;
                }

                CONCEPT_REQUIRES(!ForwardIterator<I>())
                auto operator++(int) -> decltype(current_++)
                {
                    RANGES_EXPECT(cnt_ > 0);
                    return post_increment_(std::is_void<decltype(current_++)>());
                }

                CONCEPT_REQUIRES(ForwardIterator<I>())
                counted_iterator operator++(int)
                {
                    auto tmp(*this);
                    ++*this;
                    return tmp;
                }

                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                counted_iterator& operator--()
                {
                    --current_;
                    ++cnt_;
                    return *this;
                }

                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                counted_iterator operator--(int)
                {
                    auto tmp(*this);
                    --*this;
                    return tmp;
                }

                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                counted_iterator& operator+=(difference_type n)
                {
                    RANGES_EXPECT(cnt_ >= n);
                    current_ += n;
                    cnt_ -= n;
                    return *this;
                }

                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                counted_iterator operator+(difference_type n) const
                {
                    auto tmp(*this);
                    tmp += n;
                    return tmp;
                }

                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                counted_iterator& operator-=(difference_type n)
                {
                    RANGES_EXPECT(cnt_ >= -n);
                    current_ -= n;
                    cnt_ += n;
                    return *this;
                }

                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                counted_iterator operator-(difference_type n) const
                {
                    auto tmp(*this);
                    tmp -= n;
                    return tmp;
                }

                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                reference_t<I> operator[](difference_type n) const
                {
                    RANGES_EXPECT(cnt_ >= n);
                    return current_[n];
                }

                CONCEPT_REQUIRES(InputIterator<I>())
                friend RANGES_CXX14_CONSTEXPR
                rvalue_reference_t<I> iter_move(const counted_iterator& i)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    ranges::iter_move(i.current_)
                )

                template<typename I2,
                    CONCEPT_REQUIRES_(IndirectlySwappable<I2, I>())>
                friend void iter_swap(
                    const counted_iterator& x, counted_iterator<I2> const &y)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    ranges::iter_swap(x.current_, y.current_)
                )

                friend void advance(counted_iterator& i, difference_type_t<I> n)
                {
                    ranges::advance(i.current_, n);
                    i.cnt_ -= n;
                }
            };

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator==(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return x.count() == y.count();
            }

            template<typename I>
            bool operator==(const counted_iterator<I>& x, default_sentinel)
            {
                return x.count() == 0;
            }

            template<typename I>
            bool operator==(default_sentinel, const counted_iterator<I>& x)
            {
                return x.count() == 0;
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator!=(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return !(x == y);
            }

            template<typename I>
            bool operator!=(const counted_iterator<I>& x, default_sentinel y)
            {
                return !(x == y);
            }

            template<typename I>
            bool operator!=(default_sentinel x, const counted_iterator<I>& y)
            {
                return !(x == y);
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator<(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return y.count() < x.count();
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator<=(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return !(y < x);
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator>(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return y < x;
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            bool operator>=(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return !(x < y);
            }

            template<typename I1, typename I2,
                CONCEPT_REQUIRES_(Common<I1, I2>())>
            difference_type_t<I2>
            operator-(const counted_iterator<I1>& x, const counted_iterator<I2>& y)
            {
                return y.count() - x.count();
            }

            template<typename I>
            difference_type_t<I>
            operator-(const counted_iterator<I>& x, default_sentinel)
            {
                return -x.count();
            }

            template<typename I>
            difference_type_t<I>
            operator-(default_sentinel, const counted_iterator<I>& y)
            {
                return y.count();
            }

            template<typename I,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            counted_iterator<I>
            operator+(difference_type_t<I> n, const counted_iterator<I>& x)
            {
                return x + n;
            }

            template<typename I, typename = void>
            struct value_type_
            {};

            template<typename I>
            struct value_type_<I, meta::if_<Readable<I>>>
            {
                using type = value_type_t<I>;
            };

            template<typename I, typename = void>
            struct iterator_category_
            {};

            template<typename I>
            struct iterator_category_<I, meta::if_<InputIterator<I>>>
            {
                using type = iterator_category_t<I>;
            };

            template<typename I, typename = void>
            struct iterator_traits_
            {
                using iterator_category = std::output_iterator_tag;
                using difference_type = difference_type_t<I>;
                using value_type = void;
                using reference = void;
                using pointer = void;
            };

            template<typename I>
            struct iterator_traits_<I, meta::if_<InputIterator<I>>>
            {
                using iterator_category =
                    meta::if_c<
                        ForwardIterator<I>() &&
                            std::is_reference<reference_t<I>>::value,
                        std::forward_iterator_tag,
                        std::input_iterator_tag>;
                using difference_type = difference_type_t<I>;
                using value_type = value_type_t<I>;
                using reference = reference_t<I>;
                using pointer = meta::_t<detail::pointer_type_<I>>;
            };
        } // namespace _counted_iterator_

        template<typename I>
        using counted_iterator =
            _counted_iterator_::counted_iterator<I, meta::if_<Iterator<I>>>;

        template<typename I,
            CONCEPT_REQUIRES_(Iterator<I>())>
        counted_iterator<I> make_counted_iterator(I i, difference_type_t<I> n)
        {
            return {std::move(i), n};
        }

        template<typename I>
        struct value_type<counted_iterator<I>>
          : _counted_iterator_::value_type_<I>
        {};

        template<typename I>
        struct iterator_category<counted_iterator<I>>
          : _counted_iterator_::iterator_category_<I>
        {};
    }
}

/// \cond
namespace std
{
    template<typename I>
    struct iterator_traits< ::ranges::counted_iterator<I>>
      : ::ranges::_counted_iterator_::iterator_traits_<I>
    {};
}
/// \endcond

#endif
