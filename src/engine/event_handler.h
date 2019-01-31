#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#pragma once
#include "camera.h"
#include "exception.h"
#include "inserter.h"
#include "logger.h"
#include "traits.h"
#include "transform.h"
#include "shader.h"
#include "window.h"
#include <cmath>
#include <cstddef>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

class EventHandler {
	public:
		EventHandler(EventHandler const&) = delete;
		EventHandler& operator=(EventHandler const&) = delete;

		template <typename... Args>
		static void instantiate(std::shared_ptr<Camera> const& camera, Args const&... args);

		template <typename... Args>
		static void append_shaders(Args const&... args);
	
		static std::string const PROJECTION_UNIFORM_NAME;
		static std::string const VIEW_UNIFORM_NAME;

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
		
		static std::vector<std::shared_ptr<Shader>> shaders_;
		static bool instantiated_;
		static glm::mat4 perspective_;
		static EventHandler* instance_;
		static MousePosition mouse_position_;

		static KeyModifiers  modifier_states(int mod_bits);

		template <typename... Args>
		EventHandler(std::shared_ptr<Camera> const& camera, Args const&... args);
		
		template <typename... Args>
		void init(Args const&... args);

		static void update_perspective(float width, float height);
		static void update_view();

		static void key_callback(GLFWwindow*, int key, int, int, int mod_bits);
		static void mouse_callback(GLFWwindow*, double x, double y);
		static void size_callback(GLFWwindow*, int widht, int height);
		static void shader_reload_callback(Shader const& shader);
	
};

#include "event_handler.tcc"
#endif
