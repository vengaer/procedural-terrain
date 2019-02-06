#include "shader_handler.h"

void automatic_shader_handler::operator()() const noexcept {
	shader_->enable();
}

std::shared_ptr<Shader> const automatic_shader_handler::shader() const noexcept {
	return shader_;
}
