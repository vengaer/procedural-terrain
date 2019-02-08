#include "context.h"
#include "event_handler.h"
#include "exception.h"
#include "shader.h"
#include <cstddef>
#include <glm/glm.hpp>

EventHandler::EventHandler(std::shared_ptr<Camera> const& camera) : camera_{camera} {
	init();
}

void EventHandler::instantiate(std::shared_ptr<Camera> const& camera) {

	if(instantiated_){
		ERR_LOG_WARN("Multiple attempts to instantiate event handler");
		return;
	}
	LOG("Instantiating event handler");

	instantiated_ = true;
	
	static EventHandler handler(camera);

	instance_ = &handler;
	update_perspective();
	update_view();
}

void EventHandler::init() {
	auto* primary_context = static_cast<Context*>(glfwGetWindowUserPointer(glfwGetCurrentContext()))->primary();
	if(!primary_context)
		throw OutOfOrderInitializationException{"Event handler cannot be instantiated before the primary context is created."};
	auto* context_data = static_cast<GLFWwindow*>(*primary_context);

	LOG("Setting callbacks");
	glfwSetInputMode(context_data, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(context_data, key_callback);
	glfwSetInputMode(context_data, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(context_data, mouse_callback);
	glfwSetWindowSizeCallback(context_data, size_callback);
}


void EventHandler::update_perspective() {
	auto const [near, far] = instance_->camera_->clipping_plane();
	auto* context = static_cast<Context*>(glfwGetWindowUserPointer(glfwGetCurrentContext()))->primary();

	auto perspective = glm::perspective(glm::radians(instance_->camera_->fov()), 
									static_cast<float>(context->width())/static_cast<float>(context->height()), 
									near, 
									far);
	
	Shader::template upload_to_all<true>(Shader::PROJECTION_UNIFORM_NAME, perspective);
}

void EventHandler::update_view() {
	glm::mat4 const view = instance_->camera_->view();
	Shader::template upload_to_all<true>(Shader::VIEW_UNIFORM_NAME, view);
}

void EventHandler::key_callback(GLFWwindow*, int key, int, int, int mod_bits) {
	using Dir = Camera::Direction;
	using Speed = Camera::Speed;
	Mods modifiers = static_cast<Mods>(mod_bits);

	auto& camera = instance_->camera_;
	
	Speed speed = ((modifiers & Mods::shift) == Mods::shift) ?
													Speed::Fast :
													Speed::Default;

	switch(key) {
		case GLFW_KEY_W:
			camera->translate(Dir::Forward, speed);
			update_view();
			break;
		case GLFW_KEY_S:
			camera->translate(Dir::Backward, speed);
			update_view();
			break;
		case GLFW_KEY_D:
			camera->translate(Dir::Right, speed);
			update_view();
			break;
		case GLFW_KEY_A:
			camera->translate(Dir::Left, speed);
			update_view();
			break;
		case GLFW_KEY_SPACE:
			if((modifiers & Mods::ctrl) == Mods::ctrl)
				camera->translate(Dir::Down, speed);
			else
				camera->translate(Dir::Up, speed);
			update_view();
			break;
	}

}


void EventHandler::mouse_callback(GLFWwindow*, double x, double y) {
	double delta_x = glm::clamp(x - mouse_position_.x, -5.0, 5.0);
	double delta_y = glm::clamp(y - mouse_position_.y, -5.0, 5.0);

	instance_->camera_->rotate(static_cast<float>(delta_x), static_cast<float>(delta_y));

	mouse_position_ = { x, y };
	update_view();
}

void EventHandler::size_callback(GLFWwindow*, int width, int height) {
	auto* context = static_cast<Context*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
	context->set_dimensions(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
	glViewport(0, 0, width, height);

	update_perspective();
}

bool EventHandler::instantiated_ = false;
EventHandler* EventHandler::instance_ = nullptr;
EventHandler::MousePosition EventHandler::mouse_position_{};
