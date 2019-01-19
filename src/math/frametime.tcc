namespace frametime {
	template <typename P0, typename... P1toN>
	void update_shader_time(P0&& head, P1toN... tail);
}

template <typename P0, typename... P1toN>
void frametime::update_shader_time(P0&& head, P1toN... tail) {
	static_cast<Shader>(head).upload_uniform(TIME_UNIFORM_NAME, static_cast<float>(glfwGetTime()));

	if constexpr (sizeof...(P1toN) > 0)
		update_shader_time<P1toN...>(std::forward<P1toN>(tail)...);
}


template <typename... Args>
void frametime::update(Args&&... args) {
	static_assert(all_same_v<Shader, remove_cvref_t<Args>...>, "Arguments must be shaders");
	update_shader_time<Args...>(std::forward<Args>(args)...);

	update();
}
