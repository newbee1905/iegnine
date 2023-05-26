#ifndef BC_IENGINE_HH
#define BC_IENGINE_HH

#include <algorithm>
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "iengine.hh"
#include "utils.hh"

namespace ie {
class BCIEngine : public HornIEngine {
private:
	std::unordered_set<std::string> m_visited;
	std::unordered_set<std::string> m_required_facts;

public:
	bool parse() override;
	bool solve() override;
	bool entailment_check() override;
	bool _entailment_check(std::string symbol);
	void result_output() override;
};
} // namespace ie

#endif // BC_IENGINE_HH
