#ifndef TRAITS_H
#define TRAITS_H

#pragma once
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
struct type_is {
	using type = T;
};

template <typename T>
using type_is_t = typename type_is<T>::type;

template <std::size_t N>
struct size_t_constant : std::integral_constant<std::size_t, N> { };

template <std::size_t N>
inline std::size_t constexpr size_t_constant_v = size_t_constant<N>::value;

template <typename T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T>
struct remove_cvptr : std::remove_cv<std::remove_pointer_t<T>> { };

template <typename T>
using remove_cvptr_t = typename remove_cvptr<T>::type;

namespace {
	template <typename, typename = void>
	struct fundamental_type_impl { };

	template <typename T>
	struct fundamental_type_impl<T, std::enable_if_t<std::is_fundamental_v<T>>> : type_is<T> { };

	template <typename T>
	struct fundamental_type_impl<T, std::void_t<typename T::value_type>> : std::conditional_t<
																					std::is_fundamental_v<typename T::value_type>,
																					type_is<typename T::value_type>,
																					fundamental_type_impl<typename T::value_type>
																				> { };
	
	template <typename T>
	struct fundamental_type_impl<T*> : type_is<T> { };
}

template <typename T>
struct fundamental_type : fundamental_type_impl<std::decay_t<T>> { };

template <typename T>
using fundamental_type_t = typename fundamental_type<T>::type;

template <typename, typename = void>
struct has_value_type : std::false_type { };

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type { };

template <typename T>
inline bool constexpr has_value_type_v = has_value_type<T>::value;

template <std::size_t N, typename P0, typename... P1toM>
struct nth_type : nth_type<N-1, P1toM...> { };

template <typename P0, typename... P1toM>
struct nth_type<0, P0, P1toM...> : type_is<P0> { };

template <std::size_t N, typename... P0toM>
using nth_type_t = typename nth_type<N, P0toM...>::type;
template <std::size_t N, typename P0, typename... P1toM>
struct nth_value {
	decltype(auto) constexpr operator()(P0&&, P1toM&&... tail) {			/* constexpr iff parameters are constexpr */
		return nth_value<N-1, P1toM...>{}(std::forward<P1toM>(tail)...);
	}
};

template <typename P0, typename... P1toM>
struct nth_value<0, P0, P1toM...>{
	decltype(auto) constexpr operator()(P0&& head, P1toM&&...) {
		return head;
	}
};

/* Variadic type comparisons */
/* ------------------------- */
template <typename T, typename... P0toN>
struct is_one_of : std::bool_constant<(std::is_same_v<T, P0toN> || ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr is_one_of_v = is_one_of<T, P0toN...>::value;

template <typename T, typename... P0toN>
struct all_same : std::bool_constant<(std::is_same_v<T, P0toN> && ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr all_same_v = all_same<T, P0toN...>::value;

template <typename T, typename... P0toN>
struct is_convertible_to_all : std::bool_constant<(std::is_convertible_v<T, P0toN> && ...)> { };

template <typename T>
struct is_convertible_to_all<T> : std::false_type { };

template <typename T, typename... P0toN>
inline bool constexpr is_convertible_to_all_v = is_convertible_to_all<T, P0toN...>::value;

template <typename T, typename... P0toN>
struct is_convertible_to_one_of : std::bool_constant<(std::is_convertible_v<T, P0toN> || ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr is_convertible_to_one_of_v = is_convertible_to_one_of<T, P0toN...>::value;

/* Helpers */
namespace {
	template <typename>
	struct is_std_array_impl : std::false_type { };

	template <typename T, std::size_t N>
	struct is_std_array_impl<std::array<T,N>> : std::true_type { };


	template <typename, typename = void>
	struct has_subscript_operator_impl : std::false_type { };

	template <typename T>
	struct has_subscript_operator_impl<T, std::void_t<decltype(std::declval<T>().operator[](std::declval<std::size_t>()))>> : std::true_type { };

	template <typename T, std::size_t N>
	struct has_subscript_operator_impl<T[N]> : std::true_type { };

	template <typename T>
	struct has_subscript_operator_impl<T[]> : std::true_type { };


	template <typename>
	struct is_contiguously_stored_impl : std::false_type { };
	
	template <typename T, typename... Ts>
	struct is_contiguously_stored_impl<std::vector<T, Ts...>> : std::true_type { };

	template <typename T, std::size_t N>
	struct is_contiguously_stored_impl<std::array<T, N>> : std::true_type { };

	template <typename T, std::size_t N>
	struct is_contiguously_stored_impl<T[N]> : std::true_type { };
	
	template <typename T>
	struct is_contiguously_stored_impl<T[]> : std::true_type { };


	template <typename, typename = void>
	struct wraps_numeric_type_impl : std::false_type { };

	template <typename T>
	struct wraps_numeric_type_impl<T, std::void_t<typename T::value_type>> : std::is_arithmetic<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_numeric_type_impl<T[N]> : std::is_arithmetic<T> { };
	
	template <typename T>
	struct wraps_numeric_type_impl<T[]> : std::is_arithmetic<T> { };


	template <typename, typename = void>
	struct wraps_integral_type_impl : std::false_type { };

	template <typename T>
	struct wraps_integral_type_impl<T, std::void_t<typename T::value_type>> : std::is_integral<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_integral_type_impl<T[N]> : std::is_integral<T> { };
	
	template <typename T>
	struct wraps_integral_type_impl<T[]> : std::is_integral<T> { };


	template <typename, typename = void>
	struct wraps_unsigned_type_impl : std::false_type { };

	template <typename T>
	struct wraps_unsigned_type_impl<T, std::void_t<typename T::value_type>> : std::is_unsigned<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_unsigned_type_impl<T[N]> : std::is_unsigned<T> { };

	template <typename T>
	struct wraps_unsigned_type_impl<T[]> : std::is_unsigned<T> { };


}

/* Container traits */
/* ---------------- */
template <typename T>
struct is_std_array : is_std_array_impl<remove_cvref_t<T>> { };

template <typename T>
inline bool constexpr is_std_array_v = is_std_array<T>::value;

template <typename T>
struct has_subscript_operator : has_subscript_operator_impl<remove_cvref_t<T>> { };

template <typename T>
inline bool constexpr has_subscript_operator_v = has_subscript_operator<T>::value;


template <typename T>
struct is_contiguously_stored : is_contiguously_stored_impl<remove_cvref_t<T>> { }; 	

template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

template <typename T>
struct wraps_numeric_type : wraps_numeric_type_impl<remove_cvref_t<T>> { }; 

template <typename T>
inline bool constexpr wraps_numeric_type_v = wraps_numeric_type<T>::value;

template <typename T>
struct wraps_integral_type : wraps_integral_type_impl<remove_cvref_t<T>> { };	

template <typename T>
inline bool constexpr wraps_integral_type_v = wraps_integral_type<T>::value;

template <typename T>
struct wraps_unsigned_type : wraps_unsigned_type_impl<remove_cvref_t<T>> { };			

template <typename T>
inline bool constexpr wraps_unsigned_type_v = wraps_unsigned_type<T>::value;

template <typename, typename = void>
struct supports_preallocation : std::false_type { };								

template <typename T>
struct supports_preallocation<T, std::void_t<decltype(std::declval<T>().reserve(std::declval<typename T::size_type>()))>> : std::true_type {  };

template <typename T>
inline bool constexpr supports_preallocation_v = supports_preallocation<T>::value;


#endif
