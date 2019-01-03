#include "cuboid.h"
#include "camera.h"
#include "container.h"
#include "cylinder.h"
#include "event_handler.h"
#include "ellipsoid.h"
#include "frametime.h"
#include "plane.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"
#include <iostream>


int main() {
	unsigned constexpr width = 960, height = 540;

	try{
		Window window{"test", width, height};
		Shader test("/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.vert", Shader::Type::Vertex, "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/plane.frag", Shader::Type::Fragment);
		Camera cam{};
		EventHandler::instantiate(window, cam, test);
		Plane p{1, 1, 1, 1};
		Ellipsoid e{};
		Cuboid c{};
		Cylinder cyl{};
		glm::mat4 view{1.f};
		view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

		using UC = Shader::Uniform::Construct;
		using UD = Shader::Uniform::Dimension;
		using UT = Shader::Uniform::Type;
		test.upload_uniform<UC::Matrix, UD::_4, UT::FloatPtr>("view", glm::value_ptr(view));

		glClearColor(0.1f, 0.2f, 0.4f, 0.5f);
		while(!window.should_close()){
			frametime::update();
			//frametime::display_status(frametime::Status::ALL);
			window.clear();
			test.enable();
			//p.render();
			//e.render();
			c.render();
			//cyl.render();

			Shader::disable();
			window.update();
		}
	}
	catch(ArgumentMismatchException const& err){
		std::cerr << err.what();
	}
	catch(FileIOException const& err){
		std::cerr << err.what();
	}
	catch(GLException const& err){
		std::cerr << err.what();
	}
	catch(ShaderCompilationException const& err){
		std::cerr << err.what();
	}
	catch(ShaderIncludeException const& err){
		std::cerr << err.what();
	}
	catch(ShaderLinkingException const& err){
		std::cerr << err.what();
	}

	return 0;
}
