#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target_view, float fov, float yaw, float pitch, bool invert_y, float sensitivity, ClippingPlane plane) 
: position_{position}, local_x_{}, local_y_{}, local_z_{}, view_{}, fov_{fov}, yaw_{yaw}, pitch_{pitch}, invert_y_{invert_y}, sensitivity_{sensitivity}, clipping_plane_{plane} { 
	init(target_view);
}

void Camera::init(glm::vec3 target_view) {
	local_z_ = glm::normalize(position_ - target_view);

	compute_local_xy();
	update_view();
}

void Camera::translate(Direction dir, Speed speed) {
	int sign = std::pow(-1, enum_value(dir));
	float speed_modifier = static_cast<float>(speed) * frametime::delta();
	float increment = sign * speed_modifier;
	glm::mat4 translation_matrix{1.f};

	switch(enum_value(dir) / 2) {
		case 0: 
			translation_matrix = glm::translate(translation_matrix, increment * local_x_);
			break;
		case 1:
			translation_matrix = glm::translate(translation_matrix, increment * local_y_);
			break;
		case 2:
			translation_matrix = glm::translate(translation_matrix, increment * local_z_);
			break;
	}

	position_ = translation_matrix * glm::vec4{position_, 1.f};
	update_view();
}

void Camera::rotate(double delta_x, double delta_y) {
	yaw_   += delta_x;
	pitch_ += delta_y * (invert_y_ ? -1.f : 1.f);
	pitch_ = glm::clamp(pitch_, -89.f, 89.f);

	local_z_.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_.y = std::sin(glm::radians(pitch_));
	local_z_.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_ = glm::normalize(local_z_);

	compute_local_xy();
	update_view();
}

float Camera::fov() const {
	return fov_;
}

glm::mat4 Camera::view() const {
	return view_;
}

ClippingPlane Camera::clipping_plane() const {
	return clipping_plane_;
}

void Camera::compute_local_xy(){
	local_x_ = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), local_z_));
	local_y_ = glm::cross(local_z_, local_x_);
}

void Camera::update_view() {
	view_ = glm::lookAt(position_, position_ - local_z_, glm::vec3{0.f, 1.f, 0.f});
}
