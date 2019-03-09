#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <stack>
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
        glm::vec3 position() const;
		
		bool& has_been_transformed() const;
		
	protected:
		Transform();

	private:
		using stack_t = std::stack<glm::mat4, std::vector<glm::mat4>>;
		stack_t transforms_;
		bool mutable has_been_transformed_{false};

		void add_transform(glm::mat4&& transform);

};

#endif
