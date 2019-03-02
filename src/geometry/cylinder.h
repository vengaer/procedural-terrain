#ifndef CYLINDER_H
#define CYLINDER_H

#pragma once
#include "constants.h"
#include "interpolation.h"
#include "renderer.h"
#include "shader_handler.h"
#include "transform.h"
#include "type_conversion.h"
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>


template <typename ShaderPolicy = manual_shader_handler>
class Cylinder : public Renderer<Cylinder<ShaderPolicy>, ShaderPolicy>, public Transform {
	using renderer_t = Renderer<Cylinder<ShaderPolicy>, ShaderPolicy>;
	public:
		enum class Axis { X, Y, Z };
		Cylinder(ShaderPolicy policy = {}, GLfloat height = 1.f, GLuint horizontal_segments = 30u, GLuint vertical_segments = 1u, Axis main_axis = Axis::Y, GLfloat off_axis1_scale = 0.5f, GLfloat off_axis2_scale = 0.5f);

		void init(GLfloat height, GLuint horizontal_segments, GLuint vertical_segments, Axis main_axis, GLfloat off_axis1_scale, GLfloat off_axis2_scale);

		std::vector<GLfloat> const& vertices() const;
		std::vector<GLuint> const& indices() const;

	private:
		std::vector<GLfloat> vertices_;
		std::vector<GLuint> indices_;

		static std::pair<Axis, Axis> get_off_axes(Axis main_axis);
};

#include "cylinder.tcc"
#endif
