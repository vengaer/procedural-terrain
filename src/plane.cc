#include "plane.h"

Plane::Plane(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz) : vertices_{} {
	init(x_len, dx, z_len, dz);	
}

void Plane::init(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz){
	/* Prevent potential overflow */
	glm::clamp(x_len, 0.05f, 1.f);
	glm::clamp(z_len, 0.05f, 1.f);
	glm::clamp(dx, .01f, x_len);
	glm::clamp(dz, .01f, z_len);

	GLuint x_iters = static_cast<GLuint>(x_len / dx) + 1;
	GLuint z_iters = static_cast<GLuint>(z_len / dz) + 1;

	vertices_.reserve(VERTEX_SIZE*x_iters*z_iters);
	
	GLfloat x_start = -static_cast<GLfloat>(x_len/2);
	GLfloat x = x_start;
	GLfloat z = -static_cast<GLfloat>(z_len/2);
	GLfloat s ,t;

	std::array<GLfloat, VERTEX_SIZE> vertex;
	auto idx = 0u;
	for(auto i = 0u; i < z_iters; i++, z += dz, x = x_start){
		t = interpolation::linear(static_cast<GLfloat>(i)/static_cast<GLfloat>(z_iters-1));

		for(auto j = 0u; j < x_iters; j++, x += dx, idx += VERTEX_SIZE){
			s = interpolation::linear(static_cast<GLfloat>(j)/static_cast<GLfloat>(x_iters-1));
			/* Position */
			vertex[0] = x;
			vertex[1] = 0.f;
			vertex[2] = z;
			/* Normal */
			vertex[3] = 0.f;
			vertex[4] = 1.f;	
			vertex[5] = 0.f;
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
