#include "fmt/core.h"

#include "FCIEngine.hh"
#include "enums.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace ie {
bool FCIEngine::solve() {
	if (!parse())
		throw std::invalid_argument("Wrong format for FC");

	return entailment_check();
}

bool FCIEngine::parse() {
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

bool FCIEngine::entailment_check() {
	bool new_fact_inferred = false;
	while (true) {
		// is the m_query_str already a fact?
		if (std::find(m_inferred_facts.begin(), m_inferred_facts.end(), m_query_str) !=
		    m_inferred_facts.end())
			return true;

		// checking each fact in the fact queue
		for (const auto &cur_fact : m_inferred_facts) {
			// check each sentence with same right side in m_kb
			for (auto &[right_side, left_sides] : m_kb) {
				//check each left side of those sentences
				for (auto &left_side : left_sides) {
					// if leftside is cleared, skip this sentence
					if (left_side.empty())
						continue;

					// find current fact symbol in the left side
					auto it = std::find(left_side.begin(), left_side.end(), cur_fact);
					// if found, erase from left side to keep track of undetermined symbols
					if (it != left_side.end())
						left_side.erase(it);

					// if leftside is cleared, add right side to fact queue
					if (left_side.size() > 0) {
						continue;
					}

					// only add new inferred fact if it is actually new
					// (not already in list of inferred facts)
					new_fact_inferred = (std::find(m_inferred_facts.begin(), m_inferred_facts.end(),
					                               right_side) == m_inferred_facts.end());
					if (new_fact_inferred) {
						m_inferred_facts.push_back(right_side);
						break;
					}
				}
			}
			if (new_fact_inferred)
				break;
		}
		if (!new_fact_inferred)
			break;
		new_fact_inferred = false;
	}

	return false;
}

void FCIEngine::result_output() {
	for (const auto &element : m_inferred_facts) {
		fmt::print("{} ", element);
	}
	fmt::println("");
}
} // namespace ie
