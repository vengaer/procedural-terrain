#ifndef EXCEPTION_H
#define EXCEPTION_H

#pragma once
#include <stdexcept>
#include <string>

struct ArgumentMismatchException : std::runtime_error {
	ArgumentMismatchException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct FileIOException : std::runtime_error {
	FileIOException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct GLException : std::runtime_error { 
	GLException(std::string const& what_msg) : std::runtime_error{what_msg} {} 
};

struct InvalidArgumentException : std::runtime_error {
	InvalidArgumentException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct ShaderCompilationException : std::runtime_error {
	ShaderCompilationException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct ShaderIncludeException : std::runtime_error {
	ShaderIncludeException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct ShaderLinkingException : std::runtime_error {
	ShaderLinkingException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};


#endif
