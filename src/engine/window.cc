#include "window.h"

Window::Window(std::string const& name, std::size_t width, std::size_t height, float opengl_version) : Context{name, width, height, opengl_version} {
	if(std::signbit(opengl_version))
		throw InvalidVersionException{"Version must be positive"};
	init();
}

Window::~Window(){
	glfwTerminate();
}

bool Window::should_close() const {
	return glfwWindowShouldClose(context_);
}

void Window::clear() const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::update() const{
	glfwSwapBuffers(context_);
	glfwPollEvents();
}

void Window::set_dimensions(std::size_t width, std::size_t height) {
	width_ = width;
	height_ = height;
}

std::size_t Window::width() const {
	return width_;
}

std::size_t Window::height() const {
	return height_;
}

GLFWwindow* Window::context() const {
	return context_;
}


void Window::init(){
	glfwSetWindowUserPointer(context_, this);
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
		throw GLException{"Failed to initialize GLEW\n"};

}

