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

std::vector<std::string> FCIEngine::split_string(std::string &input, char delimiter) {
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
	
	entailment_check(implications, queue, query_str);
}

void FCIEngine::entailment_check(std::unordered_map<std::string, std::vector<std::string>> &KB,
                 const std::unordered_set<std::string> &facts, const std::string &query) {
	std::unordered_set<std::string> inferred_facts = facts;

	bool result = false;

	while (true) {
		// is the query already a fact?
		result = inferred_facts.find(query) != inferred_facts.end();
		if (result) {
			fmt::print("YES\n");
			for (const auto &element : inferred_facts) {
				fmt::print("{} ", element);
			}
			return;
		}

		bool new_fact_inferred = false;

		//checking each fact in the fact queue
		for (const auto &currentFact : inferred_facts) {
			//check each sentence in KB
			for (auto &sentence : KB) {
				std::vector<std::string> &leftSide = sentence.second;
				const std::string &rightSide       = sentence.first;

				//if leftside is cleared, skip this sentence
				if (leftSide.size() == 0)
					continue;

				//find current fact symbol in the left side
				auto it = std::find(leftSide.begin(), leftSide.end(), currentFact);
				//if found, erase from left side to keep track of undetermined symbols
				if (it != leftSide.end()) {
					leftSide.erase(it);
				}

				// if leftside is cleared, add right side to fact queue
				if (leftSide.size() == 0) {
					// only add new inferred fact if it is actually new
					//(not already in list of inferred facts)
					if (inferred_facts.find(rightSide) == inferred_facts.end()) {
						inferred_facts.insert(rightSide);
						new_fact_inferred = true;
						break;
					}
				}
			}
			if (new_fact_inferred)
				break;
		}

		if (!new_fact_inferred)
			break;
	}

	fmt::print("NO\n");
	for (const auto &element : inferred_facts) {
		fmt::print("{} ", element);
	}

	return;
}
} // namespace ie
