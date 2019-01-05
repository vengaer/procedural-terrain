#ifndef UNIFORM_IMPL_H
#define UNIFORM_IMPL_H

#pragma once
#include "traits.h"
#include "type_conversion.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <type_traits>


namespace uniform {
	enum class TensorOrder { _0th, _1st, _2nd, End_ };
	enum class DecayType { Float, Int, Uint, FloatPtr, IntPtr, UintPtr, End_ };
	enum class TensorDimensions { _1 = 1, _2, _3, _4, _2x3, _3x2, _2x4, _4x2, _3x4, _4x3, End_ };

	template <typename, typename = void>
	struct order { };

	template <typename T>
	struct order<T, std::enable_if_t<std::is_arithmetic_v<T>>> : size_t_constant<0u> { };

	template <typename T>
	struct order<T, std::enable_if_t<is_one_of_v<T, glm::vec2,
												    glm::vec3,
												    glm::vec4,
												    glm::dvec2,
												    glm::dvec3,
												    glm::dvec4,
												    glm::ivec2,
												    glm::ivec3,
												    glm::ivec4,
												    glm::uvec2,
												    glm::uvec3,
												    glm::uvec4>>
			> : size_t_constant<1u> { };

	template <typename T>
	struct order<T, std::enable_if_t<is_one_of_v<T, glm::mat2,
													glm::mat3,
													glm::mat4,
													glm::mat2x3,
													glm::mat3x2,
													glm::mat2x4,
													glm::mat4x2,
													glm::mat3x4,
													glm::mat4x3,
													glm::dmat2,
													glm::dmat3,
													glm::dmat4,
													glm::dmat2x3,
													glm::dmat3x2,
													glm::dmat2x4,
													glm::dmat4x2,
													glm::dmat3x4,
													glm::dmat4x3>>
			> : size_t_constant<2u> { };

	template <typename T>
	inline std::size_t constexpr order_v = order<T>::value;

	template <typename, typename = void>
	struct dimensions { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<std::is_arithmetic_v<T>>> : size_t_constant<1u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::vec2,
												         glm::dvec2,
												         glm::ivec2,
												         glm::uvec2,
												     	 glm::mat2,
												     	 glm::dmat2>>
					> : size_t_constant<2u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::vec3,
													     glm::dvec3,
													     glm::ivec3,
													     glm::uvec3,
													     glm::mat3,
													     glm::dmat3>>
					> : size_t_constant<3u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::vec4,
													     glm::dvec4,
													     glm::ivec4,
													     glm::uvec4,
													     glm::mat4,
													     glm::dmat4>>
					> : size_t_constant<4u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat2x3,
														 glm::dmat2x3>>
					> : size_t_constant<5u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat3x2,
														 glm::dmat3x2>>
					> : size_t_constant<6u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat2x4,
														 glm::dmat2x4>>
					> : size_t_constant<7u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat4x2,
														 glm::mat4x2>>
					> : size_t_constant<8u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat3x4,
														 glm::dmat3x4>>
					> : size_t_constant<9u> { };

	template <typename T>
	struct dimensions<T, std::enable_if_t<is_one_of_v<T, glm::mat4x3,
														 glm::dmat4x3>>
					> : size_t_constant<10u> { };

	template <typename T>
	inline std::size_t constexpr dimensions_v = dimensions<T>::value;


	template <typename... Args>
	struct deduce {
		private:
			using first_t = nth_type_t<0, Args...>;
			using first_unqualified_t = remove_cvref_t<remove_cvptr_t<first_t>>;
		public:
			static TensorOrder constexpr tensor_order() {
				/* Muliple function parameters  => vector */
				if constexpr(sizeof...(Args) > 1)
					return TensorOrder::_1st;

				/* Scalars or glm constructs */
				return enum_cast<TensorOrder>(order_v<first_unqualified_t>);
			}
			static DecayType constexpr decay_type() {
				/* int, uint or float */
				if constexpr(sizeof...(Args) > 1 || std::is_fundamental_v<first_t>){
					if constexpr(std::is_integral_v<first_t>){
						if constexpr(std::is_unsigned_v<first_t>)
							return DecayType::Uint;	
						else
							return DecayType::Int;
					}
					else
						return DecayType::Float;
				}
				
				/* int*, uint* or float* */
				using fundamental_t = fundamental_type_t<first_t>;	
				
				if constexpr(std::is_integral_v<fundamental_t>){
					if constexpr(std::is_unsigned_v<fundamental_t>)
						return DecayType::UintPtr;
					else
						return DecayType::IntPtr;
				}

				return DecayType::FloatPtr;
			}
			static TensorDimensions constexpr tensor_dimensions() {

				/* Mulitple parameters passed to function => dim is number of parameters */
				if constexpr(sizeof...(Args) > 1)
					return enum_cast<TensorDimensions>(sizeof...(Args));

				/* Single components or glm constructs */
				return enum_cast<TensorDimensions>(dimensions_v<first_unqualified_t>);
			}
	};
}

#endif
