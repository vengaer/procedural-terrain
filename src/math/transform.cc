#include "transform.h"

Transform::Transform() : transforms_{std::vector<glm::mat4>{glm::mat4{1.f}}} { }

void Transform::translate(glm::vec3 direction) {
	transforms_.push(glm::translate(transforms_.top(), direction));
}

void Transform::translate(float distance, Axis axis, Sign sign) {
	glm::vec3 translation_vector{0.f};
	translation_vector[enum_value(axis)] = std::pow(-1, enum_value(sign)) * distance;
	translate(translation_vector);
}

void Transform::scale(glm::vec3 scaling_vector) {
	transforms_.push(glm::scale(transforms_.top(), scaling_vector));
}

void Transform::scale(float magnitude, Axis axis) {
	glm::vec3 scaling_vector{1.f};
	scaling_vector[enum_value(axis)] = magnitude;
	scale(scaling_vector);
}

void Transform::rotate(float degrees, glm::vec3 axis) {
	transforms_.push(glm::rotate(transforms_.top(), glm::radians(degrees), axis));
}

void Transform::rotate(float degrees, Axis axis, Sign sign) {
	glm::vec3 basis_vector{0.f};
	basis_vector[enum_value(axis)] = std::pow(-1, enum_value(sign));
	rotate(degrees, basis_vector);
}

void Transform::undo_last_transform() {
	transforms_.pop();
}

void Transform::reset_transforms() {
	while(!transforms_.empty())
		transforms_.pop();
}
	
glm::mat4 Transform::model_matrix() const {
	return transforms_.top();
}
