#ifndef CANVAS_H
#define CANVAS_H

#pragma once
#include "renderer.h"
#include <GL/glew.h>
#include <array>

class Canvas : public Renderer<Canvas> {
	static GLuint constexpr VERTEX_ARRAY_SIZE{4u * VERTEX_SIZE};
	public:
		Canvas();
	
		std::array<GLfloat, VERTEX_ARRAY_SIZE> const& vertices() const;
		std::array<GLuint, 6u> const& indices() const;

	private:
		static std::array<GLfloat, VERTEX_ARRAY_SIZE> const vertices_;
		static std::array<GLuint, 6u> const indices_;
		
};

#endif
