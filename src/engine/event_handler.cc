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
	using State = KeyModifiers::State;

	auto& camera = instance_->camera_;
	KeyModifiers const mods = modifier_states(mod_bits);
	
	Speed speed = mods.shift == State::Active ?
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
			if(mods.ctrl == State::Active)
				camera->translate(Dir::Down, speed);
			else
				camera->translate(Dir::Up, speed);
			update_view();
			break;
	}

}

EventHandler::KeyModifiers EventHandler::modifier_states(int mod_bits) {	
	using State = KeyModifiers::State;
	KeyModifiers mod;

	switch(mod_bits) {
		case 1:
			mod = { State::Active, State::Inactive, State::Inactive, State::Inactive };
			break;
		case 2:
			mod = { State::Inactive, State::Active, State::Inactive, State::Inactive };
			break;
		case 3:
			mod = { State::Active, State::Active, State::Inactive, State::Inactive };
			break;
		case 4:
			mod = { State::Inactive, State::Inactive, State::Active, State::Inactive };
			break;
		case 5:
			mod = { State::Active, State::Inactive, State::Active, State::Inactive };
			break;
		case 6:
			mod = { State::Inactive, State::Active, State::Active, State::Inactive };
			break;
		case 7:
			mod = { State::Active, State::Active, State::Active, State::Inactive };
			break;
		case 8:
			mod = { State::Inactive, State::Inactive, State::Inactive, State::Active };
			break;
		case 9:
			mod = { State::Active, State::Inactive, State::Inactive, State::Active };
			break;
		case 10:
			mod = { State::Inactive, State::Active, State::Inactive, State::Active };
			break;
		case 11:
			mod = { State::Active, State::Active, State::Inactive, State::Active };
			break;
		case 12:
			mod = { State::Inactive, State::Inactive, State::Active, State::Active };
			break;
		case 13:
			mod = { State::Active, State::Inactive, State::Active, State::Active };
			break;
		case 14:
			mod = { State::Inactive, State::Active, State::Active, State::Active };
			break;
		case 15:
			mod = { State::Active, State::Active, State::Active, State::Active };
			break;
	}
	return mod;
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
