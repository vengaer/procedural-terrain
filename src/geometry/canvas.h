#ifndef CANVAS_H
#define CANVAS_H

#pragma once
#include "renderer.h"
#include "shader.h"
#include "shader_handler.h"
#include <array>
#include <GL/glew.h>
#include <vector>

template <typename ShaderPolicy = manual_shader_handler>
class Canvas : public Renderer<Canvas<ShaderPolicy>, ShaderPolicy> {
	static GLuint constexpr VERTEX_SIZE = Renderer<Canvas<ShaderPolicy>, ShaderPolicy>::VERTEX_SIZE;
	static GLuint constexpr VERTEX_ARRAY_SIZE{4u * VERTEX_SIZE};
	static GLuint constexpr INDICES_ARRAY_SIZE{6u};
	
	using renderer_t = Renderer<Canvas<ShaderPolicy>, ShaderPolicy>;
	public:
		Canvas(ShaderPolicy policy = {});
	
		std::array<GLfloat, VERTEX_ARRAY_SIZE> const& vertices() const;
		std::array<GLuint, INDICES_ARRAY_SIZE> const& indices() const;

	private:

		static std::array<GLfloat, VERTEX_ARRAY_SIZE> const vertices_;
		static std::array<GLuint, INDICES_ARRAY_SIZE> const indices_;

        friend class PostProcessing;
};

#include "canvas.tcc"
#endif
