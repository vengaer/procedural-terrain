#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "exception.h"
#include "render_constraints.h"
#include "shader.h"
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

struct automatic_shader_activation_tag { };
struct manual_shader_activation_tag { };

/* TODO: Fix the design dilemma and add ctors that take shaders in derived classes */
/* TODO: Activate face culling */
template <typename T>
struct render_helper { };

template <>
struct render_helper<manual_shader_activation_tag> {
	void operator()(GLuint vao, GLuint idx_size, GLuint) const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, idx_size, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
};

template <>
struct render_helper<automatic_shader_activation_tag> {
	void operator()(GLuint vao, GLuint idx_size, GLuint shader_program) const {
		Shader::enable(shader_program);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, idx_size, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
};

template <typename T, typename ShaderPolicy = manual_shader_activation_tag>
class Renderer {
	public:
		void render() const;
	
		static GLuint constexpr VERTEX_SIZE = 8u;
	protected:
		Renderer(GLuint shader_id = 0u);

		template <typename... Args>
		void init(Args&&... args);
	private:
		GLuint vao_, vbo_;
		GLuint idx_buffer_;
		GLuint idx_size_;
		GLuint shader_id_;
		render_helper<ShaderPolicy> helper_;
	
		/* Tag dispatch */
		GLuint constexpr size(vertices_tag) const;
		GLuint constexpr size(indices_tag) const;
};

#include "renderer.tcc"
#endif
