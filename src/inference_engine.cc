#include "fmt/core.h"

#include "enums.hh"
#include "inference_engine.hh"

#include <array>
#include <fstream>
#include <sstream>
#include <unordered_set>

ENUM_MAP(OperatorPrecedence, Biconditional, Implication, Disjunction, Conjunction, Negation);

namespace ie {
std::vector<std::string> split_expression(const std::string &expression) {
	std::vector<std::string> tokens;
	std::stack<std::string> operators;
	std::sregex_token_iterator it(expression.begin(), expression.end(), IE_REGEX);
	std::sregex_token_iterator end;

	fmt::println("Dist {}", std::distance(it, end));
	tokens.reserve(std::distance(it, end)); // Reserve space for tokens

	// shunting yard algorithm for parsing tokens
	// and order them to the correct order of the
	// piority of functions and () brackets
	for (std::string token; it != end; ++it) {
		token = std::string(*it);

		switch (token[0]) {
		case '(':
			operators.push(token);
			break;
		case ')':
			for (; !operators.empty() && operators.top() != "(";
			     tokens.push_back(operators.top()), operators.pop())
				;

			if (operators.empty() || operators.top() == "(") {
				// TODO: Handle the error later
				fmt::print("Error: Unmatched parentheses\n");
			}

			operators.pop();
			break;
			// the operators
		case ';':
			for (; !operators.empty(); tokens.push_back(operators.top()), operators.pop())
				;
			break;
		case '~':
		case '&':
		case '|': // ||
		case '=': // =>
		case '<': // <=>
			// push all higher piority operators to the tokens
			for (; !operators.empty() && precdence[token] <= precdence[operators.top()];
			     tokens.push_back(operators.top()), operators.pop())
				;
			operators.push(token);
			break;
		default:
			tokens.push_back(token);
			break;
		}
	}

	// TODO: Has not parsed the situation where the format is wrong
	// with a non-closed )
	for (; !operators.empty(); tokens.push_back(operators.top()), operators.pop())
		;

	return tokens;
}
} // namespace ie
