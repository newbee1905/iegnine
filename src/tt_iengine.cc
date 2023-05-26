#include "tt_iengine.hh"
#include "clause.hh"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include <memory>
#include <stack>
#include <variant>

namespace ie {
bool TTIEngine::solve() {
	if (!parse())
		throw std::invalid_argument("Wrong format for TT");

	return entailment_check();
}

void TTIEngine::_parse(std::vector<std::string> &out, const std::string &expression) {
	std::stack<std::string> operators;
	std::sregex_token_iterator it(expression.begin(), expression.end(), TT_REGEX);
	std::sregex_token_iterator end;

	out.reserve(std::distance(it, end)); // Reserve space for m_tokens

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
				out.emplace_back(operators.top());
				operators.pop();
			}

			operators.pop();
			break;
			// the operators
		case ';':
			for (; !operators.empty(); out.emplace_back(operators.top()), operators.pop())
				;
			out.emplace_back(token);
			break;
		case '~':
		case '&':
		case '|': // ||
		case '=': // =>
		case '<': // <=>
			// push all higher piority operators to the m_tokens
			while (!operators.empty() && precdence[token] <= precdence[operators.top()]) {
				out.emplace_back(operators.top());
				operators.pop();
			}
			operators.push(token);
			break;
		default:
			out.emplace_back(token);
			break;
		}
	}

	// TODO: Has not parsed the situation where the format is wrong
	// with a non-closed )
	for (; !operators.empty(); out.emplace_back(operators.top()), operators.pop())
		;
}

void TTIEngine::_cnf(std::shared_ptr<Clause> &out, const std::vector<std::string> &tokens) {
	std::stack<std::shared_ptr<Clause>> cur_literals;
	std::shared_ptr<Clause> cur_left, cur_right;
	std::string cur_token;
	for (const auto &token : tokens) {
		if (token[0] == ';') {
			if (cur_literals.size() == 1)
				continue;
			cur_right = cur_literals.top();
			cur_literals.pop();
			cur_left = cur_literals.top();
			cur_literals.pop();

			cur_literals.push(Clause::Conjunction(cur_left, cur_right, optimal_cnf));
		} else if (is_operator(token[0])) {
			switch (token[0]) {
			case '~':
				cur_left = cur_literals.top();
				cur_literals.pop();

				cur_literals.push(Clause::Negation(cur_left, optimal_cnf));
				break;
			case '&':
				cur_right = cur_literals.top();
				cur_literals.pop();
				cur_left = cur_literals.top();
				cur_literals.pop();

				cur_literals.push(Clause::Conjunction(cur_left, cur_right, optimal_cnf));
				break;
			case '|':
				cur_right = cur_literals.top();
				cur_literals.pop();
				cur_left = cur_literals.top();
				cur_literals.pop();

				cur_literals.push(Clause::Disjunction(cur_left, cur_right, optimal_cnf));
				break;
			case '=':
				cur_right = cur_literals.top();
				cur_literals.pop();
				cur_left = cur_literals.top();
				cur_literals.pop();

				cur_left = Clause::Negation(cur_left, optimal_cnf);

				cur_literals.push(Clause::Disjunction(cur_left, cur_right, optimal_cnf));
				break;
			case '<':
				cur_right = cur_literals.top();
				cur_literals.pop();
				cur_left = cur_literals.top();
				cur_literals.pop();

				auto neg_cur_left  = Clause::Negation(cur_left, optimal_cnf);
				auto neg_cur_right = Clause::Negation(cur_right, optimal_cnf);

				cur_left  = Clause::Conjunction(cur_left, cur_right, optimal_cnf);
				cur_right = Clause::Conjunction(neg_cur_left, neg_cur_right, optimal_cnf);

				cur_literals.push(Clause::Disjunction(cur_left, cur_right, optimal_cnf));
				break;
			}

		} else {
			if (Clause::Clauses.find(token) == Clause::Clauses.end()) {
				Clause::Clauses[token]        = std::make_shared<Clause>();
				Clause::Clauses[token]->token = token;
			}
			cur_literals.push(Clause::Clauses[token]);
			Clause::Literals.insert(token);
		}
	}

	out = cur_literals.top();
}

bool TTIEngine::_dpll(std::shared_ptr<Clause> &c) {
	bool update = false;
	std::string abs_val_left, abs_val_right;
	if (c->left == nullptr) {
		abs_val_left = c->token;
		if (abs_val_left[0] == '~') {
			abs_val_left = abs_val_left.substr(1);
		}
		if (Clause::Literals.find(abs_val_left) != Clause::Literals.end()) {
			Clause::TruthTable[abs_val_left] = !c->neg;
			Clause::Literals.erase(abs_val_left);
			c      = nullptr;
			update = true;
		}
	} else if (c->op == "||") {
		if (c->left->left == nullptr) {
			abs_val_left = c->left->token;
			if (abs_val_left[0] == '~') {
				abs_val_left = abs_val_left.substr(1);
			}
			if (Clause::Literals.find(abs_val_left) == Clause::Literals.end()) {
				if (((Clause::TruthTable[abs_val_left] + c->left->neg) & 1))
					c = c->right;
				else
					c = nullptr;
				update = true;
			}
		} else if (c->right->left == nullptr) {
			abs_val_right = c->right->token;
			if (abs_val_right[0] == '~') {
				abs_val_right = abs_val_right.substr(1);
			}
			if (Clause::Literals.find(abs_val_right) == Clause::Literals.end()) {
				if (((Clause::TruthTable[abs_val_right] + c->right->neg) & 1))
					c = c->left;
				else
					c = nullptr;
				update = true;
			}
		}

	} else {
		update = update || _dpll(c->left);
		update = update || _dpll(c->right);
		if (c->left == nullptr && c->right == nullptr) {
			c = nullptr;
		} else if (c->left == nullptr) {
			c = c->right;
		} else {
			c = c->left;
		}
	}
	return update;
}

bool TTIEngine::parse() {
	_parse(m_kb_tokens, m_kb_str);
	_parse(m_query_tokens, m_query_str);

	_cnf(m_kb_clause, m_kb_tokens);
	_cnf(m_query_clause, m_query_tokens);

	fmt::println("KB: {}", m_kb_clause->token);
	/* fmt::println("Query: {}", m_query_clause->token); */

	if (dpll) {
		while (_dpll(m_kb_clause))
			;
		while (_dpll(m_query_clause))
			;
	}

	/* std::stack<std::shared_ptr<Clause>> s; */
	/* s.push(m_query_clause); */
	/* while (!s.empty()) { */
	/* 	auto top = s.top(); */
	/* 	s.pop(); */
	/* 	if (top == nullptr) */
	/* 		continue; */
	/* 	s.push(top->left); */
	/* 	s.push(top->right); */
	/**/
	/* 	fmt::println("Token: {}", top->token); */
	/* } */

	return true;
}

bool TTIEngine::_get(std::shared_ptr<Clause> &c) {
	if (c->left != nullptr) {
		if (c->op == "&")
			return _get(c->left) && _get(c->right);
		return _get(c->left) || _get(c->right);
	}

	auto abs_c = c->token;
	if (abs_c[0] == '~')
		abs_c = abs_c.substr(1);

	return ((Clause::TruthTable[abs_c] + c->neg) & 1);
}

bool TTIEngine::entailment_check() {
	/* for (const auto &literal : Clause::Literals) { */
	/* 	fmt::print("| {} ", literal); */
	/* } */
	/* fmt::println("| R | Q |"); */

	for (std::uint64_t i = 0; i < (1 << Clause::Literals.size()); ++i) {
		auto op_itr = Clause::Literals.begin();
		for (std::int16_t j = 0; op_itr != Clause::Literals.end(); ++op_itr, ++j) {
			Clause::TruthTable[*op_itr] = (bool)(i & (1ull << j));
			/* fmt::print("| {} ", (bool)(i & (1ull << j)) ? 1 : 0); */
		}

		auto kb = _get(m_kb_clause);
		auto q  = _get(m_query_clause);
		/* fmt::println("| {} | {} |", kb ? 1 : 0, q ? 1 : 0); */
		if (kb && !q) {
			return false;
		}
		m_count += kb && q;
	}
	return true;
}

void TTIEngine::result_output() {
	/* for (auto &[clause_token, clause_ptr] : Clause::Clauses) { */
	/* 	fmt::println("{}", clause_token); */
	/* } */
	fmt::print(" {}", m_count);
}
}; // namespace ie
