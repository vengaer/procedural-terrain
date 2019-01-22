#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "collections.h"
#include "exception.h"
#include "logger.h"
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
#include <iostream>
#include <mutex>
#include <stack>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>



class Shader {
	public:
		enum class Type { Vertex, Fragment };

		Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2);

		void enable() const;
		static void enable(GLuint program);
		static void disable();

		GLuint program_id() const;
		
		/* (Indirect) Wrappers for glUniform */
		/* Pass any arithmetic fundamental type, glm::vec (except glm::bvec) or glm::mat and the correct glUniform function will be
 		 * deduced. As the deduction relies on the type passed, pointers are not supported.
 		 * 	- If passing arithmetic types, any number between 1 and 4 works and calls the correct glUniform.
 		 *  - glm types should be passed directly and not by calling glm::value_ptr, using &...[0] or anything smiliar */
		template <typename... Args>
		void upload_uniform(std::string const& name, Args&&... args);							/* Stores uniform locations in hash map */

		template <typename... Args>
		static void upload_uniform(GLuint program, std::string const& name, Args&&... args);    /* Always calls glGetUniformLocation */

	private:
		struct Source {
			std::filesystem::path vertex{}, fragment{};
			std::filesystem::file_time_type vert_write{}, frag_write{};
		};

		GLuint const program_; /* Shader program id */
		std::unordered_map<std::string, GLint> uniforms_;
		Source source_;
		static std::size_t const depth_; /* Max recursive include depth */

		static std::atomic_bool halt_execution_;
		static std::mutex ics_mutex_;
		static std::thread update_thread_;
		static std::vector<std::reference_wrapper<Shader>> instances_;

		enum class StatusQuery { Compile, Link };
		enum class Outcome { Failure, Success, End_ };
		enum class ErrorType { None,
							   ArgumentMismatch,
							   FileIO, 
							   Compilation,
							   Include, 
							   Linking,
							   End_ };

		template <typename... Args>	
		struct Result{
			Outcome outcome{};
			std::tuple<Args...> data{};
		};

	
		GLuint init(std::string const& shader1, Type type1, std::string const& shader2, Type type2);
		
		static Result<ErrorType, std::string> read_source(std::string const& source);
		static std::string format_header_guard(std::string path);
		static Result<ErrorType, std::string> process_include_directive(std::string const& directive, std::string const& source, std::size_t idx);
		static Result<GLuint, std::string> compile(std::string const& source, Type type);
		static Result<GLuint, std::string> link(GLuint vertex_id, GLuint fragment_id);

		static Result<std::string> assert_shader_status_ok(GLuint id, StatusQuery sq);

		static Source generate_source_info(std::string const& shader1, Type type1, std::string const& shader2, Type);
		static Result<std::string,
					  std::filesystem::file_time_type, 
					  std::filesystem::file_time_type
		> get_last_write_times(Source const& source) noexcept;

		template <typename... Args>
		static void upload_uniform(GLint location, Args&&... args);

};

#include "shader.tcc"
#endif
