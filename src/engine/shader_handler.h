#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H

#pragma once
#include "shader.h"
#include <GL/glew.h>
#include <memory>

struct manual_shader_handler {
	void draw(GLuint vao, GLuint idx_size) const noexcept;
	
	std::shared_ptr<Shader> const shader() const noexcept;

	static bool constexpr is_automatic = false;
};

struct automatic_shader_handler {
	void draw(GLuint vao, GLuint idx_size) const noexcept;
	
	std::shared_ptr<Shader> const shader() const noexcept;
	
	static bool constexpr is_automatic = true;

	std::shared_ptr<Shader> shader_;
};

#endif
