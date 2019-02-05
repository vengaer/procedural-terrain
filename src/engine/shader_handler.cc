#include "shader_handler.h"

void manual_shader_handler::operator()() const noexcept { }

std::shared_ptr<Shader> const manual_shader_handler::shader() const noexcept {
	return nullptr;
}

void automatic_shader_handler::operator()() const noexcept {
	shader_->enable();
}

std::shared_ptr<Shader> const automatic_shader_handler::shader() const noexcept {
	return shader_;
}
