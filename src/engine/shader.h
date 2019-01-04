#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "exception.h"
#include "fold.h"
#include "traits.h"
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
#include <unordered_map>
#include <vector>



class Shader {
	public:
		enum class Type { Vertex, Fragment };

		struct Uniform {
			enum class Construct { Vector, Matrix, End_};
			enum class Dimension { _1 = 1, _2, _3, _4, _2x3, _3x2, _2x4, _4x2, _3x4, _4x3, End_ };
			enum class Type { Float, Int, Uint, FloatPtr, IntPtr, UintPtr, End_};
		};

		Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2, std::size_t include_depth = 8);

		void enable() const;
		static void disable();

		GLuint program_id() const;
		
		template <Shader::Uniform::Construct UC,
				  Shader::Uniform::Dimension UD,
				  Shader::Uniform::Type UT,
				  typename... Args>
		void upload_uniform(std::string const& name, Args&&... args);

		template <Shader::Uniform::Construct UC,
				  Shader::Uniform::Dimension UD,
				  Shader::Uniform::Type UT,
				  typename... Args>
		static void upload_uniform(GLuint program, std::string const& name, Args&&... args);

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

		template <Shader::Uniform::Construct UC,
				  Shader::Uniform::Dimension UD,
				  Shader::Uniform::Type UT,
				  typename... Args>
		static void upload_uniform_impl(GLint location, Args&&... args);

};

#include "shader.tcc"
#endif
