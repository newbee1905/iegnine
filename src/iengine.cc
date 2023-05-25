#include "iengine.hh"
#include <vector>

namespace ie {
std::vector<std::string> IEngine::split_string(std::string s, const char delim) {
	std::vector<std::string> out;

	for (size_t pos = 0; (pos = s.find(delim)) != std::string::npos;) {
		out.emplace_back(s.substr(0, pos));
		s = s.substr(pos + 1);
	}
	if (!s.empty())
		out.emplace_back(s);

	return out;
}

void IEngine::split_string(std::vector<std::string> out, std::string s, const char delim) {
	out.clear();

	for (size_t pos = 0; (pos = s.find(delim)) != std::string::npos;) {
		out.emplace_back(s.substr(0, pos));
		s = s.substr(pos + 1);
	}
	if (!s.empty())
		out.emplace_back(s);
}
}; // namespace ie
