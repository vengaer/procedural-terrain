#include "renderer.h"
#include "shader.h"
#include "window.h"
#include <iostream>



int main() {
	unsigned constexpr width = 960, height = 540;


	try{
		Window window{"test", width, height};
		Shader test("/home/vilhelm/repos/c++/procedural_terrain/src/shaders/bubble.vert", Shader::Type::VERTEX, "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/bubble.frag", Shader::Type::FRAGMENT);
		
		while(!window.should_close()){
			window.clear();

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
