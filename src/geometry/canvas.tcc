template <typename ShaderPolicy>
Canvas<ShaderPolicy>::Canvas(Color clear_color) : renderer_t{}, clear_color_{clear_color} {
	renderer_t::init();
}

template <typename ShaderPolicy>
Canvas<ShaderPolicy>::Canvas(ShaderPolicy policy, Color clear_color) : renderer_t{policy}, clear_color_{clear_color} {
	renderer_t::init();
}

template <typename ShaderPolicy>
void Canvas<ShaderPolicy>::render_setup() const {
	Shader::bind_default_framebuffer();
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	Shader::bind_scene_texture();
}

template <typename ShaderPolicy>
void Canvas<ShaderPolicy>::render_cleanup() const {
	//Shader::unbind_scene_texture();
	Shader::bind_main_framebuffer();
	glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
	glEnable(GL_DEPTH_TEST);
}

template <typename ShaderPolicy>
std::array<GLfloat, Canvas<ShaderPolicy>::VERTEX_ARRAY_SIZE> const& Canvas<ShaderPolicy>::vertices() const {
	return vertices_;
}

template <typename ShaderPolicy>
std::array<GLuint, Canvas<ShaderPolicy>::INDICES_ARRAY_SIZE> const& Canvas<ShaderPolicy>::indices() const {
	return indices_;
}

template <typename ShaderPolicy>
std::array<GLfloat, Canvas<ShaderPolicy>::VERTEX_ARRAY_SIZE> const Canvas<ShaderPolicy>::vertices_{
	-1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
	 1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,
	 1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
	-1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f
};

template <typename ShaderPolicy>
std::array<GLuint, Canvas<ShaderPolicy>::INDICES_ARRAY_SIZE> const Canvas<ShaderPolicy>::indices_ {
	0u, 1u, 3u,
	1u, 2u, 3u
};
