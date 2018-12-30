#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "meta_utils.h"
#include <GL/glew.h>
#include <iostream>
#include <type_traits>
#include <utility>

// TODO: Deal with c-style dynamic arrays in Renderer<T>::init

/* Inheriting (through CRTP) from Renderer will enable rendering (through the render() member function) for any type that fulfills the rendering criteria.
 * For a class T, the criteria are as follows:
 * 1. T must name public a function vertices() that returns a container that's stored contiguously in memory (std::vector, std::array or c array)
 * 	  and wraps a numeric type (std::is_arithmetic_v<typename T::value_type> == true or similarily). CV and ref modifiers for the container are accepted, pointers are not.
 * 	  
 * 2. T must name a public function indices() that returns a container that's, again, stored contiguously but wraps an integral type 
 *    (std::is_integral_v<typename T::value_type> == true, or similarly). Cv and ref modifiers for the container are accepted, pointers are not. 
 *
 * 3. T must name a public function init(Args...), taking any number of arguments. The only requirements for the function is that the object needs to be completely constructed once the function returns.
 *    T::init should not be called explicitly in the constructor of T. Instead, the constructor of T should call Renderer<T>::init(Args&&...) and pass along all the arguments that T::init(Args...) 
 *    needs. Renderer<T>::init(Args&&...) will then call T::init(Args...) and forward the parameters it receives from the T constructor. This ensures that the objects are contructed in the correct order.
 *
 * Any attempt to inherit from Renderer with a class that does not fulfill these requirements will trigger a static assert in the Renderer contructor */

namespace {	
	template <typename T, typename = void>
	struct has_arbitrary_init : std::false_type { };

	template <typename T>
	struct has_arbitrary_init<T, std::void_t<decltype(&T::init)>> : std::true_type { };

	template <typename T>
	inline bool constexpr has_arbitrary_init_v = has_arbitrary_init<T>::value;


	template <typename T, typename = void>
	struct has_vertices_size : std::false_type { };

	template <typename T>
	struct has_vertices_size<T, std::void_t<decltype(std::declval<T>().vertices_size())>> : std::true_type { };

	template <typename T, typename U = void>
	inline bool constexpr has_vertices_size_v = has_vertices_size<T,U>::value;


	template <typename T, typename = void>
	struct has_indices_size : std::false_type { };

	template <typename T>
	struct has_indices_size<T, std::void_t<decltype(std::declval<T>().indices_size())>> : std::true_type { };

	template <typename T, typename U = void>
	inline bool constexpr has_indices_size_v = has_indices_size<T,U>::value;


	template <typename T>
	struct is_renderable{
		private:
			using try_get_vertices_t = decltype(std::declval<T>().vertices());
			using try_get_indices_t = decltype(std::declval<T>().indices());
			
		public:
			static bool constexpr value = (is_contiguously_stored_v<try_get_vertices_t> || (std::is_pointer_v<try_get_vertices_t> && has_vertices_size_v<T>)) && 
										  wraps_numeric_type_v<try_get_vertices_t> &&
										  (is_contiguously_stored_v<try_get_indices_t> || (std::is_pointer_v<try_get_indices_t> && has_indices_size_v<T>))  && 
										  wraps_integral_type_v<try_get_indices_t> &&
										  has_arbitrary_init<T>::value;
	};

	template <typename T>
	inline bool constexpr is_renderable_v = is_renderable<T>::value;

}

template <typename T>
class Renderer {
	public:
		void render() const;
		
		static GLuint constexpr VERTEX_SIZE = 8u;

	protected:
		Renderer();

		template <typename... Args>
		void init(Args&&... args);
	private:
		GLuint vao_, vbo_;
		GLuint idx_buffer_;
		GLuint idx_size_;
};

#include "renderer.tcc"
#endif
