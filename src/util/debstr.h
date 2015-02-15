#pragma once

#include <sstream>

class debstr : public std::stringstream {
public:
	~debstr() {
		std::string s = str();
		CA2T uc(s.c_str(), CP_ACP);  
		OutputDebugString(uc);
	}
};
