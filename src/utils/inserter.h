#ifndef INSERTER_H
#define INSERTER_H

#pragma once
#include "traits.h"
#include <array>
#include <forward_list>
#include <queue>
#include <stack>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename Container, typename P0, typename... P1toN>
struct inserter_helper {
	static void insert(Container& c, P0&& head, P1toN&&... tail) {
		c.insert(std::end(c), std::forward<P0>(head));
		if constexpr(sizeof...(tail) > 0)
			inserter_helper<Container, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}
};

template <typename P0, typename... P1toN>
struct inserter_helper<std::forward_list<P0>, P0, P1toN...> {

	static void insert(std::forward_list<P0>& c, P0&& head, P1toN&&... tail) {
		c.push_front(std::forward<P0>(head));
		if constexpr(sizeof...(tail) > 0)
			inserter_helper<std::forward_list<P0>, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}
};

template <typename P0, typename... P1toN>
struct inserter_helper<std::stack<P0>, P0, P1toN...> {
	
	static void insert(std::stack<P0>& c, P0&& head, P1toN&&... tail) {
		c.push(std::forward<P0>(head));
		if constexpr(sizeof...(tail) > 0)
			inserter_helper<std::stack<P0>, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}
};

template <typename P0, typename... P1toN>
struct inserter_helper<std::queue<P0>, P0, P1toN...> {

	static void insert(std::queue<P0>& c, P0&& head, P1toN&&... tail) {
		c.push(std::forward<P0>(head));
		if constexpr(sizeof...(tail) > 0)
			inserter_helper<std::queue<P0>, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}
};

template <typename P0, typename... P1toN>
struct inserter_helper<std::priority_queue<P0>, P0, P1toN...> {

	static void insert(std::priority_queue<P0>& c, P0&& head, P1toN&&... tail) {
		c.push(std::forward<P0>(head));
		if constexpr(sizeof...(tail) > 0)
			inserter_helper<std::queue<P0>, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}
};

template <typename Container, std::size_t N, std::size_t I, typename P0, typename... P1toN>
struct static_inserter_helper {
	static void insert(Container& c, P0&& head, P1toN&&... tail) {
		c[I] = std::forward<P0>(head);
		if constexpr(sizeof...(tail) > 0)
			static_inserter_helper<Container, N, I+1, P1toN...>::insert(c, std::forward<P1toN>(tail)...);
	}

};

template <typename Container, std::size_t N>
struct static_inserter {
	template <typename... Args>
	void operator()(Container& c, Args&&... args) {
		static_inserter_helper<Container, N, 0u, Args...>::insert(c, std::forward<Args>(args)...);
	}
};


struct force_static_insertion_tag { };

template <typename Container, typename = void>
struct inserter {
	static_assert(has_value_type_v<Container>, "Container must name a value type, pass force_static_insertion_tag to override");
	template <typename... Args>
	void operator()(Container& c, Args&&... args) {
		static_assert(!std::is_const_v<std::remove_reference_t<Container>>, "Call to inserter with const qualified container not supported");
		static_assert((all_same_v<typename Container::value_type, Args...>), "Types are non-conformant");

		if constexpr(is_std_array_v<Container>){
			static_assert(sizeof...(args) <= std::tuple_size_v<Container>, "Cannot insert a larger number of values than the array supports");
			static_inserter<Container, std::tuple_size_v<Container>>{}(c, std::forward<Args>(args)...);
		}
		else{
			if constexpr (supports_preallocation_v<Container>)
				c.reserve(sizeof...(args));
			inserter_helper<Container, Args...>::insert(c, std::forward<Args>(args)...);
		}
	}
};

/* Insertion for non-standard, static size containers can be enabled by passing the force_static_insertion_tag.
 * Passing more parameters than can be stored will result in undefined behavior */
template <typename Container>
struct inserter<Container, force_static_insertion_tag> {
	static_assert(has_subscript_operator_v<Container>, "Container must have operator[]");
	template <typename... Args>
	void operator()(Container& c, Args&&... args) {
		static_assert(!std::is_const_v<std::remove_reference_t<Container>>, "Call to inserter with const qualified container not supported");
		static_assert((all_same_v<Args...>), "Types are non-conformant");

		if constexpr(has_subscript_operator_v<Container>)
			static_assert(std::is_reference_v<decltype(std::declval<Container>().operator[](std::declval<std::size_t>()))>, "Container's operator[] must return a reference");

		static_inserter<Container, sizeof...(args)>{}(c, std::forward<Args>(args)...);
	}
};


#endif
