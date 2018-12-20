#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "meta_utils.h"
#include <GL/glew.h>
#include <iostream>
#include <type_traits>
#include <utility>

template <typename T>
struct is_renderable{
	private:
		using try_get_vertices_t = decltype(std::declval<T>().vertices());
		using try_get_indices_t = decltype(std::declval<T>().indices());

	public:
		static bool constexpr value = is_contiguously_stored_v<try_get_vertices_t> && wraps_numeric_type_v<try_get_vertices_t> &&
									  is_contiguously_stored_v<try_get_indices_t>  && wraps_numeric_type_v<try_get_indices_t>;
};

template <typename T>
inline bool constexpr is_renderable_v = is_renderable<T>::value;

template <typename T>
class Renderer {
	public:
		/* TODO: make protected */
		Renderer() {
			static_assert(is_renderable_v<T>, "Type is not renderable\n");
		}
		static GLuint constexpr VERTEX_SIZE = 8u;

	protected:
	
	private:
};

#include "renderer.tcc"
#endif
