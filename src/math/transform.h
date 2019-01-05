#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once
#include "fold.h"
#include "inserter.h"
#include "traits.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


class Transform {
	public:
		enum class Axis { X, Y, Z };
		enum class Sign { Pos, Neg };

		void translate(glm::vec3 direction);
		void translate(float distance, Axis axis, Sign sign = Sign::Pos);

		void scale(glm::vec3 scaling_vector);
		void scale(float magnitude, Axis axis);

		void rotate(float degrees, glm::vec3 axis);
		void rotate(float degrees, Axis axis, Sign sign = Sign::Pos);

		void undo_last_transform();
		void reset_transforms();

		glm::mat4 model_matrix() const;
	
	protected:
		Transform();

	private:
		using stack_t = std::stack<glm::mat4, std::vector<glm::mat4>>;

		stack_t transforms_;

};

#endif
