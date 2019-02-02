#include "inserter.h"
#include "traits.h"
#include "transform.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <type_traits>
#include <utility>

Transform::Transform(std::shared_ptr<Shader> const& shader) : transforms_{std::vector<glm::mat4>{glm::mat4{1.f}}}, shader_{shader} { 
	instances_.push_back(std::ref(*this));
	update_model();
}

Transform::~Transform() {
	instances_.erase(std::remove_if(std::begin(instances_), std::end(instances_), [this](auto wrapper) {
		return std::addressof(wrapper.get()) == this;
	}));
}

void Transform::translate(glm::vec3 direction) {
	add_transform(glm::translate(transforms_.top(), direction));
}

void Transform::translate(float distance, Axis axis, Sign sign) {
	glm::vec3 translation_vector{0.f};
	translation_vector[enum_value(axis)] = std::pow(-1, enum_value(sign)) * distance;
	translate(translation_vector);
}

void Transform::scale(glm::vec3 scaling_vector) {
	add_transform(glm::scale(transforms_.top(), scaling_vector));
}

void Transform::scale(float magnitude, Axis axis) {
	glm::vec3 scaling_vector{1.f};
	scaling_vector[enum_value(axis)] = magnitude;
	scale(scaling_vector);
}

void Transform::rotate(float degrees, glm::vec3 axis) {
	add_transform(glm::rotate(transforms_.top(), glm::radians(degrees), axis));
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

void Transform::add_transform(glm::mat4&& transform) {
	transforms_.push(std::move(transform));
	if(shader_)
		update_model();
}

void Transform::update_model() const {
	shader_->upload_uniform(MODEL_UNIFORM_NAME, transforms_.top());
}

void Transform::force_update() {
	for(auto const& i : instances_)
		i.get().update_model();
}

std::string const Transform::MODEL_UNIFORM_NAME = "ufrm_model";
std::vector<std::reference_wrapper<Transform>> Transform::instances_;
