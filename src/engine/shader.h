#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "bitmask.h"
#include "collections.h"
#include "ct_sequence.h"
#include "exception.h"
#include "logger.h"
#include "result.h"
#include "traits.h"
#include "uniform_impl.h"
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

class Shader {
	using callback_func = void(*)(Shader const&);

	struct OptionalEffects {
		using value_type = unsigned char;
		static value_type constexpr Bloom =   0x1; /* Bloom effect */
		static value_type constexpr Blur  =   0x2; /* Gaussian blur */
		static value_type constexpr Reflect = 0x4; /* Reflection */
		static value_type constexpr Refract = 0x8; /* Refraction */
	};
	public:
		using Fx = Bitmask<OptionalEffects>;

		enum class Type { Vertex   = GL_VERTEX_SHADER, 
						  Fragment = GL_FRAGMENT_SHADER,
						  Compute  = GL_COMPUTE_SHADER,
						  TessCtrl = GL_TESS_CONTROL_SHADER,
						  TessEval = GL_TESS_EVALUATION_SHADER,
						  Geometry = GL_GEOMETRY_SHADER };
		
		/* Takes any number of pairs of std::string (holding paths to source files) and Shader::Type.*/
		/* Optionally, an Fx instance may be added as the last argument */
		/* The following example constructs a shader program that uses multiple render targets from N source files */
		/* Shader{source1, Shader::Type::Vertex, source2, Shader::Type::Fragment, ..., sourceN, Shader::Type::Fragment, Shader::Fx::Bloom} */
		template <typename... Sources>
		Shader(Sources const&... src);

		~Shader();
		
		Shader(Shader const&) = delete;
		Shader(Shader&&) = default;
		Shader& operator=(Shader const&) = delete;
		Shader& operator=(Shader&&) = default;

		void enable() const;
		static void enable(GLuint program);
		static void disable();

		static void set_reload_callback(callback_func);

		static void reallocate_texture(int width, int height);

		GLuint program_id() const;
		
		/* (Indirect) Wrappers for glUniform */
		/* Pass any arithmetic fundamental type, glm::vec (except glm::bvec) or glm::mat and the correct glUniform function will be
 		 * deduced. As the deduction relies on the type passed, pointers are not supported.
 		 * 	- If passing arithmetic types, any number between 1 and 4 works and calls the correct glUniform.
 		 *  - glm types should be passed directly and not by calling glm::value_ptr, using &...[0] or anything smiliar */
		template <bool Store = false, typename... Args>
		void upload_uniform(std::string const& name, Args&&... args) const;						/* Stores uniform locations in hash map */

		template <bool Store = false, typename... Args>
		static void upload_to_all(std::string const& name, Args&&... args);

		bool operator==(Shader const& other) const noexcept;
		bool operator!=(Shader const& other) const noexcept;

		static std::string const PROJECTION_UNIFORM_NAME;
		static std::string const VIEW_UNIFORM_NAME;
		static std::string const MODEL_UNIFORM_NAME;
		static std::string const TIME_UNIFORM_NAME;
	private:
		enum class StatusQuery { Compile, Link };
		enum class ErrorType { None,
							   ArgumentMismatch,
							   FileIO, 
							   Compilation,
							   Include, 
							   Linking,
							   End_ };
		struct Viewport {
			int width;
			int height;
		};

		struct Source {
			Source() = default;
			Source(std::string const& file, Type t);
			Source(std::string&& file, Type t);
			Source(std::filesystem::path const& file, Type t);
			Source(std::filesystem::path&& file, Type t);

			Source(Source const& other);
			Source(Source&& other);
			Source& operator=(Source const& other) &;
			Source& operator=(Source&& other) &;

			Result<std::optional<std::string>> touch() noexcept;
			void update_write_time(std::filesystem::file_time_type time = std::chrono::system_clock::now()) noexcept;

			Result<std::variant<std::string, std::filesystem::file_time_type>>
				get_last_write_time() const noexcept;
			bool has_changed();

			std::filesystem::path path{};
			std::filesystem::file_time_type last_write{};
			Type type{};

			private:
				void reconstruct();
		};

		std::atomic<GLuint> program_{}; /* Shader program id */
		std::unordered_map<std::string, GLint> mutable cached_uniform_locations_{};
		std::map<std::string, glm::mat4> mutable stored_uniform_data_{};
		std::vector<Source> sources_;
		static std::size_t constexpr depth_{8u}; /* Max recursive include depth */

		static GLuint fbo_;
		static GLuint rbo_;
		static GLuint texture_buffer_;
		static Fx effects_;

		static std::atomic_bool halt_execution_;
		static std::mutex ics_mutex_;
		static std::thread updater_thread_;
		static std::vector<std::reference_wrapper<Shader>> instances_;
		static callback_func reload_callback_;

		template <std::size_t N>
		void init(Fx fx);
	
		static void setup_texture_environment(int width, int height);

		static void delete_buffers() noexcept;

		template <typename... Args, std::size_t... Is>
		static bool constexpr even_parameters_acceptable(even_index_sequence<Is...>);
		template <typename... Args, std::size_t... Is>
		static bool constexpr odd_parameters_acceptable(odd_index_sequence<Is...>);
		
		static Result<ErrorType, std::string> read_source(std::string const& source);
		static std::string format_header_guard(std::string path);
		static Result<ErrorType, std::string> process_include_directive(std::string const& directive, std::string const& source, std::size_t idx);
		static Result<GLuint, std::string> compile(std::string const& source, Type type);

		template <typename T, typename = std::enable_if_t<is_container_v<T>>>
		static Result<std::variant<GLuint, std::string>> link(T const& ids);

		static Result<std::optional<std::string>> assert_shader_status_ok(GLuint id, StatusQuery sq);

		template <std::size_t N, std::size_t Size, typename... Sources>
		void generate_source(std::string const& path, Type type, Sources const&... rest);
	
		static GLenum constexpr to_GLenum(Type type) noexcept;

		template <typename... Args>
		static void upload_uniform(GLint location, Args&&... args);

		static void monitor_source_files();
		bool reload();
		void update_internal_uniform_locations() const;

		static Viewport viewport_info() noexcept;
};

#include "shader.tcc"
#endif
