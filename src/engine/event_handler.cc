#include "blur.h"
#include "context.h"
#include "event_handler.h"
#include "exception.h"
#include "framebuffer.h"
#include "shader.h"
#include "viewport.h"
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
	glfwSetInputMode(context_data, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(context_data, key_callback);
	glfwSetInputMode(context_data, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(context_data, mouse_callback);
	glfwSetWindowSizeCallback(context_data, size_callback);

    Viewport::update();
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

void EventHandler::key_callback(GLFWwindow*, int key, int, int action, int mod_bits) {
	using Dir = Camera::Direction;
	using Speed = Camera::Speed;
    using State = Camera::KeyState;
    auto& camera = instance_->camera_;
	Mods modifiers = static_cast<Mods>(mod_bits);
    
    State state = action == GLFW_RELEASE ? State::Up : State::Down;

    switch(key) {
        case GLFW_KEY_W:
            camera->set_state(Dir::Forward, state);
            break;
        case GLFW_KEY_S:
            camera->set_state(Dir::Backward, state);
            break;
        case GLFW_KEY_D:
            camera->set_state(Dir::Right, state);
            break;
        case GLFW_KEY_A:
            camera->set_state(Dir::Left, state);
            break;
        case GLFW_KEY_SPACE:
            if(action == GLFW_RELEASE) {
                camera->set_state(Dir::Down, State::Up);
                camera->set_state(Dir::Up, State::Up);
            }
            else {
                if((modifiers & Mods::Super) == Mods::Super)
                    camera->set_state(Dir::Down, state);
                else
                    camera->set_state(Dir::Up, state);
            }
            break;
    }
    if((modifiers & Mods::Shift) == Mods::Shift)
        camera->set_state(Speed::Fast);
    else if((modifiers & Mods::Ctrl) == Mods::Ctrl)
        camera->set_state(Speed::Slow);
    else
        camera->set_state(Speed::Default);
    update_view();
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
    Viewport::update();

	Shader::reallocate_textures();
    Framebuffer<1u>::reallocate();
    Framebuffer<2u>::reallocate();
    Framebuffer<1u, TexType::Color | TexType::Depth>::reallocate();
    HorizontalBlur::update_size();
    VerticalBlur::update_size();
}

bool EventHandler::instantiated_ = false;
EventHandler* EventHandler::instance_ = nullptr;
EventHandler::MousePosition EventHandler::mouse_position_{};
