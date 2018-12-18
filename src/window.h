#ifndef WINDOW_H
#define WINDOW_H

#pragma once
#include "exception.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Window{
	public:
		Window(std::string const& name, unsigned width, unsigned height);
		~Window();
		
		Window(Window const&) = delete;
		Window& operator=(Window const&) = delete;
		
		bool should_close() const;
		void clear() const;
		void update() const;

	private:
		GLFWwindow* window_;
		unsigned width_, height_;
		
		void init(std::string const& name, unsigned width, unsigned height);
		
		static void resize(GLFWwindow* window, int width, int height);
};

#endif
