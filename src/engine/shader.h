#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "exception.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stack>
#include <sstream>
#include <string>
#include <vector>
#include <GL/glew.h>



class Shader {
	public:
		enum class Type { VERTEX, FRAGMENT };

		Shader(std::string const& shader1, Type type1, std::string const& shader2, Type type2, unsigned include_depth = 8);

		void enable() const;
		static void disable();

	private:
		unsigned const depth_; /* Max recursive include depth */
		GLuint program_; /* Shader program id */

		enum class StatusQuery { COMPILE, LINK };
		
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

};

#endif
