#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <regex>
#include <string>
#include <vector>

#include "utils.hh"

namespace ie {
const std::regex IE_REGEX("([&~)(]|[;\\n]|[|=][|>]|<=>)|\\w+");
constexpr ie::map<std::string_view, std::int8_t, 8> precdence{
		ie::pair<std::string_view, std::int8_t>{  ";",  5},
		ie::pair<std::string_view, std::int8_t>{  "~",  3},
		ie::pair<std::string_view, std::int8_t>{  "&",  2},
		ie::pair<std::string_view, std::int8_t>{ "||",  1},
		ie::pair<std::string_view, std::int8_t>{ "=>",  0},
		ie::pair<std::string_view, std::int8_t>{"<=>",  0},
		ie::pair<std::string_view, std::int8_t>{  ")", -1},
		ie::pair<std::string_view, std::int8_t>{  "(", -1},
};

constexpr bool is_operator(char c) {
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
std::vector<std::string> split_expression(const std::string &expression);
} // namespace ie

#endif
