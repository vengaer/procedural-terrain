#ifndef PLANE_H
#define PLANE_H

#pragma once
#include "exception.h"
#include "interpolation.h"
#include "renderer.h"
#include <cmath>
#include <cstddef>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <vector>


class Plane : public Renderer<Plane> {
	public:
		Plane(GLfloat x_len = 1.f, GLfloat dx = .5f, GLfloat z_len = 1.f, GLfloat dz = .5f);
		
		std::vector<GLfloat> const& vertices() const;
		std::vector<GLuint> const& indices() const;
	
	private:
		std::vector<GLfloat> vertices_;	
		std::vector<GLuint> indices_;
		
		void init(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz);
};

#endif
