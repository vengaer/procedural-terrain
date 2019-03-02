#ifndef COLOR_TYPE_H
#define COLOR_TYPE_H

#pragma once
#include <array>
#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
struct ColorType {
	ColorType() noexcept;
	ColorType(T all) noexcept;
	ColorType(T p, T q, T r, T s) noexcept;
	ColorType(std::array<T, 4> const& arr) noexcept;
	
	union{ T x, r; };
	union{ T y, g; };
	union{ T z, b; };
	union{ T w, a; };
};

#include "color_type.tcc"
#endif
