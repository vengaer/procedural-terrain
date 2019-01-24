template <typename... Args>
EventHandler::EventHandler(std::shared_ptr<Camera> const& camera, Args const&... args) : camera_{camera} {
	init(args...);
}

template <typename... Args>
void EventHandler::instantiate(std::shared_ptr<Camera> const& camera, Args const&... args) {
	static_assert(all_same_v<std::shared_ptr<Shader>, remove_cvref_t<Args>...>, "Parameter pack may only include std::shared_ptr<Shader>");

	if(instantiated_){
		std::cerr << "Warning, multiple attempts to instantiate event handler\n";
		return;
	}

	instantiated_ = true;
	
	static EventHandler handler(camera, args...);

	instance_ = &handler;
	auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
	update_perspective(static_cast<float>(window->width()), static_cast<float>(window->height()));
	update_view();
}

template <typename... Args>
void EventHandler::init(Args const&... args) {
	inserter<std::vector<std::shared_ptr<Shader>>>{}(shaders_, args...);
	auto* context = glfwGetCurrentContext();

	glfwSetInputMode(context, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(context, key_callback);
	glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(context, mouse_callback);
	glfwSetWindowSizeCallback(context, size_callback);
	Shader::set_reload_callback(shader_reload_callback);
}

template <typename... Args>
void EventHandler::append_shaders(Args const&... args) {
	static_assert(all_same_v<std::shared_ptr<Shader>, remove_cvref_t<Args>...>, "Parameter pack may only include std::shared_ptr<Shader>");
	
	shaders_.reserve(shaders_.capacity() + sizeof...(args));

	inserter<std::vector<std::shared_ptr<Shader>>>{}(shaders_, args...);
}
