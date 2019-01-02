#include "frametime.h"

namespace frametime {
	std::size_t constexpr MEAN_FRAMES = 100u;
	float const START_TIME = static_cast<float>(glfwGetTime());

	
	std::deque<float> frame_times(MEAN_FRAMES);
	
	float current_frame{0.f}, last_frame{0.f}, delta_time{0.f} ;
}

void frametime::update() {
	current_frame = static_cast<float>(glfwGetTime());
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
}

float frametime::delta() {
	return delta_time;
}

float frametime::uptime() {
	return static_cast<float>(glfwGetTime()) - START_TIME;
}

float frametime::fps() {
	float rate = std::accumulate(std::begin(frame_times), std::end(frame_times), 0.f) / MEAN_FRAMES;

	frame_times.push_front(1 / delta_time);
	frame_times.pop_back();
	
	return rate;
}

namespace{
	void display_status_impl(frametime::Status status) {
		switch(status) {
			case frametime::Status::Fps:
				std::cout << std::left << std::setw(8) << frametime::fps() << std::setw(6) << " fps";
				break;
			default:
				std::cout << std::left << std::setw(10) << frametime::uptime() << std::setw(6) << " s";
				break;
		}
	}
}


void frametime::display_status(frametime::Status status) {
	switch(status) {
		case Status::All:
			display_status_impl(Status::Fps);
			std::cout << " ";
			display_status_impl(Status::Uptime);
			break;
		default:
			display_status_impl(status);
			break;
	}
	std::cout << '\r' << std::flush;
}

