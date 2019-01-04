#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "exception.h"
#include "render_constraints.h"
#include "traits.h"
#include <GL/glew.h>
#include <iostream>
#include <limits>
#include <type_traits>
#include <utility>

/* Inheriting (through CRTP) from Renderer will enable rendering (through the render() member function) for any type that fulfills the rendering criteria.
 * For a class T, the criteria are as follows:
 * 1. T must name public a function vertices() that returns a container that's stored contiguously in memory (std::vector, std::array or c-style array)
 * 	  and wraps a numeric type. CV and ref modifiers for the container are accepted, pointers to stl containers are not. Returning c-style dynamic arrays is valid.
 *		- If a dynamic c-style array is returned by vertices(), T must also name a public function vertices_size() that returns the size of the array returned by vertices() (an integral value).
 * 	  
 * 2. T must name a public function indices() that returns a container that's, again, stored contiguously but wraps an integral type.
 *    Cv and ref modifiers for the container are accepted, pointers to stl containers are not. Returning c-style dynamic arrays is valid.
 *      - If a dynamic c-style array is returned by indices(), T must also name a public function indices_size() that returns the size of the array returned by indices() (integral value).
 *
 * 3. T must name a public function init(Args...), taking any number of arguments. The only requirements for the function is that the object is completely constructed once the function returns.
 *    T::init should not be called explicitly in the constructor of T. Instead, the constructor of T should call Renderer<T>::init(Args&&...) and pass along all the arguments that T::init(Args...) 
 *    needs. Renderer<T>::init(Args&&...) will then call T::init(Args...) and forward the parameters it receives from the T constructor. This ensures that the objects are contructed in the correct order.
 *
 * Any attempt to inherit from Renderer with a class that does not fulfill these requirements will trigger a static assert in the Renderer contructor */

struct vertices_tag { };
struct indices_tag { };

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
	
		/* Tag dispatch */
		GLuint constexpr size(vertices_tag) const;
		GLuint constexpr size(indices_tag) const;
};

#include "renderer.tcc"
#endif
