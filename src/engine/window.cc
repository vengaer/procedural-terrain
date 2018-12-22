#include "window.h"

Window::Window(std::string const& name, unsigned width, unsigned height) : window_{nullptr}, width_{width}, height_{height} {
	init(name, width, height);
}

Window::~Window(){
	glfwDestroyWindow(window_),
	glfwTerminate();
}

bool Window::should_close() const {
	return glfwWindowShouldClose(window_);
}

void Window::clear() const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::update() const{
	glfwSwapBuffers(window_);
	glfwPollEvents();
}


void Window::init(std::string const& name, unsigned width, unsigned height){
	if(!glfwInit())
		throw GLException{"Failed to initialize GLFW\n"};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	if(!window_)
		throw GLException{"Could not create GLFW window\n"};

	glfwMakeContextCurrent(window_);

	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
		throw GLException{"Failed to initialize GLEW\n"};

	glfwSetWindowUserPointer(window_, this);
	glfwSetWindowSizeCallback(window_, resize);
}

void Window::resize(GLFWwindow* glfw_window, int width, int height){
	Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	instance->width_ = width;
	instance->height_ = height;
	glViewport(0, 0, width, height);
}
	
