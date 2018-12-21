template <typename T>
Renderer<T>::Renderer() : vao_{0u}, vbo_{0u}, idx_buffer_{0u}, idx_size_{0u} {
	static_assert(is_renderable_v<T>, "Type is not renderable\n");
}

template <typename T>
void Renderer<T>::render() const {
	glBindVertexArray(vao_);
	
	glDrawElements(GL_TRIANGLES, idx_size_, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
}

template <typename T>
template <typename... Args>
void Renderer<T>::init(Args... args) {
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
		using value_type = get_value_type_t<decltype(vertices)>;

		auto const VALUE_TYPE_SIZE  = sizeof(value_type);
		
		GLuint const SIZE = VALUE_TYPE_SIZE * vertices.size() * VERTEX_SIZE;

		/* Upload to gpu */
		glBufferData(GL_ARRAY_BUFFER, SIZE, &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); /* Positions */
		glEnableVertexAttribArray(1); /* Normals */
		glEnableVertexAttribArray(2); /* Texture */

		/* Offsets in data */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)(3 * VALUE_TYPE_SIZE));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * VALUE_TYPE_SIZE, (void*)(6 * VALUE_TYPE_SIZE));
	}
	{
		/* Get ref to indices */
		auto const& indices = static_cast<T&>(*this).indices();
		
		using value_type = get_value_type_t<decltype(indices)>;
		auto const VALUE_TYPE_SIZE = sizeof(value_type);
	
		idx_size_ = static_cast<GLuint>(indices.size());
	
		GLuint const SIZE = VALUE_TYPE_SIZE * indices.size();


		/* Upload indices to gpu */
		glGenBuffers(1, &idx_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIZE, &indices[0], GL_STATIC_DRAW);
	}
	
	/* Unbind */
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
