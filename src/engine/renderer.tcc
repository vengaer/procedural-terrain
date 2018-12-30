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
	
		//auto const NUMBER_OF_VERTICES = size<decltype(vertices), DataCategory::VERTICES>();
		
		GLuint const TOTAL_SIZE = VALUE_TYPE_SIZE * vertices.size() * VERTEX_SIZE;
		//GLuint const TOTAL_SIZE = VALUE_TYPE_SIZE * NUMBER_OF_VERTICES * VERTEX_SIZE;

		/* Upload to gpu */
		glBufferData(GL_ARRAY_BUFFER, TOTAL_SIZE, &vertices[0], GL_STATIC_DRAW);

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
		
		using value_type = get_fundamental_type_t<decltype(indices)>;
		auto const VALUE_TYPE_SIZE = sizeof(value_type);
	
		idx_size_ = static_cast<GLuint>(indices.size());
		//idx_size_ = static_cast<GLuint>(size<decltype(indices), DataCategory::INDICES>());
	
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
template <typename U, Renderer<T>::DataCategory C>
Renderer<T>::size_type<U,C> Renderer<T>::size() {
	size_type<U,C> container_size;
	if constexpr(is_contiguously_stored_v<U>){
		if constexpr(C == DataCategory::VERTICES)
			container_size = std::size(static_cast<T&>(*this).vertices());
		else
			container_size = std::size(static_cast<T&>(*this).indices());
	}
	else{
		if constexpr(C == DataCategory::VERTICES)
			container_size = static_cast<T&>(*this).vertices_size();
		else
			container_size = static_cast<T&>(*this).indices_size();
	}

	return container_size;
}
