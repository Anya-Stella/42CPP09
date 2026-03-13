#include "../include/RPN.hpp"
#include <sstream>
#include <stdexcept>

// default
RPN::RPN() {};
RPN::RPN(const RPN &other) : _stack(other._stack) {}
RPN &RPN::operator=(const RPN &rhs)
{
	if (this != &rhs)
		_stack = rhs._stack;
	return *this;
}
RPN::~RPN() {}

// helper function
bool RPN::isDigitToken(const std::string &token) const
{
	return token.length() == 1 && ('0' <= token[0] && token[0] <= '9');
}

bool RPN::isOperatorToken(const std::string &token) const
{
	return token == "+" || token == "-" || token == "*" || token == "/";
}

void	RPN::applyOperator(char op)// +
{
	// opの種類によってスタックから取り出し計算しスタックへ戻す
	if (_stack.size() < 2)
		throw std::runtime_error("Error: Not enough operands for operator");
	
	int right = _stack.top(); _stack.pop();
	int left = _stack.top(); _stack.pop();

	int result;
	switch (op)
	{
		case '+': result = left + right; break;
		case '-': result = left - right; break;
		case '*': result = left * right; break;
		case '/':
			if (right == 0)
				throw std::runtime_error("Error: Division by zero");
			result = left / right; break;
		default:
			throw std::invalid_argument("Error: Invalid operator");
	}
	_stack.push(result);
}

// using
int RPN::evaluate(const std::string &expression)
{
	while (!_stack.empty())
		_stack.pop();

	std::istringstream iss(expression);
	std::string token;

	while (iss >> token)
	{
		if (isDigitToken(token))
			_stack.push(token[0] - '0');
		else if (isOperatorToken(token))
			applyOperator(token[0]);
		else
			throw std::invalid_argument("Error: Invalid token in expression");
	}
	if (_stack.size() != 1)
		throw std::runtime_error("Error: Invalid RPN expression");
	
	int result = _stack.top();
	_stack.pop();
	return result;
}
