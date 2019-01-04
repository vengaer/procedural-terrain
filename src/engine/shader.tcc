

template <Shader::Uniform::Construct UC,
		  Shader::Uniform::Dimension UD,
		  Shader::Uniform::Type UT,
		  typename... Args>
void Shader::upload_uniform(std::string const& name, Args&&... args) {
	auto it = uniforms_.find(name);
	
	if(it == std::end(uniforms_)) {
		auto result = uniforms_.insert( { name, glGetUniformLocation(program_, name.c_str()) });
		it = result.first;
	}
	GLint location = it->second;	
	
	upload_uniform_impl<UC, UD, UT>(location, std::forward<Args>(args)...);
}

template <Shader::Uniform::Construct UC,
		  Shader::Uniform::Dimension UD,
		  Shader::Uniform::Type UT,
		  typename... Args>
void Shader::upload_uniform(GLuint program, std::string const& name, Args&&... args) {	
	GLint location = glGetUniformLocation(program, name.c_str());
	upload_uniform_impl<UC, UD, UT>(location, std::forward<Args>(args)...);
}
	

template <Shader::Uniform::Construct UC,
		  Shader::Uniform::Dimension UD,
		  Shader::Uniform::Type UT,
		  typename... Args>
void Shader::upload_uniform_impl(GLint location, Args&&... args) {
	static_assert(all_same_v<Args...>, "All parameters in uniform must be of the same type");
	static_assert((is_convertible_to_one_of_v<std::remove_pointer_t<Args>, 
											  GLfloat, 
											  GLint, 
											  GLuint> && ...),
					"All parameters in pack must be convertible to GLfloat, GLint or GLuint");
	
	using C = Uniform::Construct;
	using T = Uniform::Type;
	using D = Uniform::Dimension;


	if constexpr(UT == T::Float || UT == T::Int || UT == T::Uint)
		static_assert(enum_value(UD) == sizeof...(args), "Parameter pack does not match specified uniform");
	else
		static_assert(sizeof...(args) == 1, "Parameter pack should inlude only one parameter");

	fold_enums<C,T,D> fold;

	/* GLfloat uniforms */
	if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Float, D::_1))
		glUniform1f(location, std::forward<Args>(args)...);
	else if constexpr (fold(UC, UT, UD) == fold(C::Vector, T::Float, D::_2))
		glUniform2f(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Float, D::_3))
		glUniform3f(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Float, D::_4))
		glUniform4f(location, std::forward<Args>(args)...);
	/* GLint uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Int, D::_1))
		glUniform1i(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Int, D::_2))
		glUniform2i(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Int, D::_3))
		glUniform3i(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Int, D::_4))
		glUniform4i(location, std::forward<Args>(args)...);
	/* GLuint uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Uint, D::_1))
		glUniform1ui(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Uint, D::_2))
		glUniform2ui(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Uint, D::_3))
		glUniform3ui(location, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::Uint, D::_4))
		glUniform4ui(location, std::forward<Args>(args)...);
	/* GLfloat* uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::FloatPtr, D::_1))
		glUniform1fv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::FloatPtr, D::_2))
		glUniform2fv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::FloatPtr, D::_3))
		glUniform3fv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::FloatPtr, D::_4))
		glUniform4fv(location, 1u, std::forward<Args>(args)...);
	/* GLint* uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::IntPtr, D::_1))
		glUniform1iv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::IntPtr, D::_2))
		glUniform2iv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::IntPtr, D::_3))
		glUniform3iv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::IntPtr, D::_4))
		glUniform4iv(location, 1u, std::forward<Args>(args)...);
	/* GLuint* uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::UintPtr, D::_1))
		glUniform1uiv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::UintPtr, D::_2))
		glUniform2uiv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::UintPtr, D::_3))
		glUniform3uiv(location, 1u, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Vector, T::UintPtr, D::_4))
		glUniform4uiv(location, 1u, std::forward<Args>(args)...);
	/* nxn matrix uniforms */ 
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_2))
		glUniformMatrix2fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_3))
		glUniformMatrix3fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_4))
		glUniformMatrix4fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	/* nxm matrix uniforms */
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_2x3))
		glUniformMatrix2x3fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_3x2))
		glUniformMatrix3x2fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_2x4))
		glUniformMatrix2x4fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_4x2))
		glUniformMatrix4x2fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_3x4))
		glUniformMatrix3x4fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else if constexpr(fold(UC, UT, UD) == fold(C::Matrix, T::FloatPtr, D::_4x3))
		glUniformMatrix4x3fv(location, 1u, GL_FALSE, std::forward<Args>(args)...);
	else
		throw BadUniformParametersException{"Parameters do not match any known uniform"};
}
