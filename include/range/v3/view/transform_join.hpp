#pragma once
#include <range/v3/all.hpp>
#include <optional>
#include <variant>
namespace ranges {
	namespace detail {

		template<typename Rng>
		struct semiregular_boxed_range {
			semiregular_boxed_range() = default;
			semiregular_boxed_range(Rng r) :m_rng(std::forward<Rng>(r)) {}

			auto begin()& {
				if constexpr (is_default_cons) {
					return m_rng.begin();
				}
				else {
					return m_rng->begin();
				}
			}

			auto begin()&& {
				if constexpr (is_default_cons) {
					return m_rng.begin();
				}
				else {
					return m_rng->begin();
				}
			}

			auto begin()const& {
				if constexpr (is_default_cons) {
					return m_rng.begin();
				}
				else {
					return m_rng->begin();
				}
			}

			auto begin()const&& {
				if constexpr (is_default_cons) {
					return m_rng.begin();
				}
				else {
					return m_rng->begin();
				}
			}

			auto end()& {
				if constexpr (is_default_cons) {
					return m_rng.end();
				}
				else {
					return m_rng->end();
				}
			}

			auto end()&& {
				if constexpr (is_default_cons) {
					return m_rng.end();
				}
				else {
					return m_rng->end();
				}
			}

			auto end()const& {
				if constexpr (is_default_cons) {
					return m_rng.end();
				}
				else {
					return m_rng->end();
				}
			}

			auto end()const&& {
				if constexpr (is_default_cons) {
					return m_rng.end();
				}
				else {
					return m_rng->end();
				}
			}

		private:
			static constexpr bool is_default_cons = std::is_default_constructible_v<Rng>;
			using stored_range = std::conditional_t<is_default_cons, Rng, std::optional<Rng>>;

			stored_range m_rng{};
		};

		template<typename rng>
		static constexpr auto stored_inner_range(rng&& r) {
			if constexpr (ranges::viewable_range<rng>) {
				return ranges::views::all(r);
			}
			else {
				return semiregular_boxed_range<rng>(r);
			}
		}

		template<typename rng>
		static constexpr decltype(auto) make_storeable(rng&& r)
		{
			if constexpr (ranges::viewable_range<rng>)
			{
				return ranges::views::all(std::forward<rng>(r));
			}
			else
			{
				return std::forward<rng>(r);
			}
		}

	}

	CPP_def(
		template(typename Rng, typename Fun)
		concept transform_joinable_range,
		ranges::views::transformable_range<Rng, Fun>&&
		ranges::input_range<std::invoke_result_t<Fun&, ranges::range_reference_t<Rng>>> &&
		(!std::is_void_v<ranges::range_reference_t<std::invoke_result_t<Fun&, ranges::range_reference_t<Rng>>>>)
	);

	template<typename Rng, typename Fun>
	struct transform_join_view :
		ranges::view_facade<
		transform_join_view<Rng, Fun>,
		ranges::detail::join_cardinality<std::invoke_result_t<Fun, ranges::range_reference_t<Rng>>>()
		>
	{

		using outer_iterator = ranges::iterator_t<Rng>;
		using outer_range_t = Rng;
		using inner_range_t = std::invoke_result_t<Fun, ranges::range_reference_t<Rng>>;
		static_assert(ranges::input_range_<inner_range_t>);

		transform_join_view() = default;
		explicit transform_join_view(Rng outer, Fun f) :
			m_outer(std::move(outer)), m_fn(std::move(f)) {}


		using COuter = Rng;
		using CInner = std::invoke_result_t<Fun, ranges::range_reference_t<COuter>>;
		using stored_range = decltype(detail::stored_inner_range(std::declval<CInner>()));
		//^^^ can't be std::conditional<ranges::viewable<CInner>,all_t<CInner>,make_default_constructible<CInner>> 
	private:
		Rng m_outer{};
		Fun m_fn;

		stored_range inner_range;

		friend ranges::range_access;


		struct cursor {
			using Parent = transform_join_view;
			//since all_t<CInner> will be an error if  ranges::viewable<CInner> is false
			using ref_is_glvalue = std::is_lvalue_reference<CInner>;

		private:
			Parent* m_rng = nullptr;
			ranges::iterator_t<COuter> outer_it_{};
			ranges::iterator_t<CInner> inner_it_{};
		public:

			cursor() = default;

			cursor(Parent& p) :
				m_rng(&p),
				outer_it_(ranges::begin(p.m_outer))
			{
				m_rng->inner_range = stored_range();
				satisfy();
			}

			cursor(Parent& p, ranges::iterator_t<COuter> outer) :
				m_rng(&p),
				outer_it_(outer)
			{
				m_rng->inner_range = stored_range();
				satisfy();
			}

			constexpr void next() {
				if (++inner_it_ == ranges::end(m_rng->inner_range)) {
					++outer_it_;
					satisfy();
				}
			}

			constexpr bool equal(ranges::default_sentinel_t) const {
				return outer_it_ == ranges::end(m_rng->m_outer);
			}


			constexpr bool done()const {
				return outer_it_ == ranges::end(m_rng->m_outer);
			}

			constexpr auto read() const noexcept(noexcept(*inner_it_)) -> decltype(*inner_it_) {
				return (*inner_it_);
			}

			CPP_member
				constexpr auto arrow()->CPP_ret(ranges::iterator_t<CInner>)( //
					requires ranges::detail::has_arrow_<ranges::iterator_t<CInner>>)
			{
				return inner_it_;
			}
		private:

			constexpr void satisfy() {
				for (; outer_it_ != ranges::end(m_rng->m_outer); ++outer_it_) {
					m_rng->inner_range = detail::make_storeable(std::invoke(m_rng->m_fn, *outer_it_));
					inner_it_ = ranges::begin(m_rng->inner_range);
					if (inner_it_ != ranges::end(m_rng->inner_range)) {
						return;
					}
				}
				//assert(outer_it_ == ranges::end(m_rng->m_outer));
				inner_it_ = ranges::iterator_t<CInner>();
				//^^^ so that it'll be  == with another iterator at the end
			}

		};

		constexpr cursor begin_cursor() {
			return { *this };
		}


		constexpr ranges::default_sentinel_t end_cursor() {
			return ranges::default_sentinel;
		}

	};
	namespace views {
		struct transform_join_fn {
			friend ranges::views::view_access;

			template<typename Rng, typename Fun>            
			constexpr auto operator()(Rng&& rng, Fun fun)const
				->CPP_ret(transform_join_view<ranges::views::all_t<Rng>, Fun>)(
					requires transform_joinable_range<ranges::views::all_t<Rng>, Fun>)
			{
				return transform_join_view(ranges::views::all(static_cast<Rng&&>(rng)), std::move(fun));
			}

		private:
			template<typename Fun>
			static constexpr auto bind(transform_join_fn transform, Fun fun)
			{
				return ranges::make_pipeable(ranges::bind_back(transform, std::move(fun)));
			}
		};

		RANGES_INLINE_VARIABLE(ranges::views::view<transform_join_fn>, transform_join);
	} // namespace views
};// namespace ranges

