#include "fmt/core.h"

#include "enums.hh"
#include "FCIEngine.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>

namespace ie {
std::vector<std::string> m_tokens{};
std::unordered_map<std::string, std::unordered_set<std::string>> implications;
std::unordered_set<std::string> queue;

void FCIEngine::split_expression(const std::string &expression) {
	std::regex delimiter(";");
	std::sregex_token_iterator it(expression.begin(), expression.end(), delimiter, -1);
	std::sregex_token_iterator end;

	// Store the tokens in the m_tokens vector
	m_tokens.clear();
	while (it != end) {
		std::string token = *it;
		token.erase(std::remove_if(token.begin(), token.end(), ::isspace),
		            token.end()); // Remove spaces
		m_tokens.push_back(token);
		++it;
	}

	// Print the tokens
	for (const auto &token : m_tokens) {
		fmt::println("{}", token);
	}

	//inputting the implications
}

void FCIEngine::add_implication(const std::string &implication) {
	std::size_t arrowPos = implication.find("=>");
	if (arrowPos != std::string::npos) {
		std::string antecedent = implication.substr(0, arrowPos);
		std::string consequent = implication.substr(arrowPos + 2);
		implications[consequent].insert(antecedent);
	}
}

void FCIEngine::add_queue(const std::string &fact) {
	queue.insert(fact);
}

bool FCIEngine::is_entailed(const std::string &query) {
	std::unordered_set<std::string> visited;
	std::vector<std::string> agenda(queue.begin(), queue.end());

	while (!agenda.empty()) {
		std::string fact = agenda.back();
		agenda.pop_back();

		if (fact == query) {
			return true;
		}

		if (visited.count(fact) > 0) {
			continue;
		}

		visited.insert(fact);

		if (implications.count(fact) > 0) {
			const auto &antecedents = implications[fact];
			agenda.insert(agenda.end(), antecedents.begin(), antecedents.end());
		}
	}

	return false;
}

} // namespace ie
