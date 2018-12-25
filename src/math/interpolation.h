#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <type_traits>


namespace interpolation{
	inline double constexpr PI = 3.14159265359;

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	struct Parameters {
		T x;
		T y;
	};
	
	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	inline T linear(T t, T min = static_cast<T>(0.0), T max = static_cast<T>(1.0)){
		glm::clamp(t, min, max);
		
		return (1-t)*min + t*max;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	inline Parameters<T> polar(T angle, T radius = static_cast<T>(1.0)){
		angle = std::fmod(angle, 2*PI);

		Parameters<T> result;
		result.x = radius * std::cos(angle);
		result.y = radius * std::sin(angle);
	
		return result;
	}
}

#endif
