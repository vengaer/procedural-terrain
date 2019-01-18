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
	if(std::numeric_limits<T>::max() < from) 
		throw OverflowException{"Overflow during cast\n"};
	if(std::numeric_limits<T>::lowest() > from)
		throw UnderflowException{"Underflow during cast\n"};

	return static_cast<T>(from);
}
#endif
