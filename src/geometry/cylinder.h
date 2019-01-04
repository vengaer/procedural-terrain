#ifndef CYLINDER_H
#define CYLINDER_H

#pragma once
#include "constants.h"
#include "fold.h"
#include "interpolation.h"
#include "renderer.h"
#include "transform.h"
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>


class Cylinder : public Renderer<Cylinder>, public Transform {
	public:
		enum class Axis { X, Y, Z };
		Cylinder(GLfloat height = 1.f, GLuint horizontal_segments = 30u, GLuint vertical_segments = 1u, Axis main_axis = Axis::Y, GLfloat off_axis1_scale = 0.5f, GLfloat off_axis2_scale = 0.5f);

		void init(GLfloat height, GLuint horizontal_segments, GLuint vertical_segments, Axis main_axis, GLfloat off_axis1_scale, GLfloat off_axis2_scale);

		std::vector<GLfloat> const& vertices() const;
		std::vector<GLuint> const& indices() const;

	private:
		std::vector<GLfloat> vertices_;
		std::vector<GLuint> indices_;

		static std::pair<Axis, Axis> get_off_axes(Axis main_axis);
};


#endif
