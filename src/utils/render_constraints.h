#ifndef RENDER_CONSTRAINTS_H
#define RENDER_CONSTRAINTS_H

#pragma once
#include "traits.h"
#include <type_traits>
#include <utility>

template <typename, typename = void>
struct has_arbitrary_init : std::false_type { };

template <typename T>
struct has_arbitrary_init<T, std::void_t<decltype(&T::init)>> : std::true_type { };

template <typename T>
inline bool constexpr has_arbitrary_init_v = has_arbitrary_init<T>::value;

template <typename, typename = void>
struct has_vertices_size : std::false_type { };

template <typename T>
struct has_vertices_size<T, std::void_t<decltype(std::declval<T>().vertices_size())>> : std::true_type { };

template <typename T, typename U = void>
inline bool constexpr has_vertices_size_v = has_vertices_size<T,U>::value;

template <typename, typename = void>
struct has_indices_size : std::false_type { };

template <typename T>
struct has_indices_size<T, std::void_t<decltype(std::declval<T>().indices_size())>> : std::true_type { };

template <typename T, typename U = void>
inline bool constexpr has_indices_size_v = has_indices_size<T,U>::value;

template <typename, typename = void>
struct vertices_is_applicable_c_array : std::false_type { };

template  <typename T>
struct vertices_is_applicable_c_array<T, std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().vertices_size())>> 
: std::bool_constant<std::is_pointer_v<decltype(std::declval<T>().vertices())> && 
					 std::is_arithmetic_v<fundamental_type_t<decltype(std::declval<T>().vertices())>> && 
					 std::is_integral_v<remove_cvref_t<decltype(std::declval<T>().vertices_size())>>> { };

template <typename T>
inline bool constexpr vertices_is_applicable_c_array_v = vertices_is_applicable_c_array<T>::value;

template <typename, typename = void>
struct indices_is_applicable_c_array : std::false_type { };

template <typename T>
struct indices_is_applicable_c_array<T, std::void_t<decltype(std::declval<T>().indices()), decltype(std::declval<T>().indices_size())>>
: std::bool_constant<std::is_pointer_v<decltype(std::declval<T>().indices_size())> && 
					 std::is_arithmetic_v<fundamental_type_t<decltype(std::declval<T>().indices_size())>> &&
					 std::is_integral_v<remove_cvref_t<decltype(std::declval<T>().indices_size())>>> { };

template <typename T>
inline bool constexpr indices_is_applicable_c_array_v = indices_is_applicable_c_array<T>::value;


template <typename, typename = void>
struct is_renderable_helper : std::false_type { };

template <typename T>
struct is_renderable_helper<T, std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().indices())>> {
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

		static bool constexpr vertices_is_applicable_stl_container = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t>;
		static bool constexpr indices_is_applicable_stl_container = is_contiguously_stored_v<try_get_indices_t> && wraps_integral_type_v<try_get_indices_t>;

	public:
		static bool constexpr value = (vertices_is_applicable_stl_container || vertices_is_applicable_c_array_v<T>) && 
									  (indices_is_applicable_stl_container || indices_is_applicable_c_array_v<T>);
};

template <typename T>
inline bool constexpr is_renderable_helper_v = is_renderable_helper<T>::value;

template <typename T>
struct is_renderable : std::bool_constant<is_renderable_helper_v<T>> { };

template <typename T>
inline bool constexpr is_renderable_v = is_renderable<T>::value;

#endif
