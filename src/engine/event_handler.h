#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#pragma once
#include "camera.h"
#include "logger.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class EventHandler {
	public:
		EventHandler(EventHandler const&) = delete;
		EventHandler& operator=(EventHandler const&) = delete;

		static void instantiate(std::shared_ptr<Camera> const& camera);

	private:
		struct MousePosition {
			double x;
			double y;
		};

		struct KeyModifiers {
			enum class State { Inactive, Active };
			State shift;
			State ctrl;
			State alt;
			State super;
		};
		std::shared_ptr<Camera> camera_;
		
		static bool instantiated_;
		static EventHandler* instance_;
		static MousePosition mouse_position_;


		EventHandler(std::shared_ptr<Camera> const& camera);
		
		void init();

		static KeyModifiers modifier_states(int mod_bits);

		static void update_perspective();
		static void update_view();

		static void key_callback(GLFWwindow*, int key, int, int, int mod_bits);
		static void mouse_callback(GLFWwindow*, double x, double y);
		static void size_callback(GLFWwindow*, int widht, int height);
};

#endif
