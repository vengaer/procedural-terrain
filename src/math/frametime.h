#ifndef TIME_H
#define TIME_H

#pragma once
#include <cstddef>
#include <deque>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <numeric>

namespace frametime {
	enum class Status { All, Fps, Uptime };

	void update();
	float delta();
	float uptime();
	float fps();
	void display_status(Status status);
}

#endif
