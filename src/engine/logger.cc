#include "logger.h"

std::string logging::to_string(Label S) {
	if(S == Label::Debug)
		return "Debug";
	else if(S == Label::Warning)
		return "Warning";
	
	return "Critical";
}
