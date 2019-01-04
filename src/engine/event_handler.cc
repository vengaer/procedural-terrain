#include "event_handler.h"

void EventHandler::update_perspective(float width, float height) {
	auto const [near, far] = instance_->camera_.clipping_plane();

	perspective_ = glm::perspective(glm::radians(instance_->camera_.fov()), width/height, near, far);

	auto constexpr Matrix = Shader::Uniform::Construct::Matrix;
	auto constexpr _4 = Shader::Uniform::Dimension::_4;
	auto constexpr FloatPtr = Shader::Uniform::Type::FloatPtr;
	
	for(auto const& id : shader_ids_)
		Shader::upload_uniform<Matrix, _4, FloatPtr>(id, PROJECTION_UNIFORM_NAME, glm::value_ptr(perspective_));
}

void EventHandler::update_view() {
	glm::mat4 const view = instance_->camera_.view();

	auto constexpr Matrix = Shader::Uniform::Construct::Matrix;
	auto constexpr _4 = Shader::Uniform::Dimension::_4;
	auto constexpr FloatPtr = Shader::Uniform::Type::FloatPtr;

	for(auto const& id : shader_ids_)
		Shader::upload_uniform<Matrix, _4, FloatPtr>(id, VIEW_UNIFORM_NAME, glm::value_ptr(view));
}

void EventHandler::size_callback(GLFWwindow* window, int width, int height) {
	instance_->window_.set_dimensions(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
	glViewport(0, 0, width, height);

	update_perspective(static_cast<float>(width), static_cast<float>(height));
}

void EventHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mod_bits) {
	using Dir = Camera::Direction;
	using Speed = Camera::Speed;
	using State = KeyModifiers::State;

	Camera& camera = instance_->camera_;
	KeyModifiers const mods = modifier_states(mod_bits);
	
	Speed speed = mods.shift == State::Active ?
					Speed::Fast :
					Speed::Default;

	switch(key) {
		case GLFW_KEY_W:
			camera.translate(Dir::Forward, speed);
			update_view();
			break;
		case GLFW_KEY_S:
			camera.translate(Dir::Backward, speed);
			update_view();
			break;
		case GLFW_KEY_D:
			camera.translate(Dir::Right, speed);
			update_view();
			break;
		case GLFW_KEY_A:
			camera.translate(Dir::Left, speed);
			update_view();
			break;
		case GLFW_KEY_SPACE:
			if(mods.ctrl == State::Active)
				camera.translate(Dir::Down, speed);
			else
				camera.translate(Dir::Up, speed);
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

std::string const EventHandler::PROJECTION_UNIFORM_NAME = "ufrm_projection";
std::string const EventHandler::VIEW_UNIFORM_NAME = "ufrm_view";
std::vector<GLuint> EventHandler::shader_ids_{};
bool EventHandler::instantiated_ = false;
glm::mat4 EventHandler::perspective_{};
EventHandler* EventHandler::instance_ = nullptr;
