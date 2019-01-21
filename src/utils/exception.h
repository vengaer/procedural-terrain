#ifndef EXCEPTION_H
#define EXCEPTION_H

#pragma once
#include <stdexcept>
#include <string>

struct ArgumentMismatchException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct BadUniformParametersException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct FileSystemException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct FileIOException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct GLException : std::runtime_error { 
	using std::runtime_error::runtime_error;
};

struct InvalidArgumentException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct InvalidVersionException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct OverflowException : std::overflow_error {
	using std::overflow_error::overflow_error;
};

struct ShaderCompilationException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct ShaderIncludeException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct ShaderLinkingException : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct UnderflowException : std::underflow_error {
	using std::underflow_error::underflow_error;
};

#endif
