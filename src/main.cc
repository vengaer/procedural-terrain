#include "cuboid.h"
#include "camera.h"
#include "cylinder.h"
#include "event_handler.h"
#include "ellipsoid.h"
#include "frametime.h"
#include "plane.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"
#include <iostream>
#include "inserter.h"
#include "collections.h"
#include "logger.h"

int main() {
	unsigned constexpr width = 960, height = 540;
	try{
		Window window{"test", width, height};
		Shader test("/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.vert", Shader::Type::Vertex, "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.frag", Shader::Type::Fragment);
		test.enable();



		Camera cam{};
		EventHandler::instantiate(window, cam, test);
		Plane p{1, 1, 1, 1};
		Ellipsoid e{};
		Cuboid c{};
		Cylinder cyl{};

		test.upload_uniform("ufrm_model", c.model_matrix());

		glClearColor(0.1f, 0.2f, 0.4f, 0.5f);
		while(!window.should_close()){
			frametime::update(test);
			//frametime::display_status(frametime::Status::All);
			window.clear();
			//p.render();
			//e.render();
			c.render();
			//cyl.render();

			window.update();
		}
	}
	catch(std::runtime_error const& err){
		std::cerr << err.what();
	}

	return 0;
}
