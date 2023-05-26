#include "clause.hh"
#include "fmt/core.h"
#include "fmt/ostream.h"
#include <memory>
#include <stack>

namespace ie {
std::unordered_map<std::string, std::shared_ptr<Clause>> Clause::Clauses{};
std::unordered_set<std::string> Clause::Literals{};
std::unordered_map<std::string, int> Clause::TruthTable{};

std::shared_ptr<Clause> Clause::Negation(std::shared_ptr<Clause> left, bool optimal_cnf) {
	std::shared_ptr<Clause> cur, cur_left, cur_right;
	std::string cur_token, cur_op;

	if (left->left == nullptr) {
		if (left->neg) {
			cur_token = left->token.substr(1);
		} else {
			cur_token = fmt::format("~{}", left->token);
		}

		if (Clause::Clauses.find(cur_token) == Clause::Clauses.end()) {
			Clauses[cur_token]        = std::make_shared<Clause>();
			Clauses[cur_token]->token = cur_token;
			Clauses[cur_token]->neg   = !left->neg;
		}

		return Clause::Clauses[cur_token];
	}

	cur_left  = Negation(left->left, optimal_cnf);
	cur_right = Negation(left->right, optimal_cnf);
	if (left->op == "&") {
		return Disjunction(cur_left, cur_right, optimal_cnf);
	} else {
		return Conjunction(cur_left, cur_right, optimal_cnf);
	}
}

std::shared_ptr<Clause> Clause::Disjunction(std::shared_ptr<Clause> left,
                                            std::shared_ptr<Clause> right, bool optimal_cnf) {
	std::shared_ptr<Clause> cur, cur_left, cur_right;
	std::string cur_token, cur_op;

	if (left == nullptr)
		return right;
	if (right == nullptr)
		return left;

	if (left == right)
		return left;

	std::string abs_left = left->token;
	if (abs_left[0] == '~')
		abs_left = abs_left.substr(1);
	std::string abs_right = right->token;
	if (abs_right[0] == '~')
		abs_right = abs_right.substr(1);

	if (optimal_cnf) {
		std::stack<std::shared_ptr<Clause>> s;
		s.push(right);
		while (!s.empty()) {
			auto top = s.top();
			s.pop();
			if (top == nullptr)
				continue;
			s.push(top->left);
			s.push(top->right);

			if (abs_left == top->token && left->neg != top->neg) {
				return nullptr;
			}
		}

		for (; !s.empty(); s.pop())
			;

		s.push(left);
		while (!s.empty()) {
			auto top = s.top();
			s.pop();
			if (top == nullptr)
				continue;
			s.push(top->left);
			s.push(top->right);

			if (abs_right == top->token && right->neg != top->neg) {
				return nullptr;
			}
		}
	}

	if (left->op != "&" && right->op != "&") {
		cur_token = fmt::format("{}||{}", left->token, right->token);

		if (Clause::Clauses.find(cur_token) == Clause::Clauses.end()) {
			Clauses[cur_token]        = std::make_shared<Clause>();
			Clauses[cur_token]->token = cur_token;
			Clauses[cur_token]->left  = left;
			Clauses[cur_token]->right = right;
			Clauses[cur_token]->op    = "||";
		}

		return Clause::Clauses[cur_token];
	}

	if (right->op == "&") {
		return Conjunction(Disjunction(left, right->left, optimal_cnf),
		                   Disjunction(left, right->right, optimal_cnf), optimal_cnf);
	}

	return Conjunction(Disjunction(left->left, right, optimal_cnf),
	                   Disjunction(left->right, right, optimal_cnf), optimal_cnf);
}

std::shared_ptr<Clause> Clause::Conjunction(std::shared_ptr<Clause> left,
                                            std::shared_ptr<Clause> right, bool optimal_cnf) {
	std::shared_ptr<Clause> cur, cur_left, cur_right;
	std::string cur_token, cur_op;

	if (left == nullptr)
		return right;
	if (right == nullptr)
		return left;

	if (left == right)
		return left;

	std::string abs_left = left->token;
	if (abs_left[0] == '~')
		abs_left = abs_left.substr(1);
	std::string abs_right = right->token;
	if (abs_right[0] == '~')
		abs_right = abs_right.substr(1);

	if (optimal_cnf) {
		std::stack<std::shared_ptr<Clause>> s;
		s.push(right);
		while (!s.empty()) {
			auto top = s.top();
			s.pop();
			if (top == nullptr)
				continue;
			s.push(top->left);
			s.push(top->right);

			if (abs_left == top->token && left->neg != top->neg) {
				return nullptr;
			}
		}

		for (; !s.empty(); s.pop())
			;

		s.push(left);
		while (!s.empty()) {
			auto top = s.top();
			s.pop();
			if (top == nullptr)
				continue;
			s.push(top->left);
			s.push(top->right);

			if (abs_right == top->token && right->neg != top->neg) {
				return nullptr;
			}
		}
	}

	cur_token = fmt::format("({})&({})", left->token, right->token);

	if (Clause::Clauses.find(cur_token) == Clause::Clauses.end()) {
		Clauses[cur_token]        = std::make_shared<Clause>();
		Clauses[cur_token]->token = cur_token;
		Clauses[cur_token]->left  = left;
		Clauses[cur_token]->right = right;
		Clauses[cur_token]->op    = "&";
	}

	return Clause::Clauses[cur_token];
}
}; // namespace ie
