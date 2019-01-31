#include "event_handler.h"

void EventHandler::update_perspective(float width, float height) {
	auto const [near, far] = instance_->camera_->clipping_plane();

	perspective_ = glm::perspective(glm::radians(instance_->camera_->fov()), width/height, near, far);
	
	for(auto const& shader : shaders_) {
		shader->enable();
		shader->upload_uniform(PROJECTION_UNIFORM_NAME, perspective_);
	}
}

void EventHandler::update_view() {
	glm::mat4 const view = instance_->camera_->view();

	for(auto const& shader : shaders_) {
		shader->enable();
		shader->upload_uniform(VIEW_UNIFORM_NAME, view);
	}
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
	auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
	window->set_dimensions(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
	glViewport(0, 0, width, height);

	update_perspective(static_cast<float>(width), static_cast<float>(height));
}

void EventHandler::shader_reload_callback(Shader const& shader) {
	LOG("Reload callback triggered, reuploading uniforms...");
	shader.enable();
	glm::mat4 const view = instance_->camera_->view();
	shader.upload_uniform(PROJECTION_UNIFORM_NAME, perspective_);
	shader.upload_uniform(VIEW_UNIFORM_NAME, view);
	Transform::force_update();
}



std::string const EventHandler::PROJECTION_UNIFORM_NAME = "ufrm_projection";
std::string const EventHandler::VIEW_UNIFORM_NAME = "ufrm_view";
std::vector<std::shared_ptr<Shader>> EventHandler::shaders_{};
bool EventHandler::instantiated_ = false;
glm::mat4 EventHandler::perspective_{};
EventHandler* EventHandler::instance_ = nullptr;
EventHandler::MousePosition EventHandler::mouse_position_{};
