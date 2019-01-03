#ifndef CONTAINER_H
#define CONTAINER_H

#pragma once
#include "extended_traits.h"
#include <array>
#include <forward_list>
#include <queue>
#include <stack>
#include <type_traits>
#include <utility>

namespace container {
	template <typename, typename = void>
	struct supports_preallocation : std::false_type { };
	
	template <typename T>
	struct supports_preallocation<T, std::void_t<decltype(std::declval<T>().reserve(std::declval<typename T::value_type>()))>> : std::true_type {  };

	template <typename T, typename U = void>
	inline bool constexpr supports_preallocation_v = supports_preallocation<T,U>::value;

	template <typename Container, typename P0, typename... P1toN>
	struct inserter_helper {
		static void insert(Container& c, P0&& first, P1toN&&... rest) {
			c.insert(std::end(c), std::forward<P0>(first));
			if constexpr(sizeof...(rest) > 0)
				inserter_helper<Container, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}
	};

	template <typename P0, typename... P1toN>
	struct inserter_helper<std::forward_list<P0>, P0, P1toN...> {

		static void insert(std::forward_list<P0>& c, P0&& first, P1toN&&... rest) {
			c.push_front(std::forward<P0>(first));
			if constexpr(sizeof...(rest) > 0)
				inserter_helper<std::forward_list<P0>, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}
	};

	template <typename P0, typename... P1toN>
	struct inserter_helper<std::stack<P0>, P0, P1toN...> {
		
		static void insert(std::stack<P0>& c, P0&& first, P1toN&&... rest) {
			c.push(std::forward<P0>(first));
			if constexpr(sizeof...(rest) > 0)
				inserter_helper<std::stack<P0>, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}
	};

	template <typename P0, typename... P1toN>
	struct inserter_helper<std::queue<P0>, P0, P1toN...> {

		static void insert(std::queue<P0>& c, P0&& first, P1toN&&... rest) {
			c.push(std::forward<P0>(first));
			if constexpr(sizeof...(rest) > 0)
				inserter_helper<std::queue<P0>, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}
	};

	template <typename P0, typename... P1toN>
	struct inserter_helper<std::priority_queue<P0>, P0, P1toN...> {

		static void insert(std::priority_queue<P0>& c, P0&& first, P1toN&&... rest) {
			c.push(std::forward<P0>(first));
			if constexpr(sizeof...(rest) > 0)
				inserter_helper<std::queue<P0>, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}
	};

	template <std::size_t Size, std::size_t N, typename P0, typename... P1toN>
	struct array_inserter {
		static void insert(std::array<P0, Size>& c, P0&& first, P1toN&&... rest) {
			c[N] = std::forward<P0>(first);
			if constexpr(sizeof...(rest) > 0)
				array_inserter<Size, N+1, P1toN...>::insert(c, std::forward<P1toN>(rest)...);
		}

	};

	template <typename, typename = void>
	struct inserter { };


	template <typename Container>
	struct inserter<Container, 
		   std::void_t<typename Container::value_type>> 
	{
		template <typename... Args>
		void operator()(Container& c, Args&&... args) {
			static_assert((is_convertible_to_all_v<typename Container::value_type, Args...>), "Types are non-conformant");

			if constexpr(is_std_array_v<Container>){
				static_assert(sizeof...(args) <= std::tuple_size_v<Container>, "Cannot insert a larger number of values than the array supports");
				array_inserter<std::tuple_size_v<Container>, 0u, Args...>::insert(c, std::forward<Args>(args)...);
			}
			else{
				if constexpr (supports_preallocation_v<Container>)
					c.reserve(sizeof...(args));
				inserter_helper<Container, Args...>::insert(c, std::forward<Args>(args)...);
			}
		}
	};


}

#endif
