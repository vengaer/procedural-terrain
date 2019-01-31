#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "collections.h"
#include "context.h"
#include "exception.h"
#include "logger.h"
#include "result.h"
#include "traits.h"
#include "uniform_impl.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <stack>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

class Shader {
	using callback_func = void(*)(Shader const&);
	public:
		enum class Type { Vertex, Fragment };
		
		Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2);
		~Shader();
		
		Shader(Shader const&) = delete;
		Shader(Shader&&) = default;
		Shader& operator=(Shader const&) = delete;
		Shader& operator=(Shader&&) = default;

		void enable() const;
		static void enable(GLuint program);
		static void disable();

		static void set_reload_callback(callback_func);

		GLuint program_id() const;
		
		/* (Indirect) Wrappers for glUniform */
		/* Pass any arithmetic fundamental type, glm::vec (except glm::bvec) or glm::mat and the correct glUniform function will be
 		 * deduced. As the deduction relies on the type passed, pointers are not supported.
 		 * 	- If passing arithmetic types, any number between 1 and 4 works and calls the correct glUniform.
 		 *  - glm types should be passed directly and not by calling glm::value_ptr, using &...[0] or anything smiliar */
		template <typename... Args>
		void upload_uniform(std::string const& name, Args&&... args) const;						/* Stores uniform locations in hash map */

		template <typename... Args>
		static void upload_uniform(GLuint program, std::string const& name, Args&&... args);    /* Always calls glGetUniformLocation */

	private:
		enum class StatusQuery { Compile, Link };
		enum class ErrorType { None,
							   ArgumentMismatch,
							   FileIO, 
							   Compilation,
							   Include, 
							   Linking,
							   End_ };


		struct SourceFile {
			SourceFile() = default;
			SourceFile(std::string const& vert, std::string const& frag);
			SourceFile(std::string&& vert, std::string&& frag);
			SourceFile(std::filesystem::path const& vert, std::filesystem::path const& frag);
			SourceFile(std::filesystem::path&& vert, std::filesystem::path&& frag);

			SourceFile(SourceFile const& other);
			SourceFile(SourceFile&& other);
			SourceFile& operator=(SourceFile const& other) &;
			SourceFile& operator=(SourceFile&& other) &;

			std::pair<std::filesystem::path, std::filesystem::path> get_stems() const;
			Result<std::optional<std::string>> touch() noexcept;
			void update_write_time(std::filesystem::file_time_type time = std::chrono::system_clock::now()) noexcept;

			Result<std::variant<std::string,
								std::pair<std::filesystem::file_time_type, 
										  std::filesystem::file_time_type
			>>> get_last_write_time() const noexcept;
			bool has_changed();

			std::filesystem::path vertex{}, fragment{};
			std::filesystem::file_time_type last_vert_write{}, last_frag_write{};

			private:
				void reconstruct();
		};

		GLuint program_; /* Shader program id */
		std::unordered_map<std::string, GLint> mutable uniforms_;
		SourceFile source_;
		static std::size_t const depth_; /* Max recursive include depth */

		static std::atomic_bool halt_execution_;
		static std::mutex ics_mutex_;
		static std::thread updater_thread_;
		static std::vector<std::reference_wrapper<Shader>> instances_;
		static callback_func reload_callback_;

		void init(std::string const& shader1, Type type1, std::string const& shader2, Type type2);
		
		static Result<ErrorType, std::string> read_source(std::string const& source);
		static std::string format_header_guard(std::string path);
		static Result<ErrorType, std::string> process_include_directive(std::string const& directive, std::string const& source, std::size_t idx);
		static Result<GLuint, std::string> compile(std::string const& source, Type type);
		static Result<std::variant<GLuint, std::string>> link(GLuint vertex_id, GLuint fragment_id);

		static Result<std::optional<std::string>> assert_shader_status_ok(GLuint id, StatusQuery sq);

		static SourceFile generate_source_info(std::string const& shader1, Type type1, std::string const& shader2, Type);

		template <typename... Args>
		static void upload_uniform(GLint location, Args&&... args);

		static void reload_on_change();
		void reload();
		void update_internal_uniform_locations() const;
};

#include "shader.tcc"
#endif
