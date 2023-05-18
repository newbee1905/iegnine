#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "utils.hh"

namespace ie {

class FCIEngine {
private:
	std::string delimiter = ";";

public:
	void solve(const std::string &expression);
	void entailment_check(std::unordered_map<std::string, std::vector<std::string>> &KB,
	                 const std::unordered_set<std::string> &facts, const std::string &query);

public:

};
} // namespace ie