#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ClippingPlane{
	float near;
	float far;
};

class Camera {
	public:
		enum class Direction{ Right, Left, Up, Down, Backward, Forward };
		enum class Speed { Default = 10, Fast = 15 };

		Camera(glm::vec3 position = glm::vec3{0.f, 0.f, 3.f}, 
			   glm::vec3 target_view = glm::vec3{0.f, 0.f, 0.f}, 
			   float fov = 60.f,
			   float yaw = 0.f,			/* Relative to negative local z */
			   float pitch = 0.f,
			   bool invert_y = false,
			   float sensitivity = 0.002f,
			   ClippingPlane plane = {0.1f, 100.f});

		void translate(Direction dir, Speed speed = Speed::Default);
		void rotate(double delta_x, double delta_y);

		float fov() const;
		glm::mat4 view() const;

		ClippingPlane clipping_plane() const;

	private:
		glm::vec3 position_;
		glm::vec3 local_x_;
		glm::vec3 local_y_;
		glm::vec3 local_z_;
		glm::mat4 view_;
		float fov_;
		float yaw_;
		float pitch_;
		bool invert_y_;
		float const sensitivity_;
		ClippingPlane const clipping_plane_;

		void init(glm::vec3 target_view);

		void compute_local_xy();
		void update_view();
};

#endif
