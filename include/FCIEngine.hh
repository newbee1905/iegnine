#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils.hh"

namespace ie {

class FCIEngine {
private:
	std::string delimiter = ";";

public:
	void solve(const std::string &expression);
	bool is_entailed(const std::unordered_map<std::string, std::vector<std::string>> &KB,
	                 const std::unordered_set<std::string> &facts, const std::string &query);

public:

};
} // namespace ie