#include "canvas.h"

Canvas::Canvas() {
	Renderer<Canvas>::init();
}

std::array<GLfloat, Canvas::VERTEX_ARRAY_SIZE> const& Canvas::vertices() const {
	return vertices_;
}

std::array<GLuint, 6u> const& Canvas::indices() const {
	return indices_;
}

std::array<GLfloat, Canvas::VERTEX_ARRAY_SIZE> const Canvas::vertices_{
	-1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
	 1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,
	 1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
	-1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f
};

std::array<GLuint, 6u> const Canvas::indices_ {
	0u, 1u, 3u,
	1u, 2u, 3u
};
