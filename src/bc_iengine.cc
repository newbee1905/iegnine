#include "fmt/core.h"

#include "bc_iengine.hh"
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
				m_inferred_facts.push_back(literals[0]);
			literals.clear();
			mode = 0;
			break;
		default:
			if (mode)
				m_kb[token].push_back(literals);
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
	if (std::find(m_inferred_facts.begin(), m_inferred_facts.end(), query) !=
	    m_inferred_facts.end()) {
		if (std::find(m_required_facts.begin(), m_required_facts.end(), query) ==
		    m_required_facts.end()) {
			m_required_facts.push_back(query);
		}
		return true;
	}

	// If the query is m_visited, it indicates a cycle in the knowledge base
	if (m_visited.find(query) != m_visited.end()) {
		return false;
	}

	// mark this query as m_visited
	m_visited.insert(query);

	// check each sentence in KB
	for (auto &[right_side, left_sides] : m_kb) {
		// if found query on right side, start the recursion check on left side
		if (query == right_side) {
			bool is_left_side = false;

			// check each leftsides
			for (const auto &left_side : left_sides) {
				is_left_side = false;

				// Check if all the premises of the left side are true
				for (const auto &symbol : left_side) {
					// if true, keep going until last symbol/premise
					// if just one symbol/premise is false, mark as false
					// and skip this leftside(sentence)
					if (_entailment_check(symbol)) {
						is_left_side = true;
					} else {
						is_left_side = false;
						break;
					}
				}

				// if just one left side is proven, this current right side will be a fact
				// break the loop since already proven, no need to keep searching
				if (is_left_side) {
					break;
				}
			}

			// if just one left side is proven, this current query will be a fact
			if (is_left_side) {
				m_inferred_facts.push_back(query);
				m_required_facts.push_back(query);
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
