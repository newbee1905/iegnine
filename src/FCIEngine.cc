#include "fmt/core.h"

#include "enums.hh"
#include "FCIEngine.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>

namespace ie {
std::string delimiter = ";";
std::unordered_map<std::string, std::vector<std::string>> implications;
std::unordered_set<std::string> queue;

std::vector<std::string> split_string(std::string &input, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;

	for (char ch : input) {
		if (ch == delimiter) {
			tokens.push_back(token);
			token.clear();
		} else {
			token.push_back(ch);
		}
	}

	// Add the last token if it exists
	if (!token.empty()) {
		tokens.push_back(token);
	}

	return tokens;
}

void FCIEngine::solve(const std::string &filename) {
	//read input from file
	std::ifstream input_file(filename);
	std::string kb_str;
	std::string query_str;

	int mode = 0;
	for (std::string line; getline(input_file, line);) {
		if (line == "TELL" || line == "ASK") {
			mode++;
			continue;
		}
		fmt::println("{}", line);
		switch (mode) {
		case 1:
			kb_str = line;
			break;
		case 2:
			query_str = line;
			break;
		}
	}

	input_file.close();
	
	// Parse the Knowledge Base
	size_t pos = 0;
	std::string token;
	while ((pos = kb_str.find(delimiter)) != std::string::npos) {
		token = kb_str.substr(0, pos);
		token.erase(std::remove_if(token.begin(), token.end(), ::isspace),
		            token.end()); // Remove spaces
		if (token.find("=>") != std::string::npos) {
			std::string leftSide  = token.substr(0, token.find("=>"));
			std::string rightSide = token.substr(token.find("=>") + 2);
			
			implications[rightSide] = split_string(leftSide, '&');
		}
		else {
			queue.insert(token);
		}
		kb_str.erase(0, pos + delimiter.length());
	}

	// Print all elements in implications
	for (const auto &entry : implications) {
		int i = 0;
		for (const auto &value : entry.second) {
			if (i != 0) {
				fmt::print("&");
			}
			fmt::print("{}", value);
			i++;
		}
		fmt::print("=>{}\n", entry.first);
	}

	// Print all elements in queue/facts
	for (const auto &element : queue) {
		fmt::print("{}\n", element);
	}
	
	is_entailed(implications, queue, query_str);
}

bool FCIEngine::is_entailed(const std::unordered_map<std::string, std::vector<std::string>> &KB,
                 const std::unordered_set<std::string> &facts, const std::string &query) {
	std::unordered_set<std::string> inferred_facts = facts;

	while (true) {
		bool new_fact_inferred = false;

		for (const auto &rule : KB) {
			const std::vector<std::string> &premises = rule.second;
			const std::string &conclusion            = rule.first;

			bool all_premises_satisfied = true;
			for (const auto &premise : premises) {
				if (inferred_facts.find(premise) == inferred_facts.end()) {
					all_premises_satisfied = false;
					break;
				}
			}

			if (all_premises_satisfied && inferred_facts.find(conclusion) == inferred_facts.end()) {
				inferred_facts.insert(conclusion);
				new_fact_inferred = true;
			}
		}

		if (!new_fact_inferred) {
			break;
		}
	}

	bool result = inferred_facts.find(query) != inferred_facts.end();
	if (result)
		fmt::print("YES\n");
	else
		fmt::print("NO\n");
	for (const auto &element : inferred_facts) {
		fmt::print("{} ", element);
	}
	return result;
}
} // namespace ie
