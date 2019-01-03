#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#pragma once
#include "window.h"
#include "camera.h"
#include "container.h"
#include "extended_traits.h"
#include "shader.h"
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
	
		~EventHandler() {
			std::cout << "destroyed\n";
		}
	
		static std::string const PERSPECTIVE_UNIFORM_NAME;

	private:
		Camera& camera_;
		Window& window_;
		
		static std::vector<GLuint> shader_ids_;
		static bool instantiated_;
		static glm::mat4 perspective_;
		static EventHandler* instance_;


		template <typename... Args>
		EventHandler(Window& window, Camera& camera, Args&&... args);
		
		template <typename... Args>
		void init(Args&&... args);

		static void update_perspective(float width, float height);

		static void size_callback(GLFWwindow* window, int widht, int height);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	
};

#include "event_handler.tcc"
#endif
