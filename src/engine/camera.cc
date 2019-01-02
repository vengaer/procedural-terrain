#include "camera.h"

Camera::Camera(std::size_t window_width, std::size_t window_height, glm::vec3 position, glm::vec3 target_view, float fov) : position_{position}, local_x_{}, local_y_{}, local_z_{}, view_{}, perspective_{}, fov_{fov} { 
	init(window_width, window_height, target_view);
}

void Camera::init(std::size_t window_width, std::size_t window_height, glm::vec3 target_view) {
	local_z_ = glm::normalize(position_ - target_view);

	glm::vec3 world_y = glm::vec3{0.f, 1.f, 0.f};
	local_x_ = glm::normalize(glm::cross(world_y, local_z_));
	
	local_y_ = glm::cross(local_z_, local_x_);

	view_ = glm::lookAt(position_, position_ - local_z_, world_y);

	perspective_ = glm::perspective(glm::radians(fov_), static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 100.f);
}

glm::mat4 Camera::get_perspective() const {
	return perspective_;
}
