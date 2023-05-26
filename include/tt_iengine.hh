#ifndef TT_IENGINE_HH
#define TT_IENGINE_HH

#include <algorithm>
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "clause.hh"
#include "iengine.hh"
#include "utils.hh"

namespace ie {
class TTIEngine : public IEngine {
private:
	std::vector<std::string> m_kb_tokens;
	std::vector<std::string> m_query_tokens;
	std::shared_ptr<Clause> m_kb_clause;
	std::shared_ptr<Clause> m_query_clause;
	int m_count      = 0;
	bool optimal_cnf = false;
	bool dpll        = false;

public: // setter + getter
	TTIEngine(bool optimal_cnf, bool dpll) : optimal_cnf(optimal_cnf), dpll(dpll) {}
	std::vector<std::string> &kb_tokens() { return m_kb_tokens; }
	std::vector<std::string> &query_tokens() { return m_query_tokens; }

private:
	void _parse(std::vector<std::string> &out, const std::string &expression);
	void _cnf(std::shared_ptr<Clause> &out, const std::vector<std::string> &token);
	bool _dpll(std::shared_ptr<Clause> &c);
	bool _get(std::shared_ptr<Clause> &c);

public:
	bool parse() override;
	bool solve() override;
	bool entailment_check() override;
	void result_output() override;

public: // static functions
	static constexpr ie::map<std::string_view, std::int8_t, 8> precdence{
			ie::pair<std::string_view, std::int8_t>{  ";",  5},
			ie::pair<std::string_view, std::int8_t>{  "~",  3},
			ie::pair<std::string_view, std::int8_t>{  "&",  2},
			ie::pair<std::string_view, std::int8_t>{ "||",  2},
			ie::pair<std::string_view, std::int8_t>{ "=>",  1},
			ie::pair<std::string_view, std::int8_t>{"<=>",  1},
			ie::pair<std::string_view, std::int8_t>{  ")", -1},
			ie::pair<std::string_view, std::int8_t>{  "(", -1},
	};

	static constexpr bool is_operator(char c) {
		switch (c) {
		case '~':
		case '&':
		case '|': // ||
		case '=': // =>
		case '<': // <=>
			return true;
		default:
			return false;
		}
	}
};

} // namespace ie

#endif // TT_IENGINE_HH
