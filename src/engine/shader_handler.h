#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H

#pragma once
#include "shader.h"
#include <GL/glew.h>
#include <memory>

struct manual_shader_handler { };

struct automatic_shader_handler {
	void operator()() const noexcept;
	
	std::shared_ptr<Shader> const shader() const noexcept;
	
	static bool constexpr is_automatic = true;

	std::shared_ptr<Shader> shader_;
};

#endif
