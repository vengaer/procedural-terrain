#ifndef WINDOW_H
#define WINDOW_H

#pragma once
#include "exception.h"
#include <cstddef>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window{
		friend class EventHandler;
	public:
		Window(std::string const& name, std::size_t width, std::size_t height, float opengl_version = 4.5f);
		~Window();
		
		Window(Window const&) = delete;
		Window& operator=(Window const&) = delete;
		
		bool should_close() const;
		void clear() const;
		void update() const;
	
		std::size_t width() const;
		std::size_t height() const;
	private:
		GLFWwindow* window_;
		std::size_t width_, height_;
		
		void init(std::string const& name, std::size_t width, std::size_t height, float opengl_version);

		void set_dimensions(std::size_t width, std::size_t height);
		GLFWwindow* glfw_window() const;
};

#endif
