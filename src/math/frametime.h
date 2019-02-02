#ifndef TIME_H
#define TIME_H

#pragma once
#include "shader.h"
#include "traits.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <type_traits>

namespace frametime {
	enum class Status { All, Fps, Uptime };
	std::string const TIME_UNIFORM_NAME = "ufrm_time";

	/* Pass shaders to upload time to if any */
	template <typename... Args>
	void update(Args&&... args);

	void update();

	float delta();
	float uptime();
	float fps();
}

#include "frametime.tcc"
#endif
