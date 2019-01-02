#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "frametime.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	public:
		Camera(std::size_t window_width, std::size_t window_height, glm::vec3 position = glm::vec3{0.f, 0.f, 1.f}, glm::vec3 target_view = glm::vec3{0.f, 0.f, 0.f}, float fov = 45.f);

		glm::mat4 get_perspective() const;

	private:
		static float constexpr NEAR{0.1f}, FAR{100.f};
		glm::vec3 position_;
		glm::vec3 local_x_;
		glm::vec3 local_y_;
		glm::vec3 local_z_;
		glm::mat4 view_;
		glm::mat4 perspective_;
		float fov_;

		void init(std::size_t window_width, std::size_t window_height, glm::vec3 target_view);
};

#endif
