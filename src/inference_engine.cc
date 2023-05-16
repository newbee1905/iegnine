#include "fmt/core.h"
#include "fmt/format.h"

#include "enums.hh"
#include "inference_engine.hh"
#include "statement.hh"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>

namespace ie {
void IEngine::split_expression(const std::string &expression) {
	std::stack<std::string> operators;
	std::sregex_token_iterator it(expression.begin(), expression.end(), IE_REGEX);
	std::sregex_token_iterator end;

	/* fmt::println("Dist {}", std::distance(it, end)); */
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
			m_tokens.emplace_back(token);
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

void IEngine::evaluate_tt2() {
	std::stack<ie::Statement> atomics;
	for (const auto &token : m_tokens) {
		if ((*token)[0] == ';') {
			if (atomics.size() == 1)
				continue;
			auto atomic1 = atomics.top();
			atomics.pop();
			auto atomic2 = atomics.top();
			atomics.pop();
			auto new_atomic             = ie::combine_name(*atomic2, *atomic1, *token);
			Statement::tt2[*new_atomic] = Statement::tt2[*atomic2] && Statement::tt2[*atomic1];
			atomics.push(new_atomic);

		} else if (ie::IEngine::is_operator((*token)[0])) {
			if ((*token)[0] == '~') {
				auto new_atomic             = ie::combine_name(*(atomics.top()), *token);
				Statement::tt2[*new_atomic] = !Statement::tt2[*atomics.top()];
				atomics.pop();
				atomics.push(new_atomic);
			} else {
				auto atomic1 = atomics.top();
				atomics.pop();
				auto atomic2 = atomics.top();
				atomics.pop();
				auto new_atomic = ie::combine_name(*atomic2, *atomic1, *token);
				switch ((*token)[0]) {
				case '&':
					Statement::tt2[*new_atomic] = Statement::tt2[*atomic2] && Statement::tt2[*atomic1];
					break;
				case '|': // ||
					Statement::tt2[*new_atomic] = Statement::tt2[*atomic2] || Statement::tt2[*atomic1];
					break;
				case '=': // =>
					Statement::tt2[*new_atomic] = !Statement::tt2[*atomic2] || Statement::tt2[*atomic1];
					break;
				case '<': // <=>
					Statement::tt2[*new_atomic] = (Statement::tt2[*atomic2] && Statement::tt2[*atomic1]) ||
					                              (!Statement::tt2[*atomic2] && !Statement::tt2[*atomic1]);
					break;
				}
				atomics.push(new_atomic);
			}
		} else {
			atomics.push(token);
		}
	}
	m_final_atomic = *(atomics.top());
}

void IEngine::evaluate_tt() {
	std::stack<ie::Statement> atomics;
	for (const auto &token : m_tokens) {
		if ((*token)[0] == ';') {
			if (m_final_atomic.empty()) {
				m_final_atomic = *(atomics.top());
			} else {
				auto new_final_topic = m_final_atomic + fmt::format("&({})", *(atomics.top()));
				/* fmt::println("{} => {}", m_final_atomic, Statement::tt2[m_final_atomic]);
				 */
				/* fmt::println("{} => {}", *atomics.top(), Statement::tt2[*atomics.top()]); */
				Statement::tt[new_final_topic] = [&]() -> bool {
					return Statement::tt[m_final_atomic]() && Statement::tt[*(atomics.top())]();
				};
				m_final_atomic = new_final_topic;
			}
		} else if (ie::IEngine::is_operator((*token)[0])) {
			if ((*token)[0] == '~') {
				auto new_atomic            = ie::combine_name(*(atomics.top()), *token);
				Statement::tt[*new_atomic] = [&]() -> bool { return !(Statement::tt[*atomics.top()]()); };
				atomics.pop();
				atomics.push(new_atomic);
			} else {
				auto atomic1 = atomics.top();
				atomics.pop();
				auto atomic2 = atomics.top();
				atomics.pop();
				auto new_atomic = ie::combine_name(*atomic2, *atomic1, *token);
				switch ((*token)[0]) {
				case '&':
					Statement::tt[*new_atomic] = [&]() -> bool {
						return Statement::tt[*atomic2]() && Statement::tt[*atomic1]();
					};
					break;
				case '|': // ||
					Statement::tt[*new_atomic] = [&]() -> bool {
						return Statement::tt[*atomic2]() || Statement::tt[*atomic1]();
					};
					break;
				case '=': // =>
					Statement::tt[*new_atomic] = [&]() -> bool {
						return !Statement::tt[*atomic2]() || Statement::tt[*atomic1]();
					};
					break;
				case '<': // <=>
					Statement::tt[*new_atomic] = [&]() -> bool {
						return (!Statement::tt[*atomic2]() && !Statement::tt[*atomic1]()) ||
						       (Statement::tt[*atomic2]() && Statement::tt[*atomic1]());
					};
					break;
				}
				atomics.push(new_atomic);
			}
		} else {
			atomics.push(token);
		}
	}
	if (m_final_atomic.empty()) {
		m_final_atomic = *(atomics.top());
	} else {
		std::string new_final_topic    = m_final_atomic + fmt::format("&({})", *(atomics.top()));
		Statement::tt[new_final_topic] = [&]() -> bool {
			return Statement::tt[m_final_atomic]() && Statement::tt[*(atomics.top())]();
		};
		m_final_atomic = new_final_topic;
	}
}
} // namespace ie
