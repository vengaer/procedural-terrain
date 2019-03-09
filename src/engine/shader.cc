#include "context.h"
#include "shader.h"
#include <algorithm>
#include <fstream>
#include <memory>
#include <stack>
#include <sstream>

Shader::~Shader() {
	LOG("Deleting shader");

	std::lock_guard<std::mutex> lock{ics_mutex_};
	instances_.erase(
		std::remove_if(std::begin(instances_), 
					   std::end(instances_), 
					   [this](auto ref_wrapper) {
			return std::addressof(ref_wrapper.get()) == this;
		}));

	if(instances_.size() == 0u) {
		delete_buffers();
		#ifndef RESTRICT_THREAD_USAGE
		LOG("Joining updater thread with id ", updater_thread_.get_id());
		halt_execution_ = true;
		updater_thread_.join();
		#endif
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

bool Shader::operator==(Shader const& other) const noexcept {
	return program_ == other.program_;
}

bool Shader::operator!=(Shader const& other) const noexcept {
	return !(*this == other);
}

void Shader::set_reload_callback(callback_func func) {
	reload_callback_ = func;
}

void Shader::bind_main_framebuffer() noexcept {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void Shader::bind_default_framebuffer() noexcept {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shader::bind_scene_texture() noexcept {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_);
}

void Shader::unbind_scene_texture() noexcept {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Shader::program_id() const {
	return program_;
}

void Shader::reallocate_textures(int width, int height) {
	unbind_scene_texture();
	delete_buffers();
	setup_texture_environment(width, height);
	bind_main_framebuffer();
}

void Shader::setup_texture_environment(int width, int height) {
	glGenFramebuffers(1, &fbo_);
	bind_main_framebuffer();

	glGenTextures(1, &texture_buffer_);

    glBindTexture(GL_TEXTURE_2D, texture_buffer_);
    
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                 GL_RGB, 
                 width, 
                 height, 
                 0, 
                 GL_RGB, 
                 GL_UNSIGNED_BYTE, 
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, 
                           texture_buffer_,
                           0);

	glGenRenderbuffers(1, &rbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
							  GL_DEPTH_STENCIL_ATTACHMENT, 
							  GL_RENDERBUFFER,
							  rbo_);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};

	bind_default_framebuffer();
}

void Shader::delete_buffers() noexcept {
	LOG("Cleaning up static resources");
	glDeleteFramebuffers(1, &fbo_);
	glDeleteTextures(1, &texture_buffer_);
	glDeleteRenderbuffers(1, &rbo_);
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
	GLuint id = glCreateShader(to_GLenum(type));

	auto c_source = source.c_str();

	glShaderSource(id, 1, &c_source, nullptr);
	glCompileShader(id);
	
	auto result = assert_shader_status_ok(id, StatusQuery::Compile);	

	return { result.outcome, std::make_tuple(id, result.data.value_or(std::string{})) };
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

		error_log.erase(std::remove(std::begin(error_log), std::end(error_log), '\n'), std::end(error_log));

		return { Outcome::Failure, std::optional<std::string>{std::string{std::begin(error_log), std::end(error_log)}} };
	}

	return { Outcome::Success, std::nullopt };
}

GLenum constexpr Shader::to_GLenum(Type type) noexcept {
	return static_cast<GLenum>(enum_value(type));
}

void Shader::monitor_source_files() {
	using namespace std::chrono_literals;
	Context const shared_context{"", 1u, 1u, false, true};

	while(!halt_execution_) {
		{
			std::lock_guard<std::mutex> lock{ics_mutex_};
			for(auto& shader : instances_){
				for(auto& source : shader.get().sources_) {
					if(source.has_changed()) {
						if(shader.get().reload()) {
							if(reload_callback_)
								(*reload_callback_)(shader.get());
						}
						else
							source.update_write_time(); /* Dont' attempt to reload unless a new change is made */
						break;
					}
				}
			}
		}
		std::this_thread::sleep_for(2s);
	}
}

bool Shader::reload() {
	LOG("Change to shader source detected, reloading...");
	
	std::vector<GLuint> shader_ids(sources_.size());
	Result<GLuint, std::string> result;

	for(auto [idx, id] : enumerate(shader_ids)) {
		auto path = sources_[idx].path.string();

		LOG("Reading source ", sources_[idx].path.stem().string());
		auto [outcome, data] = read_source(path);
		auto& [error_type, content] = data;

		if(outcome == Outcome::Failure) {
			for(auto i = 0u; i < idx; i++)
				glDeleteShader(shader_ids[i]);

			if(error_type == ErrorType::FileIO){
				ERR_LOG_WARN("Error reading source file when updating. Process returned message \'", content, "\'");
			}
			else if(error_type == ErrorType::Include) {
				ERR_LOG_WARN("Error including source file when updating. Process returned message \'", content, "\'");
			}
			return false;
		}
		LOG("Compiling ", sources_[idx].path.stem().string());
		result = compile(content, sources_[idx].type);
		id = std::get<0>(result.data);
		
		if(result.outcome == Outcome::Failure) {
			for(auto i = 0u; i < idx + 1; i++)
				glDeleteShader(shader_ids[i]);
						
			ERR_LOG_WARN("Error compiling shader, process returned message \'", std::get<1>(result.data), "\'");
			return false;
		}
	}
	LOG("Relinking shader with id ", program_);
	auto [outcome, data] = link(shader_ids);

	if(outcome == Outcome::Failure) {
		ERR_LOG_CRIT("Error linking shader. Process returned message \'", std::get<std::string>(data), "\'. Attempting to schedule relinking for next cycle...");
		for(auto& source : sources_) {
			auto [touch_outcome, opt] = source.touch();
			if(touch_outcome == Outcome::Failure) {
				ERR_LOG_CRIT("Failed to schedule relinking. \'", opt.value(), "\' was returned. The operation will be  aborted");
				return false;
			}
		}
		ERR_LOG("Scheduling successful, another attempt will be made shortly");
		return false;
	}
	glDeleteProgram(program_);
	program_ = std::get<GLuint>(data);

	LOG("Shader successfully relinked and assigned new id", program_);
	for(auto& source : sources_)
		source.update_write_time();

	LOG("Updating internal uniform locations");
	update_internal_uniform_locations();

	LOG("Reuploading uniforms to gpu");
	enable();
	for(auto const& [handle, data] : stored_uniform_data_)
		upload_uniform(handle, data);

	return true;
}

void Shader::update_internal_uniform_locations() const {
	for(auto& [handle, location] : cached_uniform_locations_)
		location = glGetUniformLocation(program_, handle.c_str());
}

std::string const Shader::PROJECTION_UNIFORM_NAME = "ufrm_projection";
std::string const Shader::VIEW_UNIFORM_NAME = "ufrm_view";
std::string const Shader::MODEL_UNIFORM_NAME = "ufrm_model";
std::string const Shader::TIME_UNIFORM_NAME = "ufrm_time";

GLuint Shader::fbo_{};
GLuint Shader::rbo_{};
GLuint Shader::texture_buffer_{};

std::atomic_bool Shader::halt_execution_ = true;
std::mutex Shader::ics_mutex_{};
std::thread Shader::updater_thread_{};
std::vector<std::reference_wrapper<Shader>> Shader::instances_{};
typename Shader::callback_func Shader::reload_callback_{nullptr};

/* Shader::Source */
Shader::Source::Source(std::string const& file, Type t) : path{file}, last_write{std::chrono::system_clock::now()}, type{t} { }

Shader::Source::Source(std::filesystem::path const& file, Type t) : path{file}, last_write{std::chrono::system_clock::now()}, type{t} { }

Shader::Source::Source(std::string&& file, Type t) : path{std::move(file)}, last_write{std::chrono::system_clock::now()}, type{t} { }

Shader::Source::Source(std::filesystem::path&& file, Type t) : path{std::move(file)}, last_write{std::chrono::system_clock::now()}, type{t} { }

Shader::Source::Source(Source const& other) : last_write{other.last_write}, type{other.type} {
	reconstruct();
	path = other.path;
}

Shader::Source::Source(Source&& other) : last_write{std::move(other.last_write)}, type{other.type} {
	reconstruct();
	path = std::move(other.path);
}

Shader::Source& Shader::Source::operator=(Source const& other) & {
	reconstruct();
	path = other.path;
	last_write = other.last_write;
	type = other.type;
	return *this;
}

Shader::Source& Shader::Source::operator=(Source&& other) & {
	reconstruct();
	path = std::move(other.path);
	last_write = std::move(other.last_write);
	type = other.type;
	return *this;
}


Result<std::optional<std::string>> Shader::Source::touch() noexcept {
	namespace fs = std::filesystem;

	auto now = std::chrono::system_clock::now();
	std::error_code err;
	
	fs::last_write_time(path, now, err);
	if(err)
		return { Outcome::Failure, std::optional<std::string>{err.message()} };

	return { Outcome::Success, std::nullopt };
}

void Shader::Source::update_write_time(std::filesystem::file_time_type time) noexcept {
	last_write = time;

}
Result<std::variant<std::string, std::filesystem::file_time_type>>
	Shader::Source::get_last_write_time() const noexcept {
	namespace fs = std::filesystem;
	std::error_code err{};

	auto time = fs::last_write_time(path, err);
	if(err)
		return { Outcome::Failure, err.message() };

	return { Outcome::Success, time };
}

bool Shader::Source::has_changed() {
	namespace fs = std::filesystem;
	auto [outcome, data] = get_last_write_time();
	
	if(outcome == Outcome::Failure) {
		ERR_LOG_WARN("Failed to update write time for shader created from source", path.stem(), ". Process returned error code with message: \'", std::get<std::string>(data), "\'. No update is possible");

		return false;
	}
	auto time = std::get<fs::file_time_type>(data);
	
	return time > last_write;
}

void Shader::Source::reconstruct() {
	namespace fs = std::filesystem;
	new (&path) fs::path;
}
