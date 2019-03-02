#ifndef CANVAS_H
#define CANVAS_H

#pragma once
#include "color_type.h"
#include "renderer.h"
#include "shader.h"
#include "shader_handler.h"
#include <GL/glew.h>
#include <array>

template <typename ShaderPolicy = manual_shader_handler>
class Canvas : public Renderer<Canvas<ShaderPolicy>, ShaderPolicy> {
	static GLuint constexpr VERTEX_SIZE = Renderer<Canvas<ShaderPolicy>, ShaderPolicy>::VERTEX_SIZE;
	static GLuint constexpr VERTEX_ARRAY_SIZE{4u * VERTEX_SIZE};
	static GLuint constexpr INDICES_ARRAY_SIZE{6u};
	
	using renderer_t = Renderer<Canvas<ShaderPolicy>, ShaderPolicy>;
	public:
		using Color = ColorType<float>;

		Canvas(Color clear_color = Color{1.f, 1.f, 1.f, 1.f});
		Canvas(ShaderPolicy policy, Color clear_color = Color{1.f, 1.f, 1.f, 1.f});

		void render_setup() const;
		void render_cleanup() const;
	
		std::array<GLfloat, VERTEX_ARRAY_SIZE> const& vertices() const;
		std::array<GLuint, INDICES_ARRAY_SIZE> const& indices() const;

	private:
		Color clear_color_;

		static std::array<GLfloat, VERTEX_ARRAY_SIZE> const vertices_;
		static std::array<GLuint, INDICES_ARRAY_SIZE> const indices_;
};

#include "canvas.tcc"
#endif
