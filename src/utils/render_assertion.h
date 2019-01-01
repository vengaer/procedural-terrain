#ifndef RENDER_ASSERTION_H
#define RENDER_ASSERTION_H

#pragma once
#include "meta_utils.h"
#include <type_traits>
#include <utility>

namespace {
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
}

template <typename, typename = void, typename = void>
struct is_renderable : std::false_type { };

template <typename T>
struct is_renderable<T, std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().indices())>> {
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

		static bool constexpr vertices_is_applicable_stl_container = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t>;
		static bool constexpr indices_is_applicable_stl_container = is_contiguously_stored_v<try_get_indices_t> && wraps_integral_type_v<try_get_indices_t>;

	public:
		static bool constexpr value = vertices_is_applicable_stl_container && indices_is_applicable_stl_container && has_arbitrary_init_v<T>;
};


template <typename T>
struct is_renderable<T, 
			std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().indices())>, 
			std::enable_if_t<has_vertices_size_v<T> && !has_indices_size_v<T>>> {
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

		using try_get_vertices_size_t = decltype(std::declval<T>().vertices_size());

		static bool constexpr vertices_is_applicable_stl_container = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t>;
		static bool constexpr indices_is_applicable_stl_container = is_contiguously_stored_v<try_get_indices_t> && wraps_integral_type_v<try_get_indices_t>;

		static bool constexpr vertices_is_applicable_c_array = std::is_pointer_v<try_get_vertices_t> && std::is_arithmetic_v<get_fundamental_type_t<try_get_vertices_t>> &&
															   has_vertices_size_v<T> && std::is_integral_v<get_fundamental_type_t<try_get_vertices_size_t>>;
	public:
		static bool constexpr value = (vertices_is_applicable_stl_container || vertices_is_applicable_c_array) && indices_is_applicable_stl_container && has_arbitrary_init_v<T>;
};

template <typename T>
struct is_renderable<T, 
			std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().indices())>, 
			std::enable_if_t<!has_vertices_size_v<T> && has_indices_size_v<T>>> {
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

		using try_get_indices_size_t = decltype(std::declval<T>().indices_size());

		static bool constexpr vertices_is_applicable_stl_container = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t>;
		static bool constexpr indices_is_applicable_stl_container = is_contiguously_stored_v<try_get_indices_t> && wraps_integral_type_v<try_get_indices_t>;

		static bool constexpr indices_is_applicable_c_array = std::is_pointer_v<try_get_indices_t> && std::is_integral_v<get_fundamental_type_t<try_get_indices_t>> &&
															   has_indices_size_v<T> && std::is_integral_v<get_fundamental_type_t<try_get_indices_size_t>>;	
	public:
		static bool constexpr value = vertices_is_applicable_stl_container && (indices_is_applicable_stl_container || indices_is_applicable_c_array) && has_arbitrary_init_v<T>;
};

template <typename T>
struct is_renderable<T, 
			std::void_t<decltype(std::declval<T>().vertices()), decltype(std::declval<T>().indices())>, 
			std::enable_if_t<has_vertices_size_v<T> && has_indices_size_v<T>>> {
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

		using try_get_vertices_size_t = decltype(std::declval<T>().vertices_size());
		using try_get_indices_size_t = decltype(std::declval<T>().indices_size());

		static bool constexpr vertices_is_applicable_stl_container = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t>;
		static bool constexpr indices_is_applicable_stl_container = is_contiguously_stored_v<try_get_indices_t> && wraps_integral_type_v<try_get_indices_t>;

		static bool constexpr vertices_is_applicable_c_array = std::is_pointer_v<try_get_vertices_t> && std::is_arithmetic_v<get_fundamental_type_t<try_get_vertices_t>> &&
															   has_vertices_size_v<T> && std::is_integral_v<get_fundamental_type_t<try_get_vertices_size_t>>;
		static bool constexpr indices_is_applicable_c_array = std::is_pointer_v<try_get_indices_t> && std::is_integral_v<get_fundamental_type_t<try_get_indices_t>> &&
															   has_indices_size_v<T> && std::is_integral_v<get_fundamental_type_t<try_get_indices_size_t>>;	
	public:
		static bool constexpr value = (vertices_is_applicable_stl_container || vertices_is_applicable_c_array) &&
									  (indices_is_applicable_stl_container || indices_is_applicable_c_array) && 
									   has_arbitrary_init_v<T>;
};

template <typename T>
inline bool constexpr is_renderable_v = is_renderable<T>::value;

#endif
