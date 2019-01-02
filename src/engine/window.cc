#include "window.h"

Window::Window(std::string const& name, std::size_t width, std::size_t height, float opengl_version) : window_{nullptr}, width_{width}, height_{height} {
	if(std::signbit(opengl_version))
		throw InvalidVersionException{"OpenGL version must be positive\n"};

	init(name, width, height, opengl_version);
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


void Window::init(std::string const& name, std::size_t width, std::size_t height, float opengl_version){
	if(!glfwInit())
		throw GLException{"Failed to initialize GLFW\n"};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<std::size_t>(opengl_version));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<std::size_t>(10.f*std::fmod(opengl_version, 1.f)));
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	if(!window_)
		throw GLException{"Could not create GLFW window.\nDoes your graphics driver support OpenGL version " + std::to_string(opengl_version) +"?"};

	glfwMakeContextCurrent(window_);

	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
		throw GLException{"Failed to initialize GLEW\n"};

	glfwSetWindowUserPointer(window_, this);
	glfwSetWindowSizeCallback(window_, resize);
}

void Window::resize(GLFWwindow* glfw_window, int width, int height){
	auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	instance->width_ = static_cast<std::size_t>(width);
	instance->height_ = static_cast<std::size_t>(height);
	glViewport(0, 0, width, height);
}
	
