#include "fmt/core.h"

#include "BCIEngine.hh"
#include "enums.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace ie {
bool BCIEngine::solve() {
	if (!parse())
		throw std::invalid_argument("Wrong format for FC");

	return entailment_check();
}

bool BCIEngine::parse() {
	std::sregex_token_iterator it(m_kb_str.begin(), m_kb_str.end(), HORN_REGEX);
	std::sregex_token_iterator end;

	int mode = 0;
	std::vector<std::string> literals;
	for (std::string token, prev = ""; it != end; ++it) {
		token = *it;
		switch (token[0]) {
		case '&':
			continue;
		case '=':
			++mode;
			break;
		case ';':
			if ((literals.size() + mode) == 1)
				m_inferred_facts.insert(literals[0]);
			literals.clear();
			mode = 0;
			break;
		default:
			if (mode)
				m_kb[token] = literals;
			else
				literals.push_back(token);
			break;
		}
	}
	fmt::println("");

	return true;
}

bool BCIEngine::entailment_check() { return _entailment_check(m_query_str); }

bool BCIEngine::_entailment_check(std::string query) {
	// check if current query is a known fact
	if (m_inferred_facts.find(query) != m_inferred_facts.end()) {
		m_required_facts.insert(query);
		return true;
	}

	// If the query is m_visited, it indicates a cycle in the knowledge base
	if (m_visited.find(query) != m_visited.end()) {
		return false;
	}

	// mark this query as m_visited
	m_visited.insert(query);

	// check each sentence in KB
	for (auto &[right_side, left_side] : m_kb) {
		// if found query on right side, start the recursion check on left side
		if (query == right_side) {
			bool is_left_side = true;

			// Check if all the premises of the rule are true
			for (const auto &symbol : left_side) {
				if (!_entailment_check(symbol)) {
					is_left_side = false;
					break;
				}
			}

			// If all symbols on the left are true, mark the query as fact
			if (is_left_side) {
				m_inferred_facts.insert(query);
				m_required_facts.insert(query);
				return true;
			}
		}
	}
	return false;
}

void BCIEngine::result_output() {
	for (const auto &element : m_required_facts) {
		fmt::print("{} ", element);
	}
	fmt::println("");
}
} // namespace ie
