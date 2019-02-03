template <typename ShaderPolicy>
Transform<ShaderPolicy>::Transform(ShaderPolicy policy) : transforms_{std::vector<glm::mat4>{glm::mat4{1.f}}}, policy_{policy} { 
	if constexpr(ShaderPolicy::is_automatic)
		update_model();
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::translate(glm::vec3 direction) {
	add_transform(glm::translate(transforms_.top(), direction));
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::translate(float distance, Axis axis, Sign sign) {
	glm::vec3 translation_vector{0.f};
	translation_vector[enum_value(axis)] = std::pow(-1, enum_value(sign)) * distance;
	translate(translation_vector);
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::scale(glm::vec3 scaling_vector) {
	add_transform(glm::scale(transforms_.top(), scaling_vector));
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::scale(float magnitude, Axis axis) {
	glm::vec3 scaling_vector{1.f};
	scaling_vector[enum_value(axis)] = magnitude;
	scale(scaling_vector);
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::rotate(float degrees, glm::vec3 axis) {
	add_transform(glm::rotate(transforms_.top(), glm::radians(degrees), axis));
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::rotate(float degrees, Axis axis, Sign sign) {
	glm::vec3 basis_vector{0.f};
	basis_vector[enum_value(axis)] = std::pow(-1, enum_value(sign));
	rotate(degrees, basis_vector);
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::undo_last_transform() {
	transforms_.pop();
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::reset_transforms() {
	while(!transforms_.empty())
		transforms_.pop();
}
	
template <typename ShaderPolicy>
glm::mat4 Transform<ShaderPolicy>::model_matrix() const {
	return transforms_.top();
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::add_transform(glm::mat4&& transform) {
	transforms_.push(std::move(transform));
	if constexpr(ShaderPolicy::is_automatic)
		update_model();
}

template <typename ShaderPolicy>
void Transform<ShaderPolicy>::update_model() const {
	EventHandler::upload_model(policy_.shader(), transforms_.top());
}
