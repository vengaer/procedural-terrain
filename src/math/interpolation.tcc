
template<typename T, typename>
std::decay_t<T> interpolation::linear(T t, T min, T max){
	t = glm::clamp(t, min, max);
	
	return (1-t)*min + t*max;
}

template <typename T, typename>
interpolation::Parameters<T> interpolation::polar(T angle, T radius){
	Parameters<T> result;
	result.x = radius * std::cos(angle);
	result.y = radius * std::sin(angle);

	return result;
}
