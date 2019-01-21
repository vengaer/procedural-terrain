#include "logger.h"

std::string logging::to_string(Severity S) {
	if(S == Severity::Debug)
		return "Debug";
	else if(S == Severity::Warning)
		return "Warning";
	
	return "Critical";
}
