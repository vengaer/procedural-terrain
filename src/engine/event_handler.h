#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#pragma once
#include "camera.h"
#include "inserter.h"
#include "traits.h"
#include "shader.h"
#include "window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class EventHandler {
	public:
		template <typename... Args>
		static void instantiate(Window& window, Camera& camera, Args&&... args);

		template <typename... Args>
		static void append_shaders(Args&&... args);
	
		static std::string const PROJECTION_UNIFORM_NAME;
		static std::string const VIEW_UNIFORM_NAME;

	private:
		Camera& camera_;
		Window& window_;
		
		static std::vector<GLuint> shader_ids_;
		static bool instantiated_;
		static glm::mat4 perspective_;
		static EventHandler* instance_;

		struct KeyModifiers {
			enum class State { Inactive, Active };
			State shift;
			State ctrl;
			State alt;
			State super;
		};
		static KeyModifiers  modifier_states(int mod_bits);

		template <typename... Args>
		EventHandler(Window& window, Camera& camera, Args&&... args);
		
		template <typename... Args>
		void init(Args&&... args);

		static void update_perspective(float width, float height);
		static void update_view();

		static void size_callback(GLFWwindow* window, int widht, int height);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod_bits);
	
};

#include "event_handler.tcc"
#endif
