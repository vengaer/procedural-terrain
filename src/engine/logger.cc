#include "logger.h"

std::string logging::to_string(Label S) {
	if(S == Label::Debug)
		return "Debug";
	else if(S == Label::Warning)
		return "Warning";
	
	return "Critical";
}

std::string logging::LoggerBase::extract_label(std::string const& message) {
	std::regex label_regex{"<([A-Za-z]+)>"};
	std::smatch match;

	if(std::regex_search(message, match, label_regex))
		return match[1];

	return "UNKNOWN";
}

std::string logging::LoggerBase::extract_content(std::string const& message) {
	std::regex content_regex{": *([^:]+)$"};
	std::smatch match;

	if(std::regex_search(message, match, content_regex))
		return match[1];

	return "UNKNOWN";
}
