#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once
#include "shader.h"
#include "type_conversion.h"
#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class Transform {
	public:
		enum class Axis { X, Y, Z };
		enum class Sign { Pos, Neg };

		Transform(Transform const&) = delete;
		Transform& operator=(Transform const&) = delete;
		~Transform();

		void translate(glm::vec3 direction);
		void translate(float distance, Axis axis, Sign sign = Sign::Pos);

		void scale(glm::vec3 scaling_vector);
		void scale(float magnitude, Axis axis);

		void rotate(float degrees, glm::vec3 axis);
		void rotate(float degrees, Axis axis, Sign sign = Sign::Pos);

		void undo_last_transform();
		void reset_transforms();

		glm::mat4 model_matrix() const;

		static void force_update();
		
		static std::string const MODEL_UNIFORM_NAME;
	protected:
		Transform(std::shared_ptr<Shader> const& shader = nullptr);

	private:
		using stack_t = std::stack<glm::mat4, std::vector<glm::mat4>>;
		stack_t transforms_;
		std::shared_ptr<Shader> shader_;
		static std::vector<std::reference_wrapper<Transform>> instances_;

		void add_transform(glm::mat4&& transform);
		void update_model() const;
};

#endif
