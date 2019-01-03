#include "event_handler.h"

void EventHandler::update_perspective(float width, float height) {
	auto [near, far] = instance_->camera_.clipping_plane();

	perspective_ = glm::perspective(glm::radians(instance_->camera_.fov()), width/height, near, far);

	using UC = Shader::Uniform::Construct;
	using UD = Shader::Uniform::Dimension;
	using UT = Shader::Uniform::Type;
	
	for(auto const& id : shader_ids_)
		Shader::upload_uniform<UC::Matrix, UD::_4, UT::FloatPtr>(id, PERSPECTIVE_UNIFORM_NAME, glm::value_ptr(perspective_));
}

void EventHandler::size_callback(GLFWwindow* window, int width, int height) {
	instance_->window_.set_dimensions(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
	glViewport(0, 0, width, height);

	update_perspective(static_cast<float>(width), static_cast<float>(height));
}

void EventHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	using Dir = Camera::Direction;
	Camera& camera = instance_->camera_;

	switch(key) {
		case GLFW_KEY_W:
			camera.translate(Dir::Forward);
			break;
		case GLFW_KEY_S:
			camera.translate(Dir::Backward);
			break;
		case GLFW_KEY_D:
			camera.translate(Dir::Right);
			break;
		case GLFW_KEY_A:
			camera.translate(Dir::Left);
			break;
	}
}

std::string const EventHandler::PERSPECTIVE_UNIFORM_NAME = "perspective";
std::vector<GLuint> EventHandler::shader_ids_{};
bool EventHandler::instantiated_ = false;
glm::mat4 EventHandler::perspective_{};
EventHandler* EventHandler::instance_ = nullptr;
