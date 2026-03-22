#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "stack.hpp"
#include "queue.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

namespace karpenko {
namespace evaluator {

int priority(char op) {
  switch (op) {
    case '|':
      return 0;
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
    case '%':
      return 2;
    default:
      return 0;
  }
}

bool isNumber(const std::string& s) {
  if (s.empty()) {
    return false;
  }
  for (char c : s) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
  }
  return true;
}

Queue<std::string> toPostfix(const std::string& line) {
  std::istringstream iss(line);
  std::string token;
  Stack<char> ops;
  Queue<std::string> output;

  while (iss >> token) {
    if (isNumber(token)) {
      output.push(token);
    } else if (token == "(") {
      ops.push('(');
    } else if (token == ")") {
      while (!ops.empty() && ops.top() != '(') {
        output.push(std::string(1, ops.top()));
        ops.pop();
      }
      if (ops.empty()) {
        throw std::runtime_error("Mismatched parentheses");
      }
      ops.pop();
    } else if (token.size() == 1 &&
               (token[0] == '+' || token[0] == '-' || token[0] == '*' ||
                token[0] == '/' || token[0] == '%' || token[0] == '|')) {
      char op = token[0];
      while (!ops.empty() && ops.top() != '(' &&
             priority(ops.top()) >= priority(op)) {
        output.push(std::string(1, ops.top()));
        ops.pop();
      }
      ops.push(op);
    } else {
      throw std::runtime_error("Invalid token: " + token);
    }
  }

  while (!ops.empty()) {
    if (ops.top() == '(') {
      throw std::runtime_error("Mismatched parentheses");
    }
    output.push(std::string(1, ops.top()));
    ops.pop();
  }

  return output;
}

long long evaluatePostfix(Queue<std::string>& postfix) {
  Stack<long long> values;

  while (!postfix.empty()) {
    std::string token = postfix.front();
    postfix.pop();

    if (isNumber(token)) {
      values.push(std::stoll(token));
    } else if (token.size() == 1) {
      char op = token[0];

      if (op == '+' || op == '-' || op == '*' || op == '/' ||
          op == '%' || op == '|') {
        if (values.size() < 2) {
          throw std::runtime_error("Not enough operands for binary operator");
        }
        long long rhs = values.top();
        values.pop();
        long long lhs = values.top();
        values.pop();
        long long res = 0;

        switch (op) {
          case '+':
            res = lhs + rhs;
            break;
          case '-':
            res = lhs - rhs;
            break;
          case '*':
            res = lhs * rhs;
            break;
          case '/':
            if (rhs == 0) {
              throw std::runtime_error("Division by zero");
            }
            res = lhs / rhs;
            break;
          case '%':
            if (rhs == 0) {
              throw std::runtime_error("Modulo by zero");
            }
            res = lhs % rhs;
            break;
          case '|':
            res = lhs | rhs;
            break;
          default:
            throw std::runtime_error("Unknown operator");
        }
        values.push(res);
      } else {
        throw std::runtime_error("Unknown operator");
      }
    } else {
      throw std::runtime_error("Invalid token in postfix");
    }
  }

  if (values.size() != 1) {
    throw std::runtime_error("Invalid expression: leftover values");
  }

  return values.top();
}

long long evaluate(const std::string& line) {
  Queue<std::string> postfix = toPostfix(line);
  return evaluatePostfix(postfix);
}

}
}

#endif
