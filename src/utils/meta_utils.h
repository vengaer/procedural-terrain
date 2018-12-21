#ifndef META_UTILS_H
#define META_UTILS_H

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


/* Type T is same as one of types in parameter pack */
template <typename T, typename... P0toN>
struct is_one_of;

template <typename T>
struct is_one_of<T> : std::false_type { };

template <typename T, typename... P1toN>
struct is_one_of<T, T, P1toN...> : std::true_type { };

template <typename T, typename P0, typename... P1toN>
struct is_one_of<T, P0, P1toN...> : is_one_of<T, P1toN...> { };


/* Container stored contiguously in memory */
template <typename T>
struct is_contiguously_stored : std::false_type { };

/* std::vector */
template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...>> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> const> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> volatile> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> const volatile> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...>&> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> const&> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> volatile&> : std::true_type { };

template <typename T, typename... Ts>
struct is_contiguously_stored<std::vector<T, Ts...> const volatile&> : std::true_type { };

/* std::array */
template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N>> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> const> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> volatile> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> const volatile> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N>&> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> const&> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> volatile&> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N> const volatile&> : std::true_type { };

/* Bounded array */
template <typename T, std::size_t N>
struct is_contiguously_stored<T[N]> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<T(&)[N]> : std::true_type { };

/* Unbounded array */
template <typename T>
struct is_contiguously_stored<T[]> : std::true_type { };

template <typename T>
struct is_contiguously_stored<T(&)[]> : std::true_type { };


/* Class/structure wraps numeric type */
template <typename T, typename = void>
struct wraps_numeric_type : std::false_type { };

template <typename T>
struct wraps_numeric_type<T, std::void_t<typename T::value_type>> : std::is_arithmetic<typename T::value_type> { };

template <typename T>
struct wraps_numeric_type<T&, std::void_t<typename T::value_type>> : std::is_arithmetic<typename T::value_type> { };

template <typename T, std::size_t N>
struct wraps_numeric_type<T[N]> : std::is_arithmetic<T> { };

template <typename T, std::size_t N>
struct wraps_numeric_type<T(&)[N]> : std::is_arithmetic<T> { };

template <typename T>
struct wraps_numeric_type<T[]> : std::is_arithmetic<T> { };

template <typename T>
struct wraps_numeric_type<T(&)[]> : std::is_arithmetic<T> { };


/* Class/structure wraps integral type */
template <typename T, typename = void>
struct wraps_integral_type : std::false_type { };

template <typename T>
struct wraps_integral_type<T, std::void_t<typename T::value_type>> : std::is_integral<typename T::value_type> { };

template <typename T>
struct wraps_integral_type<T&, std::void_t<typename T::value_type>> : std::is_integral<typename T::value_type> { };

template <typename T, std::size_t N>
struct wraps_integral_type<T[N]> : std::is_integral<T> { };

template <typename T, std::size_t N>
struct wraps_integral_type<T(&)[N]> : std::is_integral<T> { };

template <typename T>
struct wraps_integral_type<T[]> : std::is_integral<T> { };

template <typename T>
struct wraps_integral_type<T(&)[]> : std::is_integral<T> { };


/* Get value_type or equivalent */
template <typename T, typename = void>
struct get_value_type { };

template <typename T>
struct get_value_type<T, std::void_t<typename T::value_type>> : type_is<typename T::value_type> { };

template <typename T>
struct get_value_type<T&, std::void_t<typename T::value_type>> : type_is<typename T::value_type> { };

template <typename T, std::size_t N>
struct get_value_type<T[N]> : type_is<T> { };

template <typename T, std::size_t N>
struct get_value_type<T(&)[N]> : type_is<T> { };

template <typename T>
struct get_value_type<T[]> : type_is<T> { };

template <typename T>
struct get_value_type<T(&)[]> : type_is<T> { };



/* Alias templates */
template <typename T>
using type_is_t = typename type_is<T>::type;

template <typename T, typename... P0toN>
inline bool constexpr is_one_of_v = is_one_of<T, P0toN...>::value;

template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

template <typename T, typename U = void>
inline bool constexpr wraps_numeric_type_v = wraps_numeric_type<T,U>::value;

template <typename T, typename U = void>
inline bool constexpr wraps_integral_type_v = wraps_integral_type<T,U>::value;

template <typename T, typename U = void>
using get_value_type_t = typename get_value_type<T,U>::type;


#endif
