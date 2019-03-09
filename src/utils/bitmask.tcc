#include <iostream>
template <typename Bits>
Bitmask<Bits>::Bitmask() noexcept : current_{static_cast<internal_type>(0)} { }

template <typename Bits>
Bitmask<Bits>::Bitmask(internal_type val) noexcept : current_{val} { }

template <typename Bits>
template <typename T, typename>
Bitmask<Bits>::Bitmask(T const&& val) noexcept : current_{val} { 
	static_assert(std::is_integral_v<remove_cvref_t<T>>, "Invalid conversion from non-integral type to Bitmask<T>");
}

template <typename Bits>
template <typename T>
Bitmask<Bits>::Bitmask(T const&& val) noexcept : current_{val} { 
	static_assert(std::is_integral_v<remove_cvref_t<T>>, "Invalid conversion from non-integral type to Bitmask<T>");
}

template <typename Bits>
Bitmask<Bits>::Bitmask(Bitmask const& other) noexcept : current_{other.current_} { }

template <typename Bits>
Bitmask<Bits>& Bitmask<Bits>::operator=(Bitmask const& other) & noexcept {
	current_ = other.current_;
	return *this;
}

template <typename Bits>
Bitmask<Bits>& Bitmask<Bits>::operator=(internal_type value) & noexcept {
	current_ = value;
	return *this;
}

template <typename Bits>
Bitmask<Bits>::operator internal_type() noexcept {
	return current_;
}

template <typename Bits>
bool Bitmask<Bits>::operator==(Bitmask const& other) const noexcept {
	return current_ == other.current_;
}

template <typename Bits>
bool Bitmask<Bits>::operator!=(Bitmask const& other) const noexcept {
	return !(*this == other);
}

template <typename T>
Bitmask<T> operator|(Bitmask<T> left, Bitmask<T> right) noexcept {
	using internal_type = typename Bitmask<T>::internal_type;
	return static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) | 
				static_cast<internal_type>(right));
}

template <typename T>
Bitmask<T> operator|(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept {
	return left | static_cast<Bitmask<T>>(right);
}

template <typename T>
Bitmask<T> operator&(Bitmask<T> left, Bitmask<T> right) noexcept {
	using internal_type = typename Bitmask<T>::internal_type;
	return static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) &
				static_cast<internal_type>(right));
}

template <typename T>
Bitmask<T> operator&(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept {
	return left & static_cast<Bitmask<T>>(right);
}

template <typename T>
Bitmask<T> operator^(Bitmask<T> left, Bitmask<T> right) noexcept {
	using internal_type = typename Bitmask<T>::internal_type;
	return static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) ^
				static_cast<internal_type>(right));
}

template <typename T>
Bitmask<T> operator^(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept {
	return left ^ static_cast<Bitmask<T>>(right);
}

template <typename T>
Bitmask<T> operator~(Bitmask<T> mask) noexcept {
	using internal_type = typename Bitmask<T>::internal_type;
	return static_cast<Bitmask<T>>(~static_cast<internal_type>(mask));
}

template <typename T>
Bitmask<T>& operator|=(Bitmask<T>& left, Bitmask<T> right) {
	using internal_type = typename Bitmask<T>::internal_type;
	left = static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) |
				static_cast<internal_type>(right));
	return left;
}

template <typename T>
Bitmask<T>& operator|=(Bitmask<T>& left, typename Bitmask<T>::internal_type right) {
	return left |= static_cast<Bitmask<T>>(right);
}

template <typename T>
Bitmask<T>& operator&=(Bitmask<T>& left, Bitmask<T> right) {
	using internal_type = typename Bitmask<T>::internal_type;
	left = static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) &
				static_cast<internal_type>(right));
	return left;
}

template <typename T>
Bitmask<T>& operator&=(Bitmask<T>& left, typename Bitmask<T>::internal_type right) {
	return left &= static_cast<Bitmask<T>>(right);
}

template <typename T>
Bitmask<T>& operator^=(Bitmask<T>& left, Bitmask<T> right) {
	using internal_type = typename Bitmask<T>::internal_type;
	left = static_cast<Bitmask<T>>(
				static_cast<internal_type>(left) ^
				static_cast<internal_type>(right));
	return left;
}

template <typename T>
Bitmask<T>& operator^=(Bitmask<T>& left, typename Bitmask<T>::internal_type right) {
	return left ^= static_cast<Bitmask<T>>(right);
}
