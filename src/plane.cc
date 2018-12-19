#include "plane.h"

Plane::Plane(GLfloat x_len, GLfloat dx, GLfloat y_len, GLfloat dy) : vertices_{} {
	init(x_len, dx, y_len, dy);	
}

void Plane::init(GLfloat x_len, GLfloat dx, GLfloat y_len, GLfloat dy){
	/* Prevent potential overflow */

	glm::clamp(x_len, 0.05f, 1.f);
	glm::clamp(y_len, 0.05f, 1.f);
	glm::clamp(dx, .01f, x_len);
	glm::clamp(dy, .01f, y_len);

	GLuint x_iters = static_cast<GLuint>(x_len / dx) + 1;
	GLuint y_iters = static_cast<GLuint>(y_len / dy) + 1;

	vertices_.reserve(VERTEX_SIZE*x_iters*y_iters);
	
	GLfloat x_start = -static_cast<GLfloat>(x_len/2);
	GLfloat x = x_start;
	GLfloat y = -static_cast<GLfloat>(y_len/2);
	GLfloat s ,t;

	std::array<GLfloat, VERTEX_SIZE> vertex;
	auto idx = 0u;
	for(auto i = 0u; i < y_iters; i++, y += dy, x = x_start){
		t = interpolation::linear(static_cast<GLfloat>(i)/static_cast<GLfloat>(y_iters-1));

		for(auto j = 0u; j < x_iters; j++, x += dx, idx += VERTEX_SIZE){
			s = interpolation::linear(static_cast<GLfloat>(j)/static_cast<GLfloat>(x_iters-1));
			/* Position */
			vertex[0] = x;
			vertex[1] = y;
			vertex[2] = 0.f;
			/* Normal */
			vertex[3] = 0.f;
			vertex[4] = 0.f;	
			vertex[5] = 1.f;
			/* Texture */
			vertex[6] = s;
			vertex[7] = t;
		
			vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
		}
	} 

}

std::vector<GLfloat> const& Plane::vertices() const{
	return vertices_;
}
