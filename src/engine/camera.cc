#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target_view, float fov, ClippingPlane plane) : position_{position}, local_x_{}, local_y_{}, local_z_{}, view_{}, fov_{fov}, clipping_plane_{plane} { 
	init(target_view);
}

void Camera::init(glm::vec3 target_view) {
	local_z_ = glm::normalize(position_ - target_view);

	glm::vec3 world_y = glm::vec3{0.f, 1.f, 0.f};
	local_x_ = glm::normalize(glm::cross(world_y, local_z_));
	
	local_y_ = glm::cross(local_z_, local_x_);

	view_ = glm::lookAt(position_, position_ - local_z_, world_y);

}

void Camera::translate(Direction dir, Speed speed) {
	int sign = std::pow(-1, enum_value(dir));
	float speed_modifier = static_cast<float>(speed) / 10.f * frametime::delta();
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
}

float Camera::fov() const {
	return fov_;
}

ClippingPlane Camera::clipping_plane() const {
	return clipping_plane_;
}
