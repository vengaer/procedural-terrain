#include "cuboid.h"
#include "camera.h"
#include "cylinder.h"
#include "ellipsoid.h"
#include "frametime.h"
#include "event_handler.h"
#include "exception.h"
#include "logger.h"
#include "plane.h"
#include "shader.h"
#include "traits.h"
#include "window.h"
#include <type_traits>

#include <regex>


template <typename Exception, typename = std::enable_if_t<is_exception_v<Exception>>>
int handle_exception(Exception const& err);

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
			frametime::update();
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
		return handle_exception(err);
	}

	return 0;
}

/* Return codes
* 0 - 9 - Fundamental OpenGL calls
* 10-19 - Error while reding, processing or reloading shaders
* 20-29 - Function argument related
* 30-39 - Over- or underflow
* 40-49 - File system related
* 100   - "Unknown" exception (e.g. std::bad_alloc, std::bad_variant_access...)
*/
template <typename Exception, typename = std::enable_if_t<is_exception_v<Exception>>>
int handle_exception(Exception const& err) {
	using namespace std::string_literals;
	std::string type;
	int ret_val;
	if(typeid(err) == typeid(ArgumentMismatchException)) {
		type = "ArgumentMismatchException"s;
		ret_val = 20;
	}
	else if(typeid(err) == typeid(BadUniformParametersException)) {
			type = "BadUniformParametersException"s;
			ret_val = 21;
	}
	else if(typeid(err) == typeid(ContextCreationException)) {
			type = "ContextCreationException"s;
			ret_val = 2;
	}
	else if(typeid(err) == typeid(FileSystemException)) {
			type = "FileSystemException"s;
			ret_val = 40;
	}
	else if(typeid(err) == typeid(FileIOException)) {
			type = "FileIOException"s;
			ret_val = 41;
	}
	else if(typeid(err) == typeid(GLException)) {
			type = "GLException"s;
			ret_val = 1;
	}
	else if(typeid(err) == typeid(InvalidArgumentException)) {
			type = "InvalidArgumentException"s;
			ret_val = 22;
	}
	else if(typeid(err) == typeid(OutOfOrderInitializationException)) {
			type = "OutOfOrderInitializationException"s;
			ret_val = 23;
	}
	else if(typeid(err) == typeid(OverflowException)) {
			type = "OverflowException"s;
			ret_val = 30;
	}
	else if(typeid(err) == typeid(ShaderCompilationException)) {
			type = "ShaderCompilationException"s;
			ret_val = 10;
	}
	else if(typeid(err) == typeid(ShaderIncludeException)) {
			type = "ShaderIncludeException"s;
			ret_val = 11;
	}
	else if(typeid(err) == typeid(ShaderLinkingException)) {
			type = "ShaderLinkingException"s;
			ret_val = 12;
	}
	else if(typeid(err) == typeid(ShaderReloadException)) {
			type = "ShaderReloadException"s;
			ret_val = 13;
	}
	else if(typeid(err) == typeid(UnderflowException)) {
			type = "UnderflowException"s;
			ret_val = 31;
	}
	else {
		type = "Unknown exception"s;
		ret_val = 100;
	}
	ERR_LOG_CRIT("Program terminated after an instance of ", type,  " was thrown: ", err.what());
	return ret_val;
}
