#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#pragma once
#include "traits.h"
#include "type_conversion.h"
#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename Container, typename = std::void_t<decltype(std::begin(std::declval<Container>()))>>
auto constexpr opt_c_begin(Container&& c);

template <typename Container, typename = std::void_t<decltype(std::end(std::declval<Container>()))>>
auto constexpr opt_c_end(Container&& c);

template <typename... Iters>
class zip_iterator {
	using ref_tuple_t = std::tuple<typename std::iterator_traits<Iters>::reference...>;
	using iter_tuple_t = std::tuple<Iters...>;
	public:
		using value_type = ref_tuple_t;
		using reference = value_type;
		using pointer = void;
		using difference_type = void;
		using iterator_category = std::forward_iterator_tag;

		zip_iterator& operator++();
		zip_iterator operator++(int);
		reference operator*();

		bool operator==(zip_iterator const& rhs) const;
		bool operator!=(zip_iterator const& rhs) const;

	private:
		iter_tuple_t iters_{};

		zip_iterator() = default;
		zip_iterator(iter_tuple_t&& iters);

		template <std::size_t... Is>
		void increment(std::index_sequence<Is...>);

		template <std::size_t... Is>
		ref_tuple_t dereference(std::index_sequence<Is...>);

		template <typename Tuple, std::size_t... Is>
		static bool compare(Tuple const& l, Tuple const& r, std::index_sequence<Is...>);

		template <typename...>
		friend class zip_collection;

};

template <typename... Conts>
class zip_collection{
	public:
		using iterator = zip_iterator<get_iterator_t<Conts>...>;
		
		zip_collection(zip_collection const&) = delete;
		zip_collection& operator=(zip_collection const&) = delete;

		iterator begin() noexcept;
		iterator end() noexcept;

	private:
		std::tuple<Conts...> collection_;
		iterator begins_, ends_;

		zip_collection(std::tuple<Conts...>&& t);
		
		using iter_tuple_t = typename iterator::iter_tuple_t;

		template <std::size_t... Is>
		void init(std::index_sequence<Is...>);

		template <typename... Args>
		friend zip_collection<Args...> zip(Args&&...);

};

template <typename... Args>
zip_collection<Args...> zip(Args&&... args);

template <typename Iter, typename Op>
class fold_iterator {
	public:
		using value_type = typename std::iterator_traits<Iter>::value_type;
		using reference = value_type;
		using pointer = void;
		using difference_type = void;
		using iterator_category = std::forward_iterator_tag;

		fold_iterator& operator++();
		fold_iterator operator++(int);
		reference operator*();

		bool operator==(fold_iterator const& rhs) const;
		bool operator!=(fold_iterator const& rhs) const;

	private:
		Iter it_{};
		value_type current_{};
		Op op_{};

		fold_iterator(Iter it, value_type&& init, Op op);

		template <typename, typename>
		friend class fold_collection;
};


template <typename Container, typename Op>
class fold_collection {
	using cont_iter_t = decltype(std::cbegin(std::declval<Container>()));
	public:
		using value_type = typename std::iterator_traits<cont_iter_t>::value_type;
		using iterator = fold_iterator<cont_iter_t, Op>;
		
		iterator begin();
		iterator end();

	private:
		iterator begin_, end_;

		fold_collection(Container const& c, value_type&& init, Op op);

		template <typename T, typename U>
		friend fold_collection<T, U> fold(T const&, value_type_t<T>, U);
};


template <typename Container, typename Op = std::plus<value_type_t<Container>>>
fold_collection<Container, Op> fold(Container const& c, value_type_t<Container> init = {}, Op op = {});

template <typename Iter>
class enumerate_iterator {
	using cont_value_type_t = typename std::iterator_traits<Iter>::value_type;
	using opt_c_value_type_t = std::conditional_t<is_const_iterator_v<Iter>, 
												  cont_value_type_t const, 
												  cont_value_type_t>;
	using enum_pair_t = std::pair<std::size_t, std::reference_wrapper<opt_c_value_type_t>>;
	public:
		using value_type = enum_pair_t;
		using reference = value_type;
		using pointer = void;
		using difference_type = void;
		using iterator_category = std::forward_iterator_tag;

		enumerate_iterator& operator++();
		enumerate_iterator operator++(int);
		reference operator*();

		bool operator==(enumerate_iterator const& rhs) const;
		bool operator!=(enumerate_iterator const& rhs) const;

	private:
		Iter it_;
		std::size_t idx_;

		enumerate_iterator(Iter it);

		template <typename>
		friend class enumerate_collection;
};

template <typename Iter>
class enumerate_collection {
	public:
		using iterator = enumerate_iterator<Iter>;

		iterator begin();
		iterator end();
	private:
		iterator begin_, end_;

		enumerate_collection(Iter begin, Iter end);
		
		template <typename Container>
		friend enumerate_collection<get_iterator_t<Container>> enumerate(Container&&);
};

template <typename Container>
enumerate_collection<get_iterator_t<Container>> enumerate(Container&& c);


template <typename Enum, typename... Rest>
struct enum_size : size_t_constant<enum_value(Enum::End_) * enum_size<Rest...>::value> { };

template <typename Enum>
struct enum_size<Enum> : size_t_constant<enum_value(Enum::End_)> { };

template <typename Enum, typename... Rest>
inline std::size_t constexpr enum_size_v = enum_size<Enum, Rest...>::value;

template <typename Enum, typename... Rest>
struct bind_enum_impl {
	static std::size_t constexpr fold(Enum e, Rest... rest) {
		return enum_value(e) * enum_size_v<Rest...> + bind_enum_impl<Rest...>::fold(rest...);
	}
};

template <typename Enum>
struct bind_enum_impl<Enum> {
	static std::size_t constexpr fold(Enum e) {
		return enum_value(e);
	}
};


/* Inspired by an excellent blog post by Jonathan Boccara found at
 * fluentcpp.com/2017/06/27/how-to-collapse-nested-switch-statements/ */
/* Combine any number of enums at compile time */
/* See Shader::upload_uniform(GLint, Args...) in shader.tcc for example of usage */
/* Requirements:
 * 	- Underlying values are all positive
 * 	- Underlying values are consecutive
 * 	- Has a dummy enumerator called End_, listed last among the enumerators */

template <typename Enum, typename... Rest>
struct bind_enum {
	static_assert(std::is_enum_v<Enum> && (std::is_enum_v<Rest> && ...), "Cannot fold non-enum type");

	constexpr std::size_t operator()(Enum e, Rest... rest) {
		return bind_enum_impl<Enum, Rest...>::fold(e, rest...);
	}
};

#include "collections.tcc"
#endif
