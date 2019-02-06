#ifndef CUBOID_H
#define CUBOID_H

#pragma once
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
class Cuboid : public Renderer<Cuboid<ShaderPolicy>, ShaderPolicy>, public Transform {
	public:
		Cuboid(ShaderPolicy policy = {}, GLfloat x_scale = 1.f, GLfloat y_scale = 1.f, GLfloat z_scale = 1.f);

		void init(GLfloat x_scale, GLfloat y_scale, GLfloat z_scale);
		
		std::vector<GLfloat> const& vertices() const;
		std::vector<GLuint> const& indices() const;

		enum class Axis { None = -1, X, Y, Z };
		enum class Sign { Pos, Neg };
	private:
		std::vector<GLfloat> vertices_;
		std::vector<GLuint> indices_;
};

#include "cuboid.tcc"
#endif
