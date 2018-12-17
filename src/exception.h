#ifndef EXCEPTION_H
#define EXCEPTION_H

#pragma once
#include <stdexcept>
#include <string>

struct FileIOException : public std::runtime_error {
	FileIOException(std::string const& what_msg) : std::runtime_error{what_msg} {}
};

struct GLException : public std::runtime_error { 
	GLException(std::string const& what_msg) : std::runtime_error{what_msg} {} 
};


#endif
