template <typename T, typename ShaderPolicy>
Renderer<T, ShaderPolicy>::Renderer(ShaderPolicy policy) : vao_{0u}, vbo_{0u}, idx_buffer_{0u}, idx_size_{0u}, policy_{policy}  {
	static_assert(is_renderable_v<T>, "Type does not fulfill the rendering requirements");
}

template <typename T, typename ShaderPolicy>
Renderer<T, ShaderPolicy>::~Renderer() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
}

template <typename T, typename ShaderPolicy>
void Renderer<T, ShaderPolicy>::render() const {
	if constexpr(requires_setup(OVERLOAD_RESOLVER))
		static_cast<T const&>(*this).render_setup();

	if constexpr(object_is_transformable(OVERLOAD_RESOLVER)) {
		if(object_has_been_transformed()) {
			policy_.shader()->template upload_uniform<true>(Shader::MODEL_UNIFORM_NAME, get_model_matrix());
			object_has_been_transformed() = false;
		}
	}

	if constexpr(policy_is_automatic(OVERLOAD_RESOLVER))
		policy_();

    bind();
    draw();
    unbind();
	
	if constexpr(requires_cleanup(OVERLOAD_RESOLVER)) 
		static_cast<T const&>(*this).render_cleanup();
}

template <typename T, typename ShaderPolicy>
void Renderer<T, ShaderPolicy>::bind() const {
	glBindVertexArray(vao_);
}   

template <typename T, typename ShaderPolicy>
void Renderer<T, ShaderPolicy>::draw() const {
	glDrawElements(GL_TRIANGLES, idx_size_, GL_UNSIGNED_INT, static_cast<void*>(0));
}

template <typename T, typename ShaderPolicy>
void Renderer<T, ShaderPolicy>::unbind() const {
	glBindVertexArray(0);
}


template <typename T, typename ShaderPolicy>
template <typename... Args>
void Renderer<T, ShaderPolicy>::init(Args&&... args) {
	if constexpr(has_arbitrary_init_v<T>)
		static_cast<T&>(*this).init(std::forward<Args>(args)...); /* Initialize T */

	/* vao */
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	/* vbo */
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	{
		auto const& vertices = static_cast<T&>(*this).vertices(); /* Ref to vertices */
		using value_type = fundamental_type_t<decltype(vertices)>;

		auto const VALUE_TYPE_SIZE  = sizeof(value_type);

		auto const NUMBER_OF_VERTICES = size(vertices_tag{});
	
		GLuint const TOTAL_SIZE = VALUE_TYPE_SIZE * NUMBER_OF_VERTICES;

		/* Upload to gpu */
		glBufferData(GL_ARRAY_BUFFER, TOTAL_SIZE, &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); /* Position */
		glEnableVertexAttribArray(1); /* Normal */
		glEnableVertexAttribArray(2); /* Texture */

		/* Offsets in data */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)(3 * VALUE_TYPE_SIZE));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)(6 * VALUE_TYPE_SIZE));
	}
	{
		auto const& indices = static_cast<T&>(*this).indices(); /* Ref to indices */
		
		using value_type = fundamental_type_t<decltype(indices)>;
		auto const VALUE_TYPE_SIZE = sizeof(value_type);
	
		idx_size_ = size(indices_tag{});
	
		GLuint const TOTAL_SIZE = VALUE_TYPE_SIZE * idx_size_;

		/* Upload indices to gpu */
		glGenBuffers(1, &idx_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, TOTAL_SIZE, &indices[0], GL_STATIC_DRAW);
	}
	
	/* Unbind */
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename T, typename ShaderPolicy>
GLuint Renderer<T, ShaderPolicy>::size(vertices_tag) const {
	std::size_t container_size;

	if constexpr(is_contiguously_stored_v<decltype(static_cast<T const&>(*this).vertices())>)
		container_size = std::size(static_cast<T const&>(*this).vertices());
	else
		container_size = static_cast<T const&>(*this).vertices_size();
	
	if(std::numeric_limits<GLuint>::max() < container_size)
		throw OverflowException{"Size of vertices array too large for OpenGL to handle\n"};
	
	return static_cast<GLuint>(container_size);
}

template <typename T, typename ShaderPolicy>
GLuint Renderer<T, ShaderPolicy>::size(indices_tag) const {
	std::size_t container_size;

	if constexpr(is_contiguously_stored_v<decltype(static_cast<T const&>(*this).indices())>)
		container_size = std::size(static_cast<T const&>(*this).indices());
	else
		container_size = static_cast<T const&>(*this).indices_size();

	if(std::numeric_limits<GLuint>::max() < container_size)
		throw OverflowException{"Size of indices array too large for OpenGL to handle\n"};

	return static_cast<GLuint>(container_size);
}

template <typename T, typename ShaderPolicy>
template <typename U>
auto constexpr Renderer<T, ShaderPolicy>::requires_setup(int) noexcept -> std::remove_reference_t<decltype((void)std::declval<U>().render_setup(), std::declval<bool>())> {
	return true;
}

template <typename T, typename ShaderPolicy>
bool constexpr Renderer<T, ShaderPolicy>::requires_setup(long) noexcept {
	return false;
}

template <typename T, typename ShaderPolicy>
template <typename U>
auto constexpr Renderer<T, ShaderPolicy>::requires_cleanup(int) noexcept -> std::remove_reference_t<decltype((void)std::declval<U>().render_cleanup(), std::declval<bool>())> {
	return true;
}

template <typename T, typename ShaderPolicy>
bool constexpr Renderer<T, ShaderPolicy>::requires_cleanup(long) noexcept {
	return false;
}

template <typename T, typename ShaderPolicy>
template <typename U>
auto constexpr Renderer<T, ShaderPolicy>::policy_is_automatic(int) noexcept -> std::remove_reference_t<decltype((void)U::is_automatic, std::declval<bool>())> {
	return ShaderPolicy::is_automatic;
}

template <typename T, typename ShaderPolicy>
bool constexpr Renderer<T, ShaderPolicy>::policy_is_automatic(long) noexcept {
	return false;
}

template <typename T, typename ShaderPolicy>
template <typename U>
auto constexpr Renderer<T, ShaderPolicy>::object_is_transformable(int) noexcept -> std::remove_reference_t<decltype((void)std::declval<U>().has_been_transformed(), std::declval<bool>())> {
	return true;
}

template <typename T, typename ShaderPolicy>
bool constexpr Renderer<T, ShaderPolicy>::object_is_transformable(long) noexcept {
	return false;
}

template <typename T, typename ShaderPolicy>
bool& Renderer<T, ShaderPolicy>::object_has_been_transformed() const noexcept {
	return static_cast<T const&>(*this).has_been_transformed();
}

template <typename T, typename ShaderPolicy>
glm::mat4 Renderer<T, ShaderPolicy>::get_model_matrix() const noexcept {
	return static_cast<T const&>(*this).model_matrix();
}
