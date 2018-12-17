#include "shader.h"

Shader::Shader(std::string const& shader1, ShaderType type1, std::string const& shader2, ShaderType type2, unsigned max_depth) : program_{0}, max_depth_{max_depth} {
	if(type1 == type2)
		throw ArgumentMismatchException{"Provided shader types must differ\n"};

	auto s = read_source(shader1);
	std::cout << s << "\n";
}


std::string Shader::read_source(std::string const& source) const{
	std::stack<std::ifstream, std::vector<std::ifstream>> streams;
	streams.push(std::ifstream{source});
	
	if(!streams.top().is_open())
		throw FileIOException{"Unable to open " + source + " for reading\n"};

	std::string contents{}, line;	
	
	while(!streams.empty()){

		while(std::getline(streams.top(), line)){
			if(auto idx = line.find(" "); line.substr(0, idx) == "#include"){
				/* Max include depth reached, to prevent infinite recursion */
				if(streams.size() >= max_depth_){
					std::cerr << "Warning, max include depth reached, omitting\n";
					continue;
				}
				
				/* Process include directive */
				auto incl_res = process_include(line, source, idx);
				/* Ill formed, omit file */
				if(!incl_res.well_formed){
					std::cerr << "Error including file " << incl_res.content << ", incorrect quotation. Omitting include directive\n";
					continue;
				}

				/* c/c++ style header guard to prevent redefinition */		
				std::string header_guard = format_header_guard(incl_res.content);

				/* Append header guard */
				contents.append("#ifndef " + header_guard + "\n");	
				contents.append("#define " + header_guard + "\n");

				std::ifstream file{incl_res.content};					
				
				if(!file.is_open()){
					std::cerr << "Error opening file " << incl_res.content << ", file not found. Omitting include directive\n";
					continue;
				}
				streams.push(std::move(file));
			}
			/* Version already specified by root file, skip */
			else if(streams.size() > 1 && line.substr(0, line.find(" ")) == "#version")
				continue;
			/* Add line to source */
			else
				contents.append(line + "\n");
			
		}
		streams.top().close();
		streams.pop();

		/* End c/c++ style header guard */
		if(!streams.empty())
			contents.append("#endif");
	}

	return contents;	
}

std::string Shader::format_header_guard(std::string const& path) const {
	
	std::string header_guard = path;
	if(header_guard.find('/') != std::string::npos)
		header_guard = header_guard.substr(header_guard.find_last_of('/')+1, header_guard.size() -1);

	std::transform(std::begin(header_guard), std::end(header_guard), std::begin(header_guard), ::toupper);
	std::transform(std::begin(header_guard), std::end(header_guard), std::begin(header_guard), [] (char c) {
		return c == '.' ? '_' : c;
	});
	
	return header_guard;
}

Shader::IncludeResult Shader::process_include(std::string const& directive, std::string const& source, std::size_t idx) const{
	IncludeResult result{true, std::string{}};

	/* File name */
	std::string incl_path  = directive.substr(idx+1, directive.length()-1);
	
	/* File to be included must be given in double quotes */
	if(incl_path[0] != '"' || incl_path[incl_path.size() - 1] != '"')
		result.well_formed = false;

	 /* Remove quotations */ 
	incl_path = incl_path.substr(1, incl_path.size() - 2);
	

	/* Append file name to source's path if needed */
	if(source.find('/') != std::string::npos)
		incl_path = source.substr(0, source.find_last_of('/')+1) + incl_path;	

	result.content = std::move(incl_path);

	return result;
}
