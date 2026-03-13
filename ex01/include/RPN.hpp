#pragma once
#include <iostream>
#include <stack>
#include <string>

class RPN
{
private:
	std::stack<int> _stack;

public:
	// default
	RPN();
	RPN(const RPN &other);
	RPN &operator=(const RPN &rhs);
	~RPN();

	// using
	int evaluate(const std::string &expression);

private:
	bool isDigitToken(const std::string &token) const;
	bool isOperatorToken(const std::string &token) const;
	void applyOperator(char op);
};
