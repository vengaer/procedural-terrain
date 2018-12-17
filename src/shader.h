#ifndef SHADER_H
#define SHADER_H

#pragma once
#include "exception.h"
#include <fstream>
#include <iostream>
#include <string>
#include <GL/glew.h>

enum class ShaderType { NA = -1, VERTEX, FRAGMENT };

class Shader {
	public:
		Shader(const std::string& shader1, ShaderType type1, const std::string& shader2, ShaderType type2);

	private:
		GLuint program_;
		
		std::string read_source(std::string const& source) const;

};

#endif
