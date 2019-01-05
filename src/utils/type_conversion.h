/* Inspired by an excellent blog post by Jonathan Boccara found at
 * fluentcpp.com/2017/06/27/how-to-collapse-nested-switch-statements/ */
#ifndef FOLD_H
#define FOLD_H

#pragma once
#include "traits.h"
#include <cstddef>
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

template <typename Enum, typename... Rest>
struct enum_size : std::integral_constant<std::size_t, enum_value(Enum::End_) * enum_size<Rest...>::value> { };

template <typename Enum>
struct enum_size<Enum> : std::integral_constant<std::size_t, enum_value(Enum::End_)> { };

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

template <typename Enum, typename... Rest>
struct enum_fold {
	static_assert(std::is_enum_v<Enum> && (std::is_enum_v<Rest> && ...), "Cannot fold non-enum type");

	constexpr std::size_t operator()(Enum e, Rest... rest) {
		return enum_fold_impl<Enum, Rest...>::fold(e, rest...);
	}
};


#endif
