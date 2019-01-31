#include "shader.h"

Shader::Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2) : program_{}, uniforms_{}, source_{} {
	init(shader1, type1, shader2, type2);
}

Shader::~Shader() {
	LOG("Deleting shader");
	std::lock_guard<std::mutex> lock{ics_mutex_};

	instances_.erase(std::remove_if(std::begin(instances_), std::end(instances_), [this](auto ref_wrapper) {
		return std::addressof(ref_wrapper.get()) == this;
	}));

	if(instances_.size() == 0) {
		LOG("Joining updater thread with id ", updater_thread_.get_id());
		halt_execution_ = true;
		updater_thread_.join();
	}
}

void Shader::enable() const {
	enable(program_);
}

void Shader::enable(GLuint program) {
	glUseProgram(program);
}

void Shader::disable() {
	glUseProgram(0);
}

void Shader::set_reload_callback(callback_func func) {
	reload_callback_ = func;
}

GLuint Shader::program_id() const {
	return program_;
}

void Shader::init(std::string const& shader1, Type type1, std::string const& shader2, Type type2){
	if(type1 == type2)
		throw ArgumentMismatchException{"Provided shader types must differ\n"};

	source_ = generate_source_info(shader1, type1, shader2, type2);
	
	std::array<GLuint, 2> shader_ids;
	{
		Result<GLuint, std::string> result;
		bind_enum<Outcome, ErrorType> bind;
		{
			LOG("Reading source ", shader1);
			auto [outcome, data] = read_source(shader1);
			auto& [error_type, content] = data;

			if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO))
				throw FileIOException{content};
			else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include))
				throw ShaderIncludeException{content};

			LOG("Compiling ", shader1);
			result = compile(std::move(content), type1);
			if(result.outcome == Outcome::Failure) {
				glDeleteShader(std::get<0>(result.data));
				throw ShaderCompilationException{std::get<1>(result.data)};
			}
			shader_ids[enum_value(type1)] = std::get<0>(result.data);
		}
		LOG("Reading source ", shader2);
		auto [outcome, data] = read_source(shader2);
		auto& [error_type, content] = data;

		if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO)) {
			glDeleteShader(std::get<0>(result.data));
			glDeleteShader(shader_ids[enum_value(type1)]);
			throw FileIOException{content};
		}
		else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include)) {
			glDeleteShader(std::get<0>(result.data));
			glDeleteShader(shader_ids[enum_value(type1)]);
			throw ShaderIncludeException{content};
		}

		LOG("Compiling ", shader2);
		result = compile(std::move(content), type2);
		if(result.outcome == Outcome::Failure) {
			glDeleteShader(shader_ids[enum_value(type1)]);
			glDeleteShader(std::get<0>(result.data));
			throw ShaderCompilationException{std::get<1>(result.data)};
		}
		shader_ids[enum_value(type2)] = std::get<0>(result.data);
	}
	LOG("Linking shader ", program_);
	auto [outcome, data] = link(shader_ids[enum_value(Type::Vertex)], shader_ids[enum_value(Type::Fragment)]);

	if(outcome == Outcome::Failure)
		throw ShaderLinkingException{std::get<std::string>(data)};

	std::lock_guard<std::mutex> lock{ics_mutex_};
	program_ = std::get<GLuint>(data);
	LOG("Linking successful, assigning new id ", program_);

	LOG("Marking instance for automatic updating");
	instances_.push_back(std::ref(*this));

	if(halt_execution_){
		LOG("Creating separate thread for execution");
		halt_execution_ = false;
		updater_thread_ = std::thread{reload_on_change};
		LOG("Thead with id ", updater_thread_.get_id(), " successfully created");
	}
}


Result<Shader::ErrorType, std::string> Shader::read_source(std::string const& source){
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
					ERR_LOG_WARN("Max include depth reached, omitting ",  line, " included in file ", source);
					continue;
				}
				
				/* Process include directive */
				result = process_include_directive(line, source, idx);

				if(result.outcome == Outcome::Failure)
					return result;

				std::ifstream file{std::get<1>(result.data)};
				
				if(!file.is_open())
					return { Outcome::Failure, std::make_tuple(ErrorType::FileIO, "Unable to open file " + std::get<1>(result.data) + " included (possibly recursively) from file " + source) };

				/* c/c++ style header guard to prevent redefinition */		
				std::string header_guard = format_header_guard(std::get<1>(result.data));

				/* Append header guard */
				contents << "#ifndef " << header_guard << "\n#define " << header_guard << "\n";

				LOG("Including file", std::get<1>(result.data));
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

Result<Shader::ErrorType, std::string> Shader::process_include_directive(std::string const& directive, std::string const& source, std::size_t idx){
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

	return { Outcome::Success, std::make_tuple(ErrorType::None, std::move(incl_path)) };
}

Result<GLuint, std::string> Shader::compile(std::string const& source, Type type){
	GLenum shader_type = type == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	
	GLuint id = glCreateShader(shader_type);

	auto c_source = source.c_str();

	glShaderSource(id, 1, &c_source, nullptr);
	glCompileShader(id);
	
	auto result = assert_shader_status_ok(id, StatusQuery::Compile);	

	return { result.outcome, std::make_tuple(id, result.data.value_or(std::string{})) };
}

Result<std::variant<GLuint, std::string>> Shader::link(GLuint vertex_id, GLuint fragment_id) {
	GLuint program_id = glCreateProgram();

	glAttachShader(program_id, vertex_id);
	glAttachShader(program_id, fragment_id);

	glLinkProgram(program_id);
	
	auto result = assert_shader_status_ok(program_id, StatusQuery::Link);

	/* Clean up regardless of success */
	glDetachShader(program_id, vertex_id);
	glDetachShader(program_id, fragment_id);

	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);	
	if(result.outcome == Outcome::Failure)
		return { result.outcome, result.data.value() };

	return { result.outcome, program_id };
}

Result<std::optional<std::string>> Shader::assert_shader_status_ok(GLuint id, StatusQuery sq){
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
		for(auto c : error_log)
			std::cout << c;

		return { Outcome::Failure, std::optional<std::string>{std::string{std::begin(error_log), std::end(error_log)}} };
	}

	return { Outcome::Success, std::nullopt };
}

Shader::SourceFile Shader::generate_source_info(std::string const& shader1, Type type1, std::string const& shader2, Type) {
	namespace fs = std::filesystem;
	SourceFile source;
	source.vertex = type1 == Type::Vertex ?
								shader1 :
								shader2 ;
	source.fragment = type1 == Type::Fragment ?
								shader1 :
								shader2 ;
	auto [outcome, data] = source.get_last_write_time();

	if(outcome == Outcome::Failure)
		throw FileSystemException{std::get<std::string>(data)};

	auto& [vert_time, frag_time] = std::get<std::pair<fs::file_time_type, fs::file_time_type>>(data);
	
	source.last_vert_write = vert_time;
	source.last_frag_write = frag_time;
	return source;
}


void Shader::reload_on_change() {
	using namespace std::chrono_literals;
	Context const shared_context{"", 1, 1, false, true};

	while(!halt_execution_) {
		{
			std::lock_guard<std::mutex> lock{ics_mutex_};
			for(auto& shader : instances_){
				if(shader.get().source_.has_changed()) {
					shader.get().reload();
					if(reload_callback_)
						(*reload_callback_)(shader.get());
				}
			}
		}
		std::this_thread::sleep_for(2s);
	}
}

void Shader::reload() {
	LOG("Change to shader source detected, reloading...");

	std::array<GLuint, 2> shader_ids;
	{
		Result<GLuint, std::string> result;
		bind_enum<Outcome, ErrorType> bind;
		{
			LOG("Reading source ", source_.vertex.string());
			auto [outcome, data] = read_source(source_.vertex.string());
			auto& [error_type, content] = data;

			if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO)) {
				ERR_LOG_WARN("Error reading source file when updating. Process returned message \'", content, "\'");
				return;
			}
			else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include)) {
				ERR_LOG_WARN("Error inluding source file when updating. Process returned message \'", content, "\'");
				return;
			}


			LOG("Compiling ", source_.vertex.string());
			result = compile(std::move(content), Type::Vertex);
			if(result.outcome == Outcome::Failure) {
				glDeleteShader(std::get<0>(result.data));
				ERR_LOG_WARN("Error compiling shader, process returned message \'", std::get<1>(result.data), "\'");
				return;
			}
			shader_ids[enum_value(Type::Vertex)] = std::get<0>(result.data);
		}
		LOG("Reading source ", source_.fragment.string());
		auto [outcome, data] = read_source(source_.fragment.string());
		auto& [error_type, content] = data;

		if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::FileIO)) {
			glDeleteShader(std::get<0>(result.data));
			glDeleteShader(shader_ids[enum_value(Type::Vertex)]);
			ERR_LOG_WARN("Error reading source file when updating. Process returned message \'", content, "\'");
			return;
		}
		else if(bind(outcome, error_type) == bind(Outcome::Failure, ErrorType::Include)) {
			glDeleteShader(std::get<0>(result.data));
			glDeleteShader(shader_ids[enum_value(Type::Vertex)]);
			ERR_LOG_WARN("Error inluding source file when updating. Process returned message \'", content, "\'");
			return;
		}

		LOG("Compiling ", source_.fragment.string());
		result = compile(std::move(content), Type::Fragment);
		if(result.outcome == Outcome::Failure) {
			glDeleteShader(shader_ids[enum_value(Type::Vertex)]);
			glDeleteShader(std::get<0>(result.data));
			ERR_LOG_WARN("Error compiling shader, process returned message \'", std::get<1>(result.data), "\'");
			return;
		}
		shader_ids[enum_value(Type::Fragment)] = std::get<0>(result.data);
	}
	LOG("Replacing shader with id ", program_);
	auto [outcome, data] = link(shader_ids[enum_value(Type::Vertex)], shader_ids[enum_value(Type::Fragment)]);

	if(outcome == Outcome::Failure) {
		ERR_LOG_CRIT("Error linking shader. Process returned message \'", std::get<std::string>(data), "\'. Attempting to schedule relinking for next execution loop...");
		auto [touch_outcome, opt] = source_.touch();
		if(touch_outcome == Outcome::Failure) {
			throw ShaderLinkingException{opt.value()};
		}

		ERR_LOG("Scheduling successful, another linking attempt will be made shortly");
		return;
	}

	glDeleteProgram(program_);
	program_ = std::get<GLuint>(data);

	LOG("Shader successfully linked and assigned new id ", program_);
	source_.update_write_time();

	update_internal_uniform_locations();
	
}

void Shader::update_internal_uniform_locations() const {
	for(auto& [handle, location] : uniforms_)
		location = glGetUniformLocation(program_, handle.c_str());
}


std::size_t const Shader::depth_ = 8u;
std::atomic_bool Shader::halt_execution_ = true;
std::mutex Shader::ics_mutex_{};
std::thread Shader::updater_thread_{};
std::vector<std::reference_wrapper<Shader>> Shader::instances_{};
typename Shader::callback_func Shader::reload_callback_{nullptr};

/* Shader::SourceFile */
Shader::SourceFile::SourceFile(std::string const& vert, std::string const& frag) : vertex{vert}, fragment{frag}, last_vert_write{}, last_frag_write{} { }

Shader::SourceFile::SourceFile(std::filesystem::path const& vert, std::filesystem::path const& frag) : vertex{vert}, fragment{frag}, last_vert_write{}, last_frag_write{} { }

Shader::SourceFile::SourceFile(std::string&& vert, std::string&& frag) : vertex{std::move(vert)}, fragment{std::move(frag)}, last_vert_write{}, last_frag_write{} { }

Shader::SourceFile::SourceFile(std::filesystem::path&& vert, std::filesystem::path&& frag) : vertex{std::move(vert)}, fragment{std::move(frag)}, last_vert_write{}, last_frag_write{} { }

Shader::SourceFile::SourceFile(SourceFile const& other) : last_vert_write{other.last_vert_write}, last_frag_write{other.last_frag_write} {
	reconstruct();
	vertex = other.vertex;
	fragment = other.fragment;
}

Shader::SourceFile::SourceFile(SourceFile&& other) : last_vert_write{std::move(other.last_vert_write)}, last_frag_write{std::move(other.last_frag_write)} {
	reconstruct();
	vertex = std::move(other.vertex);
	fragment = std::move(other.fragment);
}

Shader::SourceFile& Shader::SourceFile::operator=(SourceFile const& other) & {
	reconstruct();
	vertex = other.vertex;
	fragment = other.fragment;
	last_vert_write = other.last_vert_write;
	last_frag_write = other.last_frag_write;
	return *this;
}

Shader::SourceFile& Shader::SourceFile::operator=(SourceFile&& other) & {
	reconstruct();
	vertex = std::move(other.vertex);
	fragment = std::move(other.fragment);
	last_vert_write = std::move(other.last_vert_write);
	last_frag_write = std::move(other.last_frag_write);
	return *this;
}

std::pair<std::filesystem::path, std::filesystem::path> Shader::SourceFile::get_stems() const {
	return { vertex.stem(), fragment.stem() };
}

Result<std::optional<std::string>> Shader::SourceFile::touch() noexcept {
	namespace fs = std::filesystem;

	auto now = std::chrono::system_clock::now();
	std::error_code err;
	
	fs::last_write_time(vertex, now, err);
	if(err)
		return { Outcome::Failure, std::optional<std::string>{err.message()} };

	fs::last_write_time(fragment, now, err);
	if(err)
		return { Outcome::Failure, std::optional<std::string>{err.message()} };

	return { Outcome::Success, std::nullopt };
}

void Shader::SourceFile::update_write_time(std::filesystem::file_time_type time) noexcept {
	last_vert_write = last_frag_write = time;

}
Result<std::variant<std::string,
						    std::pair<std::filesystem::file_time_type, 
									  std::filesystem::file_time_type
>>> Shader::SourceFile::get_last_write_time() const noexcept {
	namespace fs = std::filesystem;
	std::error_code err{};

	auto vert_time = fs::last_write_time(vertex, err);
	if(err)
		return { Outcome::Failure, err.message() };

	auto frag_time = fs::last_write_time(fragment, err);
	if(err)
		return { Outcome::Failure, err.message() };

	return { Outcome::Success, std::make_pair(vert_time, frag_time) };
}

bool Shader::SourceFile::has_changed() {
	namespace fs = std::filesystem;
	auto [outcome, data] = get_last_write_time();
	
	if(outcome == Outcome::Failure) {
		auto [v_stem, f_stem] = get_stems();
		ERR_LOG_WARN("Failed to update write time for shader created from vertex source ", v_stem, 
				 	 " and fragment source ", f_stem, ". Process returned error code with message: \'", 
				 	 std::get<std::string>(data), "\'. No update is possible");

		return false;
	}
	auto& [vert_time, frag_time] = std::get<std::pair<fs::file_time_type, fs::file_time_type>>(data);
	
	return vert_time > last_vert_write || frag_time > last_frag_write;
}

void Shader::SourceFile::reconstruct() {
	namespace fs = std::filesystem;
	new (&vertex) fs::path;
	new (&fragment) fs::path;
}
