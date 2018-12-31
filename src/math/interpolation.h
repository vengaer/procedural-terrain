#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#pragma once
#include "meta_utils.h"
#include <cmath>
#include <glm/glm.hpp>
#include <type_traits>


namespace interpolation{
	inline double constexpr PI = 3.14159265359;
	
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>>>
	struct Parameters {
		std::decay_t<T> x;
		std::decay_t<T> y;
	};

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>>>
	std::decay_t<T> linear(T t, T min = static_cast<T>(0.0), T max = static_cast<T>(1.0));

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>>>
	Parameters<T> polar(T angle, T radius = static_cast<T>(1.0));
}

#include "interpolation.tcc"

#endif
