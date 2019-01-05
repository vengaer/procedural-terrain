#ifndef TYPE_CONVERSION_H
#define TYPE_CONVERSION_H 
#pragma once
#include "exception.h"
#include "traits.h"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

template <typename Enum, 
		  typename Numeric = std::underlying_type_t<Enum>, 
		  typename = std::enable_if_t<std::is_enum_v<Enum> && std::is_arithmetic_v<Numeric>>>
Numeric constexpr enum_value(Enum e) {
	return static_cast<Numeric>(e);
}

template <typename Enum,
		  typename Integral,
		  typename = std::enable_if_t<std::is_enum_v<Enum> && 
					 std::is_integral_v<remove_cvref_t<Integral>> &&
					 std::is_convertible_v<Integral, std::underlying_type_t<Enum>>>>
Enum constexpr enum_cast(Integral value) {
	return static_cast<Enum>(value);
}

template <typename T, typename F, typename = std::enable_if_t<std::is_convertible_v<T,F>>>
T safe_cast(F from) {
	if(std::numeric_limits<T>::max() < from) {
		from = std::numeric_limits<T>::max();
		std::cerr << "Overflow caught, clamping value\n";
	}
	if(std::numeric_limits<T>::lowest() > from){
		from = std::numeric_limits<T>::lowest();
		std::cerr << "Underflow caught, clamping value\n";
	}

	return static_cast<T>(from);
}

template <typename Enum, typename... Rest>
struct enum_size : size_t_constant<enum_value(Enum::End_) * enum_size<Rest...>::value> { };

template <typename Enum>
struct enum_size<Enum> : size_t_constant<enum_value(Enum::End_)> { };

template <typename Enum, typename... Rest>
inline std::size_t constexpr enum_size_v = enum_size<Enum, Rest...>::value;

template <typename Enum, typename... Rest>
struct enum_fold_impl {
	static std::size_t constexpr fold(Enum e, Rest... rest) {
		return enum_value(e) * enum_size_v<Rest...> + enum_fold_impl<Rest...>::fold(rest...);
	}
};

template <typename Enum>
struct enum_fold_impl<Enum> {
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
struct enum_fold {
	static_assert(std::is_enum_v<Enum> && (std::is_enum_v<Rest> && ...), "Cannot fold non-enum type");

	constexpr std::size_t operator()(Enum e, Rest... rest) {
		return enum_fold_impl<Enum, Rest...>::fold(e, rest...);
	}
};


#endif
