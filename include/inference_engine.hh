#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hh"

namespace ie {
const std::regex IE_REGEX("([&~)(]|[;\\n]|[|=][|>]|<=>)|\\w+");

class IEngine {
private:
	std::string m_final_atomic;
	std::vector<std::string> m_tokens;
	/* std::unordered_map<std::string, bool> tt{}; */
	std::unordered_map<std::string, std::function<bool()>> tt;

public:
	// getter + setter
	std::vector<std::string> &tokens() { return m_tokens; };

	void split_expression(const std::string &expression);
	void evaluate_tt();

public:
	static constexpr ie::map<std::string_view, std::int8_t, 8> precdence{
			ie::pair<std::string_view, std::int8_t>{  ";",  5},
			ie::pair<std::string_view, std::int8_t>{  "~",  3},
			ie::pair<std::string_view, std::int8_t>{  "&",  1},
			ie::pair<std::string_view, std::int8_t>{ "||",  1},
			ie::pair<std::string_view, std::int8_t>{ "=>",  2},
			ie::pair<std::string_view, std::int8_t>{"<=>",  2},
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

#endif
