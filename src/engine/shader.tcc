template <typename... Sources>
Shader::Shader(Sources const&... src) : program_{}, uniforms_{}, sources_(sizeof...(Sources)/2) {
	static_assert(sizeof...(Sources) % 2 == 0, "Arguments must be given in \"pairs\" of std::string (or something convertible to it) and Shader::Type");
	static_assert(even_parameters_acceptable<Sources...>(even_index_sequence_for<Sources...>{}), "Even arguments must be convertible to std::string");
	static_assert(odd_parameters_acceptable<Sources...>(odd_index_sequence_for<Sources...>{}),   "Odd arguments must be of type Shader::Type");

	generate_source<0u>(src...);
	init<sizeof...(Sources)/2>();
}
	
template <typename... Args, std::size_t... Is>
bool constexpr Shader::even_parameters_acceptable(even_index_sequence<Is...>) {
	return (std::is_convertible_v<remove_cvref_t<nth_type_t<Is, Args...>>, std::string> && ...);
}

template <typename... Args, std::size_t... Is>
bool constexpr Shader::odd_parameters_acceptable(odd_index_sequence<Is...>) {
	return (std::is_same_v<remove_cvref_t<nth_type_t<Is, Args...>>, Type> && ...);
}

template <std::size_t N, typename... Sources>
void Shader::generate_source(std::string const& path, Type type, Sources const&... rest) {
	sources_[N] = {path, type};
	if constexpr(sizeof...(Sources))
		generate_source<N+1u>(rest...);
}

template <std::size_t N>
void Shader::init() {
	std::array<GLuint, N> shader_ids;
	Result<GLuint, std::string> result;

	for(auto [idx, id] : enumerate(shader_ids)) {
		auto path = sources_[idx].path.string();

		LOG("Reading source ", sources_[idx].path.stem().string());
		auto [outcome, data] = read_source(path);
		auto& [error_type, content] = data;

		if(outcome == Outcome::Failure) {
			for(auto i = 0u; i < idx; i++)
				glDeleteShader(shader_ids[i]);

			if(error_type == ErrorType::FileIO)
				throw FileIOException{content};
			else if(error_type == ErrorType::Include)
				throw ShaderIncludeException{content};
		}
	
		LOG("Compiling ", sources_[idx].path.stem().string());
		result = compile(content, sources_[idx].type);
		id = std::get<0>(result.data);

		if(result.outcome == Outcome::Failure) {
			for(auto i = 0u; i < idx + 1; i++)
				glDeleteShader(shader_ids[i]);
			throw ShaderCompilationException{std::get<1>(result.data)};
		}
	}
	LOG("Linking program");
	auto [outcome, data] = link(shader_ids);

	if(outcome == Outcome::Failure)
		throw ShaderLinkingException{std::get<std::string>(data)};

	std::lock_guard<std::mutex> lock{ics_mutex_};
	program_ = std::get<GLuint>(data);
	LOG("Linking successful, assigning new id ", program_);

	LOG("Marking instance for automatic updating");
	instances_.push_back(std::ref(*this));


	if(halt_execution_) {
		LOG("Creating separate thread for execution");
		halt_execution_ = false;
		updater_thread_ = std::thread(monitor_source_files);
		LOG("Thread with id", updater_thread_.get_id(), " successfully created");
	}
}

template <typename T, typename = std::enable_if_t<is_container_v<T>>>
Result<std::variant<GLuint, std::string>> Shader::link(T const& ids) {
	GLuint program_id = glCreateProgram();

	for(auto id : ids)
		glAttachShader(program_id, id);

	glLinkProgram(program_id);

	auto result = assert_shader_status_ok(program_id, StatusQuery::Link);

	for(auto id : ids) {
		glDetachShader(program_id, id);
		glDeleteShader(id);
	}

	if(result.outcome == Outcome::Failure)
		return { result.outcome, result.data.value() };

	return { result.outcome, program_id };
}


template <typename... Args>
void Shader::upload_uniform(std::string const& name, Args&&... args) const {
	auto it = uniforms_.find(name);
	
	if(it == std::end(uniforms_)) {
		auto result = uniforms_.emplace(name, glGetUniformLocation(program_, name.c_str()));
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

	bind_enum<order_t, decay_t, dim_t> bind;

	using first_unqualified_t = remove_cvref_t<nth_type_t<0, Args...>>;

	/* Args are fundamental types */
	if constexpr (std::is_fundamental_v<first_unqualified_t>) {
		/* GLfloat uniforms */
		if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::Float, dim_t::_1))
			glUniform1f(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Float, dim_t::_2))
			glUniform2f(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Float, dim_t::_3))
			glUniform3f(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Float, dim_t::_4))
			glUniform4f(location, std::forward<Args>(args)...);
		/* GLint uniforms */
		else if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::Int, dim_t::_1))
			glUniform1i(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Int, dim_t::_2))
			glUniform2i(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Int, dim_t::_3))
			glUniform3i(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Int, dim_t::_3))
			glUniform4i(location, std::forward<Args>(args)...);
		/* GLuint uniforms */
		else if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::Uint, dim_t::_1))
			glUniform1ui(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Uint, dim_t::_2))
			glUniform2ui(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Uint, dim_t::_3))
			glUniform3ui(location, std::forward<Args>(args)...);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::Uint, dim_t::_4))
			glUniform4ui(location, std::forward<Args>(args)...);
	}
	/* Args are glm types, safe to call glm::value_ptr */
	else {
		static_assert(sizeof...(args) == 1, "Only a single non-fundamental type may be passed");

		auto const* value_ptr = glm::value_ptr(nth_value<0, Args...>{}(std::forward<Args>(args)...));

		/* GLfloat* uniforms */
		if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::FloatPtr, dim_t::_1)) 		/* Will currently never be true */
			glUniform1fv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::FloatPtr, dim_t::_2))
			glUniform2fv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::FloatPtr, dim_t::_3))
			glUniform3fv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::FloatPtr, dim_t::_4))
			glUniform4fv(location, 1u, value_ptr);
		/* GLint* uniforms */
		else if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::IntPtr, dim_t::_1)) 		/* Will currently never be true */
			glUniform1iv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::IntPtr, dim_t::_2))
			glUniform2iv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::IntPtr, dim_t::_3))
			glUniform3iv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::IntPtr, dim_t::_4))
			glUniform4iv(location, 1u, value_ptr);
		/* GLuint* uniforms */
		else if constexpr (bind(order, decay, dim) == bind(order_t::_0th, decay_t::UintPtr, dim_t::_1))		/* Will currently never be true */
			glUniform1uiv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::UintPtr, dim_t::_2))
			glUniform2uiv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::UintPtr, dim_t::_3))
			glUniform3uiv(location, 1u, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_1st, decay_t::UintPtr, dim_t::_4))
			glUniform4uiv(location, 1u, value_ptr);
		/* nxn matrix uniforms */ 
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_2))
			glUniformMatrix2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_3))
			glUniformMatrix3fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_4))
			glUniformMatrix4fv(location, 1u, GL_FALSE, value_ptr);	
		/* nxm matrix uniforms */
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_2x3))
			glUniformMatrix2x3fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_3x2))
			glUniformMatrix3x2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_2x4))
			glUniformMatrix2x4fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_4x2))
			glUniformMatrix4x2fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_3x4))
			glUniformMatrix3x4fv(location, 1u, GL_FALSE, value_ptr);
		else if constexpr (bind(order, decay, dim) == bind(order_t::_2nd, decay_t::FloatPtr, dim_t::_4x3))
			glUniformMatrix4x3fv(location, 1u, GL_FALSE, value_ptr);
		else
			throw BadUniformParametersException{"Parameters do not match any known uniform"};
	}
}
