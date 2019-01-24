#ifndef CONTEXT_H
#define CONTEXT_H

#pragma once
#include "exception.h"
#include <cmath>
#include <cstddef>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Context {
	public:
		Context(std::string const& name, std::size_t widht, std::size_t height, bool visible = true, bool shared = false);
		~Context();
		Context(Context const&) = delete;
		Context(Context&&) = default;
		Context& operator=(Context const&) = delete;
		Context& operator=(Context&&) = default;

	protected:
		GLFWwindow* context_;
		std::size_t width_, height_;

		enum class Type { Primary, Secondary };

		Context(std::string const& name, std::size_t width, std::size_t height, float version = 4.5f);
	private:
		bool is_visible_{true};
		static Context* primary_;
		
		void init(Type type, std::string const& name, bool shared, float version = -1.f);
};
#endif
