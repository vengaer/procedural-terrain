
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>>>
std::decay_t<T> interpolation::linear(T t, T min, T max){
	glm::clamp(t, min, max);
	
	return (1-t)*min + t*max;
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>>>
interpolation::Parameters<T> interpolation::polar(T angle, T radius){
	Parameters<T> result;
	result.x = radius * std::cos(angle);
	result.y = radius * std::sin(angle);

	return result;
}
