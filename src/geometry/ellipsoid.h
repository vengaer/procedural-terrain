#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#pragma once
#include "constants.h"
#include "interpolation.h"
#include "renderer.h"
#include "shader_handler.h"
#include "transform.h"
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>


template <typename ShaderPolicy = manual_shader_handler>
class Ellipsoid : public Renderer<Ellipsoid<ShaderPolicy>, ShaderPolicy>, public Transform<ShaderPolicy> {
	public:
		Ellipsoid(ShaderPolicy policy = {}, GLuint vertical_segments = 30u, GLuint horizontal_segments = 30u,  GLfloat x_scale = 1.f, GLfloat y_scale = 1.f, GLfloat z_scale = 1.f);

		void init(GLuint vertical_segments, GLuint horizontal_segments, GLfloat x_scale, GLfloat y_scale, GLfloat z_scale);

		std::vector<GLfloat> const& vertices() const;
		std::vector<GLuint> const& indices() const;
	private:
		std::vector<GLfloat> vertices_;
		std::vector<GLuint> indices_;
};

#include "ellipsoid.tcc"
#endif
