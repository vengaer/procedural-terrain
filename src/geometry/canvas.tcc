template <typename ShaderPolicy>
Canvas<ShaderPolicy>::Canvas(ShaderPolicy policy) : renderer_t{policy} {
	renderer_t::init();
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
