#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#pragma once
#include <glm/glm.hpp>
#include <type_traits>

namespace interpolation{
	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	inline T linear(T t, T min = static_cast<T>(0.0), T max = static_cast<T>(1.0)){
		glm::clamp(t, min, max);
		
		return (1-t)*min + t*max;
	}
}

#endif
