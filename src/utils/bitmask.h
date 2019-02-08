#ifndef BITMASK_H
#define BITMASK_H

#pragma once
#include "traits.h"
#include <type_traits>

/* Example usage:
 * struct Bits {
 * 		using value_type = std::size_t;
 * 		static value_type constexpr log_errors   = 0x1;
 * 		static value_type constexpr log_warnings = 0x2;
 * 		static value_type constexpr log_loopback = 0x4;
 * 		// However many other "enumerators" are needed...
 * };
 *
 * void foo() {
 * 		using Mask = Bitmask<Bits>;
 * 		Mask log_mask = Mask::log_errors | Mask::log_warnings;
 * 		if((log_mask & Mask::log_errors) == Mask::log_errors)
 * 			// Errors are being logged
 * 		// ...
 * }
 * */
 /*
 * The template argument must name a nested type alias for an integral type called value_type
 * It's up to the user to define the bits for the options correctly (e.g. static value_type constexpr log_verbose = 0x7; will break the mask)
 */

template <typename Bits>
class Bitmask : public Bits {
	static_assert(std::is_aggregate_v<Bits>, "Template argument must be an aggregate type");
	static_assert(std::is_integral_v<typename Bits::value_type>, "Template argument must name an integral value_type alias");
	public:
		using value_type = typename Bits::value_type;
		using internal_type = promote_t<value_type>;

		Bitmask() noexcept;

		Bitmask(internal_type&& val) noexcept; /* internal_type only */
		
		template <typename T, 
				  typename = std::enable_if_t<std::is_integral_v<remove_cvref_t<T>> && 
											  std::is_const_v<std::remove_reference_t<T>>>>
		Bitmask(T&& val) noexcept; /* const integral type */

		template <typename T>
		explicit Bitmask(T&& val) noexcept; /* Other types require explicit conversion */

		
		Bitmask(Bitmask const& other) noexcept;
		Bitmask& operator=(Bitmask const& other) & noexcept;
		Bitmask& operator=(internal_type value) & noexcept;

		explicit operator internal_type() noexcept;

		bool operator==(Bitmask const& other) const noexcept;
		bool operator!=(Bitmask const& other) const noexcept;

		template <typename T>
		friend Bitmask<T> operator|(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator|(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept;
		template <typename T>
		friend Bitmask<T> operator&(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator&(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept;
		template <typename T>
		friend Bitmask<T> operator^(Bitmask<T> left, Bitmask<T> right) noexcept;
		template <typename T>
		friend Bitmask<T> operator^(Bitmask<T> left, typename Bitmask<T>::internal_type right) noexcept;
		template <typename T>
		friend Bitmask<T> operator~(Bitmask<T> mask) noexcept;
		template <typename T>
		friend Bitmask<T>& operator|=(Bitmask<T>& left, Bitmask<T> right);
		template <typename T>
		friend Bitmask<T>& operator|=(Bitmask<T>& left, typename Bitmask<T>::internal_type right);
		template <typename T>
		friend Bitmask<T>& operator&=(Bitmask<T> left, Bitmask<T> right);
		template <typename T>
		friend Bitmask<T>& operator&=(Bitmask<T> left, typename Bitmask<T>::internal_type right);
		template <typename T>
		friend Bitmask<T>& operator^=(Bitmask<T> left, Bitmask<T> right);
		template <typename T>
		friend Bitmask<T>& operator^=(Bitmask<T> left, typename Bitmask<T>::internal_type right);

	private:
		internal_type current_{};
};

#include "bitmask.tcc"
#endif
