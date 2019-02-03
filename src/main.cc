#include "cuboid.h"
#include "camera.h"
#include "cylinder.h"
#include "event_handler.h"
#include "ellipsoid.h"
#include "frametime.h"
#include "plane.h"
#include "shader.h"
#include "window.h"

#include "logger.h"

int main() {
	unsigned constexpr width = 960, height = 540;

	try{
		Window window{"test", width, height};
		std::shared_ptr<Shader> plane_shader = std::make_shared<Shader>( "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.vert", Shader::Type::Vertex, 
																		 "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.frag", Shader::Type::Fragment );

		std::shared_ptr<Camera> cam = std::make_shared<Camera>();
		EventHandler::instantiate(cam);
		Plane p{automatic_shader_handler{plane_shader}};
		Ellipsoid e{automatic_shader_handler{plane_shader}};
		Cuboid c{automatic_shader_handler{plane_shader}};
		Cylinder cyl{automatic_shader_handler{plane_shader}};


		glClearColor(0.1f, 0.2f, 0.4f, 0.5f);
		while(!window.should_close()){
			frametime::update(plane_shader);
			//frametime::display_status(frametime::Status::All);
			window.clear();
			//p.render();
			e.render();			//		
			//plane_shader->upload_uniform("ufrm_model", c.model_matrix());
			//c.render();
			//cyl.render();

			window.update();
		}
	}
	catch(std::exception const& err){
		ERR_LOG_CRIT(err.what());
	}

	return 0;
}
