#include <iostream>
template <typename Bits>
Bitmask<Bits>::Bitmask() noexcept : current_{static_cast<value_type>(0)} { }

template <typename Bits>
Bitmask<Bits>::Bitmask(value_type val) noexcept : current_{val} { }

template <typename Bits>
Bitmask<Bits>::Bitmask(Bitmask const& other) noexcept : current_{other.current_} { }

template <typename Bits>
Bitmask<Bits>& Bitmask<Bits>::operator=(Bitmask const& other) & noexcept {
	current_ = other.current_;
	return *this;
}

template <typename Bits>
Bitmask<Bits>& Bitmask<Bits>::operator=(value_type value) & noexcept {
	current_ = value;
	return *this;
}

template <typename Bits>
Bitmask<Bits>::operator value_type() noexcept {
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
	using value_type = typename T::value_type;
	return static_cast<Bitmask<T>>(
				static_cast<value_type>(left) | 
				static_cast<value_type>(right));
}

template <typename T>
Bitmask<T> operator&(Bitmask<T> left, Bitmask<T> right) noexcept {
	using value_type = typename T::value_type;
	return static_cast<Bitmask<T>>(
				static_cast<value_type>(left) &
				static_cast<value_type>(right));
}

template <typename T>
Bitmask<T> operator^(Bitmask<T> left, Bitmask<T> right) noexcept {
	using value_type = typename T::value_type;
	return static_cast<Bitmask<T>>(
				static_cast<value_type>(left) ^
				static_cast<value_type>(right));
}

template <typename T>
Bitmask<T> operator~(Bitmask<T> mask) noexcept {
	using value_type = typename T::value_type;
	return static_cast<Bitmask<T>>(~static_cast<value_type>(mask));
}

template <typename T>
Bitmask<T>& operator|=(Bitmask<T>& left, Bitmask<T> right) {
	using value_type = typename T::value_type;
	left = static_cast<Bitmask<T>>(
				static_cast<value_type>(left) |
				static_cast<value_type>(right));
	return left;
}

template <typename T>
Bitmask<T>& operator&=(Bitmask<T>& left, Bitmask<T> right) {
	using value_type = typename T::value_type;
	left = static_cast<Bitmask<T>>(
				static_cast<value_type>(left) &
				static_cast<value_type>(right));
	return left;
}

template <typename T>
Bitmask<T>& operator^=(Bitmask<T>& left, Bitmask<T> right) {
	using value_type = typename T::value_type;
	left = static_cast<Bitmask<T>>(
				static_cast<value_type>(left) ^
				static_cast<value_type>(right));
	return left;
}
