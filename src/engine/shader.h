#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "collections.h"
#include "exception.h"
#include "traits.h"
#include "uniform_impl.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stack>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>



class Shader {
	public:
		enum class Type { Vertex, Fragment };

		Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2, std::size_t include_depth = 8);

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
		std::size_t const depth_; /* Max recursive include depth */
		GLuint const program_; /* Shader program id */
		std::unordered_map<std::string, GLint> uniforms_;

		enum class StatusQuery { Compile, Link };
		
		struct AssertionResult{
			bool succeeded;
			std::string msg;
		};
	
		GLuint init(std::string const& shader1, Type type1, std::string const& shader2, Type type2);
		
		std::string read_source(std::string const& source) const;
		std::string format_header_guard(std::string path) const;
		std::string process_include_directive(std::string const& directive, std::string const& source, std::size_t idx) const;
		GLuint compile(std::string const& source, Type type) const;
		GLuint link(GLuint vertex_id, GLuint fragment_id) const;

		AssertionResult assert_shader_status_ok(GLuint id, StatusQuery sq) const;

		template <typename... Args>
		static void upload_uniform(GLint location, Args&&... args);

};

#include "shader.tcc"
#endif
