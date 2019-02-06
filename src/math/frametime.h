#ifndef TIME_H
#define TIME_H

#pragma once

namespace frametime {
	void update();

	float delta();
	float uptime();
	float fps();
}

#endif
