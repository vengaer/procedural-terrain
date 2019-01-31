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

class C {
	public:
		C(int i) : i_{i} { };
		C(C const& c) {
			std::cout << "copy ctor for c\n";
			i_ = c.i_;
		}
		C(C&& c) {
			std::cout << "move ctor\n";	
			i_ = c.i_;
		}
		
		C& operator=(C const& other) {
			std::cout << "copy assignment\n";
			i_ = other.i_;
			return *this;
		}
		C& operator=(C&& c) {
			std::cout << "move assignment\n";
			i_ = c.i_;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream&, C const&);
	private:
		int i_;
};

std::ostream& operator<<(std::ostream& os, C const& c) {
	os << c.i_;
}

int main() {
	unsigned constexpr width = 960, height = 540;
	std::vector<C> v1{1,2,3,4};
	std::vector c2{3,4,5,6,7,};
	std::cout << "zipping\n";
	for(auto [i,j] : zip(v1,std::vector<C>{1,3,4,5}))
		std::cout << i << " " << j << "\n";

	try{
		Window window{"test", width, height};
		std::shared_ptr<Shader> plane_shader = std::make_shared<Shader>( "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.vert", Shader::Type::Vertex, "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.frag", Shader::Type::Fragment );

		std::shared_ptr<Camera> cam = std::make_shared<Camera>();
		EventHandler::instantiate(cam, plane_shader);
		Plane p{plane_shader};
		Ellipsoid e{plane_shader};
		Cuboid c{plane_shader};
		Cylinder cyl{plane_shader};


		glClearColor(0.1f, 0.2f, 0.4f, 0.5f);
		while(!window.should_close()){
			frametime::update(plane_shader);
			//frametime::display_status(frametime::Status::All);
			window.clear();
			//p.render();
			//e.render();			//		
			plane_shader->upload_uniform("ufrm_model", c.model_matrix());
			c.render();
			//cyl.render();

			window.update();
		}
	}
	catch(std::exception const& err){
		ERR_LOG_CRIT(err.what());
	}

	return 0;
}
