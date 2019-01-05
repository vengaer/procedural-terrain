#include "shader.h"

Shader::Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2, std::size_t max_depth) : depth_{max_depth}, program_{init(shader1, type1, shader2, type2)}, uniforms_{} {}

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

GLuint Shader::init(std::string const& shader1, Type type1, std::string const& shader2, Type type2){
	if(type1 == type2)
		throw ArgumentMismatchException{"Provided shader types must differ\n"};
	
	/* shader_ids[0] holds vertex id, shader_ids[1] fragment id */
	std::array<GLuint, 2> shader_ids;
	
	/* Clean up shader(s) if compilation fails */
	try{
		shader_ids[enum_value(type1)] = compile(read_source(shader1), type1);
	}
	catch(ShaderCompilationException const& err){
		glDeleteShader(shader_ids[enum_value(type1)]);
		throw;
	}
	try{
		shader_ids[enum_value(type2)] = compile(read_source(shader2), type2);
	}
	catch(ShaderCompilationException const& err){
		for(auto const& shader : shader_ids)
			glDeleteShader(shader);
		throw;
	}
	
	return link(shader_ids[enum_value(Type::Vertex)], shader_ids[enum_value(Type::Fragment)]);
}


std::string Shader::read_source(std::string const& source) const{
	/* Every time an include directive is found, push stream with file to be included onto
 	 * the stack */
	std::stack<std::ifstream, std::vector<std::ifstream>> streams;

	streams.push(std::ifstream{source});
	
	if(!streams.top().is_open())
		throw FileIOException{"Unable to open " + source + " for reading\n"};

	std::string line;	
	std::ostringstream contents;
	
	while(!streams.empty()){

		while(std::getline(streams.top(), line)){
			if(auto idx = line.find(" "); line.substr(0, idx) == "#include"){

				/* Max include depth reached, to prevent infinite recursion */
				if(streams.size() >= depth_){
					std::cerr << "Warning, max include depth reached, omitting " 
							  << line << " included in file " << source;
					continue;
				}
				
				/* Process include directive */
				std::string incl_path = process_include_directive(line, source, idx);

				std::ifstream file{incl_path};					
				
				if(!file.is_open())
					throw FileIOException{"Unable to open file " + incl_path + " included from " + source};

				/* c/c++ style header guard to prevent redefinition */		
				std::string header_guard = format_header_guard(incl_path);

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

	return contents.str();	
}

std::string Shader::format_header_guard(std::string path) const {
	
	/* Get file name */
	if(path.find('/') != std::string::npos)
		path = path.substr(path.find_last_of('/')+1, path.size() -1);
	
	std::transform(std::begin(path), std::end(path), std::begin(path), ::toupper);
	std::transform(std::begin(path), std::end(path), std::begin(path), [] (char c) {
		return c == '.' ? '_' : c;
	});

	/* Add identifier unique to the class */
	path += "_SHADER_PROC_INCL";
	
	return std::move(path);
}

std::string Shader::process_include_directive(std::string const& directive, std::string const& source, std::size_t idx) const{

	/* File name */
	std::string incl_path  = directive.substr(idx+1, directive.length()-1);
	
	if(incl_path[0] != '"' || incl_path[incl_path.size() - 1] != '"')
		throw ShaderIncludeException{"Include directives must be enclosed in double quotes (\"\")"};

	 /* Remove quotations */ 
	incl_path = incl_path.substr(1, incl_path.size() - 2);
	

	/* Append file name to source's path if needed */
	if(source.find('/') != std::string::npos)
		incl_path = source.substr(0, source.find_last_of('/')+1) + incl_path;	

	return incl_path;
}

GLuint Shader::compile(std::string const& source, Type type) const{
	GLenum shader_type = type == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	
	GLuint id = glCreateShader(shader_type);

	auto c_source = source.c_str();

	glShaderSource(id, 1, &c_source, nullptr);
	glCompileShader(id);
	
	AssertionResult result = assert_shader_status_ok(id, StatusQuery::Compile);	

	if(!result.succeeded)
		throw ShaderCompilationException{result.msg};
	
	return id;
}

GLuint Shader::link(GLuint vertex_id, GLuint fragment_id) const {
	GLuint program_id = glCreateProgram();

	glAttachShader(program_id, vertex_id);
	glAttachShader(program_id, fragment_id);

	glLinkProgram(program_id);
	
	AssertionResult result = assert_shader_status_ok(program_id, StatusQuery::Link);

	/* Clean up regardless of success */
	glDetachShader(program_id, vertex_id);
	glDetachShader(program_id, fragment_id);

	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);	

	if(!result.succeeded)
		throw ShaderLinkingException{result.msg};

	return program_id;
}

Shader::AssertionResult Shader::assert_shader_status_ok(GLuint id, StatusQuery sq) const{
	AssertionResult res{ true, std::string{} };

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

		res.succeeded = false;
		res.msg = std::string{std::begin(error_log), std::end(error_log)};
	}

	return res;
}
