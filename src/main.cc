#include "window.h"
#include <iostream>



int main() {
	unsigned constexpr width = 960, height = 540;

	try{
		Window window{"test", width, height};
		
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
