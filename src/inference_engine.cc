#include "fmt/core.h"

#include "enums.hh"
#include "inference_engine.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>

namespace ie {
std::vector<std::string> m_tokens{};
/* std::unordered_map<std::string, bool> tt{}; */
std::unordered_map<std::string, std::function<bool()>> tt{};

void IEngine::split_expression(const std::string &expression) {
	std::stack<std::string> operators;
	std::sregex_token_iterator it(expression.begin(), expression.end(), IE_REGEX);
	std::sregex_token_iterator end;

	fmt::println("Dist {}", std::distance(it, end));
	m_tokens.reserve(std::distance(it, end)); // Reserve space for m_tokens

	// shunting yard algorithm for parsing m_tokens
	// and order them to the correct order of the
	// piority of functions and () brackets
	for (std::string token; it != end; ++it) {
		token = std::string(*it);

		switch (token[0]) {
		case '(':
			operators.push(token);
			break;
		case ')':
			while (!operators.empty() && operators.top() != "(") {
				m_tokens.emplace_back(operators.top());
				operators.pop();
			}

			operators.pop();
			break;
			// the operators
		case ';':
			for (; !operators.empty(); m_tokens.emplace_back(operators.top()), operators.pop())
				;
			m_tokens.emplace_back("&");
			break;
		case '~':
		case '&':
		case '|': // ||
		case '=': // =>
		case '<': // <=>
			// push all higher piority operators to the m_tokens
			while (!operators.empty() && precdence[token] <= precdence[operators.top()]) {
				m_tokens.emplace_back(operators.top());
				operators.pop();
			}
			operators.push(token);
			break;
		default:
			m_tokens.emplace_back(token);
			break;
		}
	}

	// TODO: Has not parsed the situation where the format is wrong
	// with a non-closed )
	for (; !operators.empty(); m_tokens.emplace_back(operators.top()), operators.pop())
		;
}
} // namespace ie
