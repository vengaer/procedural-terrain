#ifndef FRAMETIME_H
#define FRAMETIME_H

#pragma once

namespace frametime {
	void update();

	float delta();
	float uptime();
	float fps();
}

#endif
