#ifndef INFERENCE_ENGINE_HH
#define INFERENCE_ENGINE_HH

#include <compare>
#include <functional>
#include <iterator>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "statement.hh"
#include "utils.hh"

#include "fmt/format.h"
#include "fmt/ostream.h"

namespace ie {
const std::regex IE_REGEX("([&~)(]|[;\\n]|[|=][|>]|<=>)|\\w+");

class IEngine {
private:
	std::string m_final_atomic;
	std::vector<Statement> m_tokens;

public:
	// getter + setter
	std::vector<Statement> &tokens() { return m_tokens; };
	std::string &final_atomic() { return m_final_atomic; };

	void split_expression(const std::string &expression);
	void evaluate_tt();
	void evaluate_tt2();

	IEngine() { m_final_atomic = ""; }

public: // static functions
	constexpr static ie::map<std::string_view, std::int8_t, 8> precdence{
			ie::pair<std::string_view, std::int8_t>{  ";",  5},
			ie::pair<std::string_view, std::int8_t>{  "~",  3},
			ie::pair<std::string_view, std::int8_t>{  "&",  2},
			ie::pair<std::string_view, std::int8_t>{ "||",  1},
			ie::pair<std::string_view, std::int8_t>{ "=>",  0},
			ie::pair<std::string_view, std::int8_t>{"<=>",  0},
			ie::pair<std::string_view, std::int8_t>{  ")", -1},
			ie::pair<std::string_view, std::int8_t>{  "(", -1},
	};

	constexpr static bool is_operator(char c) {
		return (c == '~' || c == '&' || c == '|' || c == '=' || c == '<');
	}

	constexpr static bool is_separator(char c) { return (c == ';' || c == '\n'); }
};

} // namespace ie

#endif
