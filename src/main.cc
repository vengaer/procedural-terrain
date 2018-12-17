#include "shader.h"
#include "window.h"
#include <iostream>



int main() {
	unsigned constexpr width = 960, height = 540;

	try{
		Window window{"test", width, height};
		Shader test("/home/vilhelm/repos/c++/procedural_terrain/src/shaders/test.vert", ShaderType::VERTEX, "/home/vilhelm/repos/c++/procedural_terrain/src/shaders/test.vert", ShaderType::FRAGMENT);
		
		while(!window.should_close()){
			window.clear();

			window.update();
		}
	}
	catch(GLException const& err){
		std::cerr << err.what();
	}

	return 0;
}
