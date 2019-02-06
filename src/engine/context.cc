#include "context.h"
#include "exception.h"
#include "logger.h"
#include <cmath>
#include <functional>


Context::Context(std::string const& name, std::size_t width, std::size_t height, bool visible, bool shared) : context_{nullptr}, width_{width}, height_{height}, is_visible_{visible} {
	init(Type::Secondary, name, shared);
} 

Context::Context(std::string const& name, std::size_t width, std::size_t height, float version) : context_{nullptr}, width_{width}, height_{height}, is_visible_{true} {
	init(Type::Primary, name, false, version);
}

Context::~Context() {
	glfwDestroyWindow(context_);
	context_ = nullptr;
}

Context const* Context::primary() {
	return primary_;
}

std::size_t Context::width() const {
	return width_;
}

std::size_t Context::height() const {
	return height_;
}

Context::operator GLFWwindow*() const {
	return context_;
}

void Context::set_dimensions(std::size_t width, std::size_t height) {
	width_ = width;
	height_ = height;
}

void Context::init(Type type, std::string const& name, bool shared, float version) {
	LOG("Creating ", (is_visible_ ? "visible" : "invisible"), " context");
	if(!primary_) {
		if(type != Type::Primary)
			throw OutOfOrderInitializationException{"Primary context must be created before any secondary ones"};
		if(!glfwInit())
			throw GLException{"Failed to initialize GLFW"};
		LOG("Context type is primary");
	}

	if(!std::signbit(version)) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<std::size_t>(version));
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<std::size_t>(10.f*std::fmod(version, 1.f)));
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		LOG("OpenGL version ", version, " specified");
	}
	if(!is_visible_)
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		
	context_ = glfwCreateWindow(width_, height_, name.c_str(), nullptr, shared ? primary_->context_ : nullptr);
	
	if(!primary_ && type == Type::Primary) {
		primary_ = this;
		LOG("Primary context set successfully");
	}

	if(!context_)
		throw GLException{"Could not create GLFW window. Most likely, OpenGL version " + std::to_string(version) +" is not supported"};

	glfwMakeContextCurrent(context_);
	glfwSetWindowUserPointer(context_, this);
}

Context* Context::primary_ = nullptr;
