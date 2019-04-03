#include "camera.h"
#include "ellipsoid.h"
#include "frametime.h"
#include "event_handler.h"
#include "exception.h"
#include "scene.h"
#include "shader.h"
#include "terrain.h"
#include "water.h"
#include "window.h"
#include <memory>

int main() 
try {
	unsigned constexpr width = 960, height = 540;
	
    Window window{"Main", width, height};

    std::shared_ptr<Shader> scene_shader   = std::make_shared<Shader>("assets/shaders/scene.vert", Shader::Type::Vertex,
                                                                      "assets/shaders/scene.frag", Shader::Type::Fragment);
    std::shared_ptr<Shader> terrain_shader = std::make_shared<Shader>("assets/shaders/terrain.vert", Shader::Type::Vertex, 
                                                                      "assets/shaders/terrain.frag", Shader::Type::Fragment);
    std::shared_ptr<Shader> sun_shader     = std::make_shared<Shader>("assets/shaders/sun.vert", Shader::Type::Vertex, 
                                                                      "assets/shaders/sun.frag", Shader::Type::Fragment);
    std::shared_ptr<Shader> water_shader   = std::make_shared<Shader>("assets/shaders/water.vert", Shader::Type::Vertex,
                                                                      "assets/shaders/water.frag", Shader::Type::Fragment);

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    EventHandler::instantiate(camera);

    Ellipsoid sun{automatic_shader_handler{sun_shader}};
    sun.translate(glm::vec3{-80.0, 40.0, 0.0});
    sun.scale(glm::vec3{10.0, 10.0, 10.0});
    
    float const water_height   = -10.8f;
    float const terrain_height = -10.f;

    Water water{water_shader, camera, terrain_height, automatic_shader_handler{water_shader}};
    water.translate(glm::vec3{0.0, water_height, 0.0});
    water.scale(glm::vec3{40.0, 1.0, 40.0});
    
    Terrain terrain{automatic_shader_handler{terrain_shader}, 10.f, 2.f, .05f, 2.f, .05f};
    terrain.translate(glm::vec3{0.0, terrain_height, 0.0});
    terrain.scale(glm::vec3{20.0, 1.0, 20.0});

    Scene scene{automatic_shader_handler{scene_shader}, {0.1f, 0.2f, 0.4f, 0.5f}};

    terrain_shader->upload_uniform("ufrm_sun_position", sun.position());
    water_shader->upload_uniform("ufrm_sun_position", sun.position());
    
    PostProcessing post_processing;

    /* Render part of scene that should be reflected and refracted in the water */
    auto render_scene = [&terrain]() {
        terrain.render();
    };

    while(!window.should_close()){
        window.clear();
        frametime::update();
        camera->update();

        water.pre_process(render_scene, sun_shader, terrain_shader);

        Shader::bind_main_framebuffer();
        render_scene();
        sun.render();
        water.render();

        Shader::bind_scene_texture();
        post_processing.perform();

        scene.render(); 
        window.update();
    }

	return 0;
}
catch(std::exception const& err){
    /* Return codes
    * 0 - 9 - Fundamental OpenGL calls
    * 10-19 - Error while reding, processing or reloading shaders
    * 20-29 - Function argument related
    * 30-39 - Over- or underflow
    * 40-49 - File system related
    */
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
	else if(typeid(err) == typeid(FramebufferException)) {
        ret_val = 14;
        type = "FramebufferException"s;
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
		throw;
	}

	ERR_LOG_CRIT("Program terminated after an instance of ", type,  " was thrown: ", err.what());
	return ret_val;
}


