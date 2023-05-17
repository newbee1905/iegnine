#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hh"

namespace ie {

class FCIEngine {
private:
	std::vector<std::string> m_tokens;

public:
	// getter + setter
	std::vector<std::string> &tokens() { return m_tokens; };
	void solve(const std::string &expression);
	void add_queue(const std::string &fact);
	void add_implication(const std::string &implication);
	bool is_entailed(const std::string &query);

public:

};
} // namespace ie