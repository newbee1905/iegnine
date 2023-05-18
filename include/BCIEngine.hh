#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "utils.hh"

class BCIEngine {
private:
	std::string delimiter = ";";

public:
	void solve(const std::string &filename);
	bool entailment_check(const std::string &query, std::unordered_set<std::string> &visited);

public:
};