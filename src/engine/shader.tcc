

template <typename... Args>
void Shader::upload_uniform(std::string const& name, Args&&... args) {
	auto it = uniforms_.find(name);
	
	if(it == std::end(uniforms_)) {
		auto result = uniforms_.insert( { name, glGetUniformLocation(program_, name.c_str()) });
		it = result.first;
	}
	GLint location = it->second;	
	
	upload_uniform(location, std::forward<Args>(args)...);
}

template <typename... Args>
void Shader::upload_uniform(GLuint program, std::string const& name, Args&&... args) {	
	GLint location = glGetUniformLocation(program, name.c_str());
	upload_uniform(location, std::forward<Args>(args)...);
}

/* The compiler uses this template to generate functions that at runtime perform a call to the correct
 * version of glUniform and absolutely nothing else */
template <typename... Args>
void Shader::upload_uniform(GLint location, Args&&... args) {
	static_assert(sizeof...(args) <= 4 && sizeof...(args) > 0, "Function must be given 1 to 4 parameters");
	static_assert(all_same_v<Args...>, "All parameters must be of the same type");
	static_assert((!std::is_pointer_v<Args> && ...), "Pointers are not supported. If using glm, pass the entire object rather than calling glm::value_ptr");


	using order_t = uniform::TensorOrder;
	using decay_t = uniform::DecayType;	
	using dim_t   = uniform::TensorDimensions;

	using uniform::deduce;

	order_t constexpr order = deduce<Args...>::tensor_order();
	decay_t constexpr decay = deduce<Args...>::decay_type();
	dim_t   constexpr dim   = deduce<Args...>::tensor_dimensions();

	enum_fold<order_t, decay_t, dim_t> fold;

	using first_unqualified_t = remove_cvref_t<nth_type_t<0, Args...>>;

	/* Args are fundamental types */
	if constexpr (std::is_fundamental_v<first_unqualified_t>) {
		/* GLfloat uniforms */
		if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::Float, dim_t::_1))
			glUniform1f(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Float, dim_t::_2))
			glUniform2f(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Float, dim_t::_3))
			glUniform3f(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Float, dim_t::_4))
			glUniform4f(location, std::forward<Args>(args)...);
		/* GLint uniforms */
		else if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::Int, dim_t::_1))
			glUniform1i(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Int, dim_t::_2))
			glUniform2i(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Int, dim_t::_3))
			glUniform3i(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Int, dim_t::_3))
			glUniform4i(location, std::forward<Args>(args)...);
		/* GLuint uniforms */
		else if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::Uint, dim_t::_1))
			glUniform1ui(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Uint, dim_t::_2))
			glUniform2ui(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Uint, dim_t::_3))
			glUniform3ui(location, std::forward<Args>(args)...);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::Uint, dim_t::_4))
			glUniform4ui(location, std::forward<Args>(args)...);
	}
	/* Args are glm types, safe to call value_ptr */
	else {
		static_assert(sizeof...(args) == 1, "Only a single non-fundamental type may be passed");

		auto const* value_ptr = glm::value_ptr(nth_value<0, Args...>{}(std::forward<Args>(args)...));

		/* GLfloat* uniforms */
		if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::FloatPtr, dim_t::_1)) 		/* Will currently never be true */
			glUniform1fv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::FloatPtr, dim_t::_2))
			glUniform2fv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::FloatPtr, dim_t::_3))
			glUniform3fv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::FloatPtr, dim_t::_4))
			glUniform4fv(location, 1u, value_ptr);
		/* GLint* uniforms */
		else if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::IntPtr, dim_t::_1)) 		/* Will currently never be true */
			glUniform1iv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::IntPtr, dim_t::_2))
			glUniform2iv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::IntPtr, dim_t::_3))
			glUniform3iv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::IntPtr, dim_t::_4))
			glUniform4iv(location, 1u, value_ptr);
		/* GLuint* uniforms */
		else if constexpr (fold(order, decay, dim) == fold(order_t::_0th, decay_t::UintPtr, dim_t::_1))		/* Will currently never be true */
			glUniform1uiv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::UintPtr, dim_t::_2))
			glUniform2uiv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::UintPtr, dim_t::_3))
			glUniform3uiv(location, 1u, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_1st, decay_t::UintPtr, dim_t::_4))
			glUniform4uiv(location, 1u, value_ptr);
		/* nxn matrix uniforms */ 
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_2))
			glUniformMatrix2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_3))
			glUniformMatrix3fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_4))
			glUniformMatrix4fv(location, 1u, GL_FALSE, value_ptr);	
		/* nxm matrix uniforms */
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_2x3))
			glUniformMatrix2x3fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_3x2))
			glUniformMatrix3x2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_2x4))
			glUniformMatrix2x4fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_4x2))
			glUniformMatrix4x2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_3x4))
			glUniformMatrix3x4fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (fold(order, decay, dim) == fold(order_t::_2nd, decay_t::FloatPtr, dim_t::_4x3))
			glUniformMatrix4x3fv(location, 1u, GL_FALSE, value_ptr);
		else
			throw BadUniformParametersException{"Parameters do not match any known uniform"};
	}
}
