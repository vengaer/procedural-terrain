template <typename... Args>
EventHandler::EventHandler(Window& window, Camera& camera, Args&&... args) : camera_{camera}, window_{window} { 
	init(std::forward<Args>(args)...);
}


template <typename... Args>
void EventHandler::instantiate(Window& window, Camera& camera, Args&&... args) {
	static_assert(all_same_v<Shader, remove_cvref_t<Args>...>, "Parameter pack may only include Shader instances");

	if(instantiated_){
		std::cerr << "Warning, multiple attempts to instantiate event handler\n";
		return;
	}

	instantiated_ = true;
	
	static EventHandler handler(window, camera, std::forward<Args>(args)...);

	instance_ = &handler;
	update_perspective(static_cast<float>(window.width()), static_cast<float>(window.height()));
	update_view();
}

template <typename... Args>
void EventHandler::init(Args&&... args) {
	inserter<std::vector<GLuint>>{}(shader_ids_, std::forward<decltype(std::declval<Args>().program_id())...>(args.program_id())...);

	glfwSetWindowSizeCallback(window_.glfw_window(), size_callback);
	glfwSetInputMode(window_.glfw_window(), GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window_.glfw_window(), key_callback);
}

template <typename... Args>
void EventHandler::append_shaders(Args&&... args) {
	static_assert(all_same_v<Shader, remove_cvref_t<Args>...>, "Parameter pack may only include Shader instances");
	
	shader_ids_.reserve(shader_ids_.capacity() + sizeof...(args));

	inserter<std::vector<GLuint>>{}(shader_ids_, std::forward<decltype(std::declval<Args>().program_id())...>(args.program_id())...);;
}
