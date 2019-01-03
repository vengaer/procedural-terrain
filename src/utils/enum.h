/* Inspired by an excellent blog post by Jonathan Boccara found at
 * fluentcpp.com/2017/06/27/how-to-collapse-nested-switch-statements/ */
#ifndef ENUM_H
#define ENUM_H

#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace {

	template <typename Enum, 
			  typename Integral = std::underlying_type_t<Enum>, 
			  typename = std::enable_if_t<std::is_enum_v<Enum> && std::is_integral_v<Integral>>>
	constexpr Integral enum_value(Enum e) {
		return static_cast<Integral>(e);
	}

	template <typename Enum,
			  typename Integral = std::underlying_type_t<Enum>,
			  typename = std::enable_if_t<std::is_enum_v<Enum> && std::is_convertible_v<Integral, std::underlying_type_t<Enum>>>>
	constexpr Enum enum_cast(Integral value) {
		return static_cast<Enum>(value);
	}

	template <typename Enum, typename... Rest>
	struct enum_size : std::integral_constant<std::size_t, enum_value(Enum::End_) * enum_size<Rest...>::value> { };

	template <typename Enum>
	struct enum_size<Enum> : std::integral_constant<std::size_t, enum_value(Enum::End_)> { };

	template <typename Enum, typename... Rest>
	inline std::size_t constexpr enum_size_v = enum_size<Enum, Rest...>::value;

	template <typename Enum, typename... Rest>
	struct fold_enums_impl {
		static constexpr std::size_t fold(Enum e, Rest... rest) {
			return enum_value(e) * enum_size_v<Rest...> + fold_enums_impl<Rest...>::fold(rest...);
		}
	};

	template <typename Enum>
	struct fold_enums_impl<Enum> {
		static constexpr std::size_t fold(Enum e) {
			return enum_value(e);
		}
	};

	template <typename Enum, typename... Rest>
	struct fold_enums {
		static_assert(std::is_enum_v<Enum> && (std::is_enum_v<Rest> && ...), "Cannot fold non-enum type");

		constexpr std::size_t operator()(Enum e, Rest... rest) {
			return fold_enums_impl<Enum, Rest...>::fold(e, rest...);
		}
	};

	struct folder {
		template <typename... Args>
		static constexpr std::size_t deduce(Args&&... args) {
			fold_enums<Args...> fold;
			return fold(std::forward<Args>(args)...);
		}
	};

}

#endif
