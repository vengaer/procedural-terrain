template <typename... Args>
EventHandler::EventHandler(std::shared_ptr<Camera> const& camera, Args const&... args) : camera_{camera} {
	init(args...);
}

template <typename... Args>
void EventHandler::instantiate(std::shared_ptr<Camera> const& camera, Args const&... args) {
	static_assert(all_same_v<std::shared_ptr<Shader>, remove_cvref_t<Args>...>, "Parameter pack may only include std::shared_ptr<Shader>");

	if(instantiated_){
		ERR_LOG_WARN("Multiple attempts to instantiate event handler");
		return;
	}

	instantiated_ = true;
	
	static EventHandler handler(camera, args...);

	instance_ = &handler;
	update_perspective();
	update_view();
}

template <typename... Args>
void EventHandler::init(Args const&... args) {
	if constexpr(sizeof...(Args))
		inserter<std::unordered_map<std::shared_ptr<Shader>, glm::mat4>>{}(shader_model_pairs_, std::make_pair(args, glm::mat4{})...);

	auto* context = glfwGetCurrentContext();

	glfwSetInputMode(context, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(context, key_callback);
	glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(context, mouse_callback);
	glfwSetWindowSizeCallback(context, size_callback);
}

template <typename... Args>
void EventHandler::append_shaders(Args const&... args) {
	static_assert(sizeof...(Args) && all_same_v<std::shared_ptr<Shader>, remove_cvref_t<Args>...>, "Parameter pack may only include std::shared_ptr<Shader>");
	
	shader_model_pairs_.reserve(shader_model_pairs_.size() + sizeof...(args));

	inserter<std::unordered_map<std::shared_ptr<Shader>, glm::mat4>>{}(shader_model_pairs_, std::make_pair(args, glm::mat4{})...);
}

