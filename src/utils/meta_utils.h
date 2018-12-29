#ifndef META_UTILS_H
#define META_UTILS_H

#pragma once
#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
struct type_is {
	using type = T;
};

template <typename T>
using type_is_t = typename type_is<T>::type;

/* Remove cv and ref qualifiers */
template <typename T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;


/* Type T is same as one of types in parameter pack */
template <typename T, typename... P0toN>
struct is_one_of;

template <typename T>
struct is_one_of<T> : std::false_type { };

template <typename T, typename... P1toN>
struct is_one_of<T, T, P1toN...> : std::true_type { };

template <typename T, typename P0, typename... P1toN>
struct is_one_of<T, P0, P1toN...> : is_one_of<T, P1toN...> { };

template <typename T, typename... P0toN>
inline bool constexpr is_one_of_v = is_one_of<T, P0toN...>::value;

/* impls (if applicable) */
namespace {
	template <typename T>
	struct is_contiguously_stored_impl : std::false_type { };
	
	template <typename T, typename... Ts>
	struct is_contiguously_stored_impl<std::vector<T, Ts...>> : std::true_type { };

	template <typename T, std::size_t N>
	struct is_contiguously_stored_impl<std::array<T, N>> : std::true_type { };

	template <typename T, std::size_t N>
	struct is_contiguously_stored_impl<T[N]> : std::true_type { };
	
	template <typename T>
	struct is_contiguously_stored_impl<T[]> : std::true_type { };

	template <typename T, typename = void>
	struct wraps_numeric_type_impl : std::false_type { };

	template <typename T>
	struct wraps_numeric_type_impl<T, std::void_t<typename T::value_type>> : std::is_arithmetic<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_numeric_type_impl<T[N]> : std::is_arithmetic<T> { };
	
	template <typename T>
	struct wraps_numeric_type_impl<T[]> : std::is_arithmetic<T> { };


	template <typename T, typename = void>
	struct wraps_integral_type_impl : std::false_type { };

	template <typename T>
	struct wraps_integral_type_impl<T, std::void_t<typename T::value_type>> : std::is_integral<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_integral_type_impl<T[N]> : std::is_integral<T> { };
	
	template <typename T>
	struct wraps_integral_type_impl<T[]> : std::is_integral<T> { };


	template <typename T, typename = void>
	struct wraps_unsigned_type_impl : std::false_type { };

	template <typename T>
	struct wraps_unsigned_type_impl<T, std::void_t<typename T::value_type>> : std::is_unsigned<typename T::value_type> { };

	template <typename T, std::size_t N>
	struct wraps_unsigned_type_impl<T[N]> : std::is_unsigned<T> { };

	template <typename T>
	struct wraps_unsigned_type_impl<T[]> : std::is_unsigned<T> { };


	template <typename T, typename = void>
	struct get_value_type_impl { };

	template <typename T>
	struct get_value_type_impl<T, std::void_t<typename T::value_type>> : type_is<typename T::value_type> { };
	
	template <typename T, std::size_t N>
	struct get_value_type_impl<T[N]> : type_is<T> { };

	template <typename T>
	struct get_value_type_impl<T[]> : type_is<T> { };
}

/* Container stored contiguously in memory */
template <typename T>
struct is_contiguously_stored : is_contiguously_stored_impl<remove_cvref_t<T>> { };

/* Class/structure wraps numeric type */
template <typename T, typename U = void>
struct wraps_numeric_type : wraps_numeric_type_impl<remove_cvref_t<T>, U> { };

/* Class/structure wraps integral type */
template <typename T, typename U = void>
struct wraps_integral_type : wraps_integral_type_impl<remove_cvref_t<T>, U> { };

/* Class/structure wraps unsigned type */
template <typename T, typename U = void>
struct wraps_unsigned_type : wraps_unsigned_type_impl<remove_cvref_t<T>, U> { };

/* Get value_type or equivalent */
template <typename T, typename U = void>
struct get_value_type : get_value_type_impl<remove_cvref_t<T>, void> { };

/* Alias templates */
template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

template <typename T, typename U = void>
inline bool constexpr wraps_numeric_type_v = wraps_numeric_type<T,U>::value;

template <typename T, typename U = void>
inline bool constexpr wraps_integral_type_v = wraps_integral_type<T,U>::value;

template <typename T, typename U = void>
inline bool constexpr wraps_unsigned_type_v = wraps_unsigned_type<T,U>::value;

template <typename T, typename U = void>
using get_value_type_t = typename get_value_type<T,U>::type;


#endif
