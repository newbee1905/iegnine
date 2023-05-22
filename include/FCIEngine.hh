#ifndef FC_IENGINE_HH
#define FC_IENGINE_HH

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
class FCIEngine : public HornIEngine {
public:
	bool parse() override;
	bool solve() override;
	bool entailment_check() override;
	void result_output() override;
};

} // namespace ie

#endif // FCIENGINE_HH
