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
		Plane(GLfloat x_len = 1.f, GLfloat dx = .2f, GLfloat y_len = 1.f, GLfloat dy = .2f);
		
		std::vector<GLfloat> const& vertices() const;
	
	private:
		std::vector<GLfloat> vertices_;	
		
		void init(GLfloat x_len, GLfloat dx, GLfloat y_len, GLfloat dy);


};

#endif
