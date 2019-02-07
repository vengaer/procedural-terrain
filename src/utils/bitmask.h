#ifndef BITMASK_H
#define BITMASK_H

#pragma once
#include <type_traits>

template <typename Bits>
class Bitmask : public Bits {
	static_assert(std::is_aggregate_v<Bits>, "Template argument must be an aggregate type");
	static_assert(std::is_integral_v<typename Bits::value_type>, "Template argument must name an integral value_type alias");
	public:
		using value_type = typename Bits::value_type;
		Bitmask() noexcept;
		Bitmask(value_type val) noexcept;
		
		Bitmask(Bitmask const& other) noexcept;
		Bitmask& operator=(Bitmask const& other) & noexcept;

		operator value_type() noexcept;

		bool operator==(Bitmask const& other) const noexcept;
		bool operator!=(Bitmask const& other) const noexcept;

		template <typename T>
		friend Bitmask<T> operator|(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator&(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator^(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator~(Bitmask<T> mask) noexcept;
		template <typename T>
		friend Bitmask<T>& operator|=(Bitmask<T>& left, Bitmask<T> right);
		template <typename T>
		friend Bitmask<T>& operator&=(Bitmask<T> left, Bitmask<T> right);
		template <typename T>
		friend Bitmask<T>& operator^=(Bitmask<T> left, Bitmask<T> right);

	private:
		value_type current_{};
};

#include "bitmask.tcc"
#endif
