#include "fmt/core.h"

#include "BCIEngine.hh"
#include "enums.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>

std::string delimiter = ";";
std::unordered_map<std::string, std::vector<std::string>> implications;
std::unordered_set<std::string> queue;
std::unordered_set<std::string> required_facts;

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

void BCIEngine::solve(const std::string &filename) {
	// read input from file
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
		} else {
			queue.insert(token);
		}
		kb_str.erase(0, pos + delimiter.length());
	}

	//list of visited symbols to prevent looping
	std::unordered_set<std::string> visited;

	//printing the results
	if (entailment_check(query_str, visited)) {
		fmt::print("YES: ");
		for (const auto &element : required_facts) {
			fmt::print("{} ", element);
		}
	} else {
		fmt::print("NO");
	}
}

bool BCIEngine::entailment_check(const std::string &query,
                                 std::unordered_set<std::string> &visited) {

	//check if current query is a known fact
	if (queue.find(query) != queue.end()) {
		required_facts.insert(query);
		return true;
	}

	// If the query is visited, it indicates a cycle in the knowledge base
	if (visited.find(query) != visited.end()) {
		return false;
	}

	// mark this query as visited
	visited.insert(query);

	// check each sentence in KB
	for (auto &sentence : implications) {
		std::vector<std::string> &leftSide = sentence.second;
		const std::string &rightSide       = sentence.first;

		//if found query on right side, start the recursion check on left side
		if (query == rightSide) {
			bool isLeftSideTrue = true;

			// Check if all the premises of the rule are true
			for (const auto &symbol : leftSide) {
				if (!entailment_check(symbol, visited)) {
					isLeftSideTrue = false;
					break;
				}
			}

			// If all symbols on the left are true, mark the query as fact
			if (isLeftSideTrue) {
				queue.insert(query);
				required_facts.insert(query);
				return true;
			}
		}
	}
	return false;
}
