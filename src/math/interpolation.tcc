
template<typename T, typename>
std::decay_t<T> interpolation::linear(T x, T min, T max){
	return (1-x)*min + x*max;
}

template <typename T, typename>
std::decay_t<T> interpolation::cubic(T x, T p0, T p1, T p2, T p3) {
    return (-0.5 * p0 + 1.5 * p1 - 1.5 * p2 + 0.5 * p3) * std::pow(x, 3.0) +
           (p0 - 2.5 * p1 + 2 * p2 - 0.5 * p3) * std::pow(x, 2.0) + 
           (-0.5 * p0 + 0.5 * p2) * x + p1;
}

template <typename T, typename>
interpolation::Parameters<T> interpolation::polar(T angle, T radius){
	Parameters<T> result;
	result.x = radius * std::cos(angle);
	result.y = radius * std::sin(angle);

	return result;
}

template <typename T, typename>
std::decay_t<T> interpolation::cosine(T x, T min, T max) {
    using math::constants::PI;

    T mu = static_cast<T>(1.0 - std::cos(x*PI)) / static_cast<T>(2.0);
    return linear(mu, min, max);
}

