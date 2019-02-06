#ifndef WINDOW_H
#define WINDOW_H

#pragma once
#include "context.h"
#include <cstddef>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window : public Context {
	public:
		Window(std::string const& name, std::size_t width, std::size_t height, float opengl_version = 4.5f);
		~Window();
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;
		
		bool should_close() const;
		void clear() const;
		void update() const;
	
	private:
		void init();
};

#endif
