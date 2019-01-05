#ifndef TIME_H
#define TIME_H

#pragma once
#include "shader.h"
#include <cstddef>
#include <deque>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <numeric>

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
	void display_status(Status status);
}

#include "frametime.tcc"
#endif
