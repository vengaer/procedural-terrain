template <typename T>
Renderer<T>::Renderer() : vao_{0u}, vbo_{0u}, idx_buffer_{0u}, idx_size_{0u} {
	static_assert(is_renderable_v<T>, "Type does not fulfill the rendering requirements");
}

template <typename T>
void Renderer<T>::render() const {
	glBindVertexArray(vao_);
	
	glDrawElements(GL_TRIANGLES, idx_size_, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
}

template <typename T>
template <typename... Args>
void Renderer<T>::init(Args&&... args) {
	/* Initialize T */
	static_cast<T&>(*this).init(std::forward<Args>(args)...);

	/* vao */
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	/* vbo */
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	{
		/* Get ref to vertices */
		auto const& vertices = static_cast<T&>(*this).vertices();
		using value_type = get_fundamental_type_t<decltype(vertices)>;

		auto const VALUE_TYPE_SIZE  = sizeof(value_type);

		auto const NUMBER_OF_VERTICES = size(vertices_tag{});
	
		GLuint const TOTAL_SIZE = VALUE_TYPE_SIZE * NUMBER_OF_VERTICES * VERTEX_SIZE;

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
		/* Get ref to indices */
		auto const& indices = static_cast<T&>(*this).indices();
		
		using value_type = get_fundamental_type_t<decltype(indices)>;
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

template <typename T>
constexpr GLuint Renderer<T>::size(vertices_tag) const {
	GLuint container_size;

	if constexpr(is_contiguously_stored_v<decltype(static_cast<T const&>(*this).vertices())>)
		container_size = std::size(static_cast<T const&>(*this).vertices());
	else
		container_size = static_cast<T const&>(*this).vertices_size();

	return container_size;
}

template <typename T>
constexpr GLuint Renderer<T>::size(indices_tag) const {
	GLuint container_size;

	if constexpr(is_contiguously_stored_v<decltype(static_cast<T const&>(*this).indices())>)
		container_size = std::size(static_cast<T const&>(*this).indices());
	else
		container_size = static_cast<T const&>(*this).indices_size();

	return container_size;
}
