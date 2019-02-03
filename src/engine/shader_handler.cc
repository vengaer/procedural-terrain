#include "shader_handler.h"

void manual_shader_handler::draw(GLuint vao, GLuint idx_size) const noexcept {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, idx_size, GL_UNSIGNED_INT, static_cast<void*>(0));
	glBindVertexArray(0);
}

std::shared_ptr<Shader> manual_shader_handler::shader() const noexcept {
	return nullptr;
}

void automatic_shader_handler::draw(GLuint vao, GLuint idx_size) const noexcept {
	shader_->enable();
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, idx_size, GL_UNSIGNED_INT, static_cast<void*>(0));
	glBindVertexArray(0);
}

std::shared_ptr<Shader> automatic_shader_handler::shader() const noexcept {
	return shader_;
}
