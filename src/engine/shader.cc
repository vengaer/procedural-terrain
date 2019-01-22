#include "shader.h"

Shader::Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2) : program_{init(shader1, type1, shader2, type2)}, uniforms_{}, source_{} {}

void Shader::enable() const {
	enable(program_);
}

void Shader::enable(GLuint program) {
	glUseProgram(program);
}

void Shader::disable() {
	glUseProgram(0);
}

GLuint Shader::program_id() const {
	return program_;
}

Shader::Source Shader::generate_source_info(std::string const& shader1, Type type1, std::string const& shader2, Type) {
	namespace fs = std::filesystem;
	Source result;
	result.vertex   = type1 == Type::Vertex ? 
								shader1 :
								shader2 ;
	result.fragment = type1 == Type::Fragment ?
								shader1 :
								shader2 ;
	auto [outcome, tuple] = get_last_write_times(result);
	auto& [message, vert_time, frag_time] = tuple;

	if(outcome == Outcome::Failure)
		throw FileSystemException{ message };
	result.vert_write = vert_time;
	result.frag_write = frag_time;

	return result;
}

Shader::Result<std::string,
			   std::filesystem::file_time_type,
			   std::filesystem::file_time_type
> Shader::get_last_write_times(Source const& source) noexcept {
	using namespace std::string_literals;
	namespace fs = std::filesystem;
	std::error_code err;

	auto vert_time = fs::last_write_time(source.vertex, err);
	if(err)
		return { Outcome::Failure, std::make_tuple(err.message(), fs::file_time_type{}, fs::file_time_type{}) };

	auto frag_time = fs::last_write_time(source.fragment, err);
	if(err)
		return { Outcome::Failure, std::make_tuple(err.message(), fs::file_time_type{}, fs::file_time_type{}) };
	
	return { Outcome::Success, std::make_tuple(""s, vert_time, frag_time) };
}

GLuint Shader::init(std::string const& shader1, Type type1, std::string const& shader2, Type type2){
	if(type1 == type2)
		throw ArgumentMismatchException{"Provided shader types must differ\n"};
	//generate_source_info(shader1, type1, shader2, type2);
	
	std::array<GLuint, 2> shader_ids;
	Result<GLuint, std::string> result;
	{
		bind_enum<Outcome, ErrorType> bind;
		{
			auto [outcome, tuple] = read_source(shader1);
			auto& [error_type, data] = tuple;

			if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO))
				throw FileIOException{data};
			else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include))
				throw ShaderIncludeException{data};

			result = compile(std::move(data), type1);
			if(result.outcome == Outcome::Failure) {
				glDeleteShader(std::get<0>(result.data));
				throw ShaderCompilationException{std::get<1>(result.data)};
			}

			shader_ids[enum_value(type1)] = std::get<0>(result.data);
		}
		auto [outcome, tuple] = read_source(shader2);
		auto& [error_type, data] = tuple;

		if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO)) {
			glDeleteShader(std::get<0>(result.data));
			throw FileIOException{data};
		}
		else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include)) {
			glDeleteShader(std::get<0>(result.data));
			throw ShaderIncludeException{data};
		}

		result = compile(std::move(data), type2);
		if(result.outcome == Outcome::Failure) {
			glDeleteShader(shader_ids[enum_value(type1)]);
			glDeleteShader(std::get<0>(result.data));
			throw ShaderCompilationException{std::get<1>(result.data)};
		}
		shader_ids[enum_value(type2)] = std::get<0>(result.data);
	}

	result = link(shader_ids[enum_value(Type::Vertex)], shader_ids[enum_value(Type::Fragment)]);

	if(result.outcome == Outcome::Failure)
		throw ShaderLinkingException{std::get<1>(result.data)};
	
	return std::get<0>(result.data);	
}


Shader::Result<Shader::ErrorType, std::string> Shader::read_source(std::string const& source){
	/* Every time an include directive is found, push stream with file to be included onto
 	 * the stack */
	std::stack<std::ifstream, std::vector<std::ifstream>> streams;

	streams.push(std::ifstream{source});
	
	if(!streams.top().is_open())
		return { Outcome::Failure, std::make_tuple(ErrorType::FileIO, "Unable to open " + source + " for reading\n") };

	std::string line;	
	std::ostringstream contents;
	Result<ErrorType, std::string> result;
	
	while(!streams.empty()){

		while(std::getline(streams.top(), line)){
			if(auto idx = line.find(" "); line.substr(0, idx) == "#include"){

				/* Max include depth reached, to prevent infinite recursion */
				if(streams.size() >= depth_){
					LOG_WARN("Max include depth reached, omitting ",  line, " included in file ", source);
					E_LOG_WARN("Max include depth reached, omitting ",  line, " included in file ", source);
					continue;
				}
				
				/* Process include directive */
				result = process_include_directive(line, source, idx);

				if(result.outcome == Outcome::Failure)
					return result;

				std::ifstream file{std::get<1>(result.data)};
				
				if(!file.is_open())
					return { Outcome::Failure, std::make_tuple(ErrorType::FileIO, "Unable to open file " + std::get<1>(result.data) + " cincluded (possibly recursively) from file " + source) };

				/* c/c++ style header guard to prevent redefinition */		
				std::string header_guard = format_header_guard(std::get<1>(result.data));

				/* Append header guard */
				contents << "#ifndef " << header_guard << "\n#define " << header_guard << "\n";

				streams.push(std::move(file));
				
			}
			/* Version already specified by root file, skip */
			else if(streams.size() > 1 && line.substr(0, line.find(" ")) == "#version")
				continue;
			/* Add line to source */
			else
				contents << line << "\n";
			
		}
		streams.top().close();
		streams.pop();

		/* End c/c++ style header guard */
		if(!streams.empty())
			contents << "#endif\n";
	}

	return { Outcome::Success, std::make_tuple(ErrorType::None, contents.str()) };	
}

std::string Shader::format_header_guard(std::string path) {
	
	/* Get file name */
	if(path.find('/') != std::string::npos)
		path = path.substr(path.find_last_of('/')+1, path.size() -1);
	
	std::transform(std::begin(path), std::end(path), std::begin(path), ::toupper);
	std::transform(std::begin(path), std::end(path), std::begin(path), [] (char c) {
		return c == '.' ? '_' : c;
	});

	/* Add identifier unique to the class */
	path += "_SHADER_PROC_INCL";
	
	return path;
}

Shader::Result<Shader::ErrorType, std::string> Shader::process_include_directive(std::string const& directive, std::string const& source, std::size_t idx){
	using namespace std::string_literals;

	/* File name */
	std::string incl_path  = directive.substr(idx+1, directive.length()-1);
	
	if(incl_path[0] != '"' || incl_path[incl_path.size() - 1] != '"')
		return { Outcome::Failure, std::make_tuple(ErrorType::Include, "Include directives must be enclosed in double qutes (\"\")"s) };

	 /* Remove quotations */ 
	incl_path = incl_path.substr(1, incl_path.size() - 2);
	

	/* Append file name to source's path if needed */
	if(source.find('/') != std::string::npos)
		incl_path = source.substr(0, source.find_last_of('/')+1) + incl_path;	

	return {Outcome::Success, std::make_tuple(ErrorType::None, std::move(incl_path)) };
}

Shader::Result<GLuint, std::string> Shader::compile(std::string const& source, Type type){
	GLenum shader_type = type == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	
	GLuint id = glCreateShader(shader_type);

	auto c_source = source.c_str();

	glShaderSource(id, 1, &c_source, nullptr);
	glCompileShader(id);
	
	Result<std::string> result = assert_shader_status_ok(id, StatusQuery::Compile);	

	return { result.outcome, std::make_tuple(id, std::get<0>(result.data)) };
}

Shader::Result<GLuint, std::string> Shader::link(GLuint vertex_id, GLuint fragment_id) {
	GLuint program_id = glCreateProgram();

	glAttachShader(program_id, vertex_id);
	glAttachShader(program_id, fragment_id);

	glLinkProgram(program_id);
	
	Result<std::string> result = assert_shader_status_ok(program_id, StatusQuery::Link);

	/* Clean up regardless of success */
	glDetachShader(program_id, vertex_id);
	glDetachShader(program_id, fragment_id);

	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);	

	return { result.outcome, std::make_tuple(program_id, std::get<0>(result.data)) };
}

Shader::Result<std::string> Shader::assert_shader_status_ok(GLuint id, StatusQuery sq){
	using namespace std::string_literals;

	GLint succeeded = 0;

	if(sq == StatusQuery::Compile)
		glGetShaderiv(id, GL_COMPILE_STATUS, &succeeded);
	else
		glGetProgramiv(id, GL_LINK_STATUS, &succeeded);

	if(succeeded == GL_FALSE){
		GLint max_length = 0;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

		std::vector<GLchar> error_log(max_length);

		if(sq == StatusQuery::Compile)
			glGetShaderInfoLog(id, max_length, &max_length, &error_log[0]);
		else
			glGetProgramInfoLog(id, max_length, &max_length,  &error_log[0]);

		return { Outcome::Failure, std::make_tuple(std::string{std::begin(error_log), std::end(error_log)}) };
	}

	return { Outcome::Success, ""s };
}

std::size_t const Shader::depth_ = 8u;
std::atomic_bool Shader::halt_execution_ = true;
std::mutex ics_mutex_{};
std::thread update_thread_{};
std::vector<std::reference_wrapper<Shader>> Shader::instances_{};
