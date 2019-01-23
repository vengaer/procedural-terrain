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
#include <utility>
#include <vector>

struct main_context_tag { };

class Context {
	public:
		enum class Id { None = -1, Secondary, Main };
		Context(std::string const& name, std::size_t width, std::size_t height, bool visible = true, Id share_with = Id::None, float version = 4.5f);
		~Context();
		Context(Context const&) = delete;
		Context& operator=(Context const&) = delete;

	protected:
		GLFWwindow* context_;
		std::size_t width_, height_;

		Context(main_context_tag, std::string const& name, std::size_t width, std::size_t height, Id share_with = Id::None, float version = 4.5f);
	private:
		bool is_visible_{true};
		static std::vector<std::pair<Id, std::reference_wrapper<Context>>> instances_;
		
		void init(std::string const& name, std::size_t width, std::size_t height, Id share_with, float version, Id context_id = Id::Secondary);
};
#endif
