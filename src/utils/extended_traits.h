#ifndef EXTENDED_TRAITS_H
#define EXTENDED_TRAITS_H

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

template <typename T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;


template <typename T, typename... P0toN>
struct is_one_of : std::bool_constant<(std::is_same_v<T, P0toN> || ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr is_one_of_v = is_one_of<T, P0toN...>::value;

template <typename T, typename... P0toN>
struct all_same : std::bool_constant<(std::is_same_v<T, P0toN> && ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr all_same_v = all_same<T, P0toN...>::value;

template <typename T, typename... P0toN>
struct all_convertible : std::bool_constant<(std::is_convertible_v<T, P0toN> && ...)> { };

template <typename T, typename... P0toN>
inline bool constexpr all_convertible_v = all_convertible<T, P0toN...>::value;

/* impls (if applicable) */
namespace {
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


	template <typename, typename = void>
	struct get_fundamental_type_impl { };

	template <typename T>
	struct get_fundamental_type_impl<T, std::enable_if_t<std::is_fundamental_v<T>>> : type_is<T> { };

	template <typename T>
	struct get_fundamental_type_impl<T, std::void_t<typename T::value_type>> : std::conditional_t<
																					std::is_fundamental_v<typename T::value_type>,
																					type_is<typename T::value_type>,
																					get_fundamental_type_impl<typename T::value_type>
																				> { };
	
	template <typename T>
	struct get_fundamental_type_impl<T*> : type_is<T> { };
}

/* Container stored contiguously in memory */
template <typename T>
struct is_contiguously_stored : is_contiguously_stored_impl<remove_cvref_t<T>> { };

template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

/* Class/structure wraps numeric type */
template <typename T, typename U = void>
struct wraps_numeric_type : wraps_numeric_type_impl<remove_cvref_t<T>, U> { };

template <typename T, typename U = void>
inline bool constexpr wraps_numeric_type_v = wraps_numeric_type<T,U>::value;

/* Class/structure wraps integral type */
template <typename T, typename U = void>
struct wraps_integral_type : wraps_integral_type_impl<remove_cvref_t<T>, U> { };

template <typename T, typename U = void>
inline bool constexpr wraps_integral_type_v = wraps_integral_type<T,U>::value;

/* Class/structure wraps unsigned type */
template <typename T, typename U = void>
struct wraps_unsigned_type : wraps_unsigned_type_impl<remove_cvref_t<T>, U> { };

template <typename T, typename U = void>
inline bool constexpr wraps_unsigned_type_v = wraps_unsigned_type<T,U>::value;

/* Get underlying fundamental type */
template <typename T, typename U = void>
struct get_fundamental_type : get_fundamental_type_impl<std::decay_t<T>, void> { };

template <typename T, typename U = void>
using get_fundamental_type_t = typename get_fundamental_type<T,U>::type;

#endif
