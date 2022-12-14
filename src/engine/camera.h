#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "bitmask.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ClipSpace {
	float near;
	float far;
};

class Camera {
    struct dirs {
        using value_type = unsigned char;
        static value_type constexpr Right    = 0x1;
        static value_type constexpr Left     = 0x2;
        static value_type constexpr Up       = 0x4;
        static value_type constexpr Down     = 0x8;
        static value_type constexpr Backward = 0x10;
        static value_type constexpr Forward  = 0x20;
    };

    struct rot_dirs {
        using value_type = unsigned char;
        static value_type constexpr Right = 0x1;
        static value_type constexpr Left  = 0x2;
        static value_type constexpr Up    = 0x4;
        static value_type constexpr Down  = 0x8;
    };
    using MoveMask = Bitmask<dirs>;
    using RotationMask = Bitmask<rot_dirs>;
	public:
		enum class Direction{ 
            Right    = 0x1, 
            Left     = 0x2, 
            Up       = 0x4, 
            Down     = 0x8, 
            Backward = 0x10, 
            Forward  = 0x20 
        };
        enum class RotationDirection {
            Right = 0x1,
            Left  = 0x2,
            Up    = 0x4,
            Down  = 0x8
        };
		enum class Speed { Slow = 1, Default = 5, Fast = 15 };
        enum class KeyState { Up, Down };

		Camera(glm::vec3 position = glm::vec3{0.f, 0.f, 3.f}, 
			   glm::vec3 target_view = glm::vec3{0.f, 0.f, 0.f}, 
			   float fov = 60.f,
			   float yaw = 0.f,			/* Relative to negative local z */
			   float pitch = 35.f,
			   bool invert_y = false,
			   ClipSpace space = {0.1f, 200.f});

		void rotate(double delta_x, double delta_y);

        void set_state(Direction dir, KeyState state);
        void set_state(RotationDirection dir, KeyState state);
        void set_state(Speed speed);

        void update();
        
		float fov() const;
		glm::mat4 view() const;

		ClipSpace clip_space() const;
        
        glm::vec3 position();
        void set_position(glm::vec3 pos);

        float pitch() const;
        void set_pitch(float pitch);
        void invert_pitch();

        glm::vec3 view_direction() const;

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
		ClipSpace const clip_space_;
        MoveMask direction_{};
        RotationMask rotation_{};
        Speed speed_{Speed::Default};

        static std::size_t constexpr ROTATION_SPEED{25u};

		void init(glm::vec3 target_view);

		void compute_local_xy();
		void update_view();
};

#endif
