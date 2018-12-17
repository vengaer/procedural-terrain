#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "exception.h"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <GL/glew.h>

enum class ShaderType { NA = -1, VERTEX, FRAGMENT };


class Shader {
	public:
		Shader(std::string const& shader1, ShaderType type1, std::string const& shader2, ShaderType type2, unsigned include_depth = 8);

		struct IncludeResult {
			bool well_formed;
			std::string content;
		};
	private:
		GLuint program_;
		unsigned max_depth_; /* Max include depth */
		
		std::string read_source(std::string const& source) const;
		std::string format_header_guard(std::string const& path) const;
		IncludeResult process_include(std::string const& directive, std::string const& source, std::size_t idx) const;

};

#endif
