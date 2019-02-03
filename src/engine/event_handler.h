#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#pragma once
#include "camera.h"
#include "inserter.h"
#include "logger.h"
#include "traits.h"
#include "shader.h"
#include "shader_handler.h"
#include "window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <type_traits>
#include <unordered_map>

class EventHandler {
	public:
		EventHandler(EventHandler const&) = delete;
		EventHandler& operator=(EventHandler const&) = delete;

		template <typename... Args>
		static void instantiate(std::shared_ptr<Camera> const& camera, Args const&... args);

		template <typename... Args>
		static void append_shaders(Args const&... args);

		static void upload_model(std::shared_ptr<Shader> const& shader, glm::mat4 model);
	
		static std::string const PROJECTION_UNIFORM_NAME;
		static std::string const VIEW_UNIFORM_NAME;
		static std::string const MODEL_UNIFORM_NAME;

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
		
		static std::unordered_map<std::shared_ptr<Shader>, glm::mat4> shader_model_pairs_;
		static bool instantiated_;
		static glm::mat4 perspective_;
		static EventHandler* instance_;
		static MousePosition mouse_position_;

		static KeyModifiers  modifier_states(int mod_bits);

		template <typename... Args>
		EventHandler(std::shared_ptr<Camera> const& camera, Args const&... args);
		
		template <typename... Args>
		void init(Args const&... args);

		static void update_perspective();
		static void update_view();

		static void key_callback(GLFWwindow*, int key, int, int, int mod_bits);
		static void mouse_callback(GLFWwindow*, double x, double y);
		static void size_callback(GLFWwindow*, int widht, int height);
		static void shader_reload_callback(Shader const& shader);
	
};

#include "event_handler.tcc"
#endif
