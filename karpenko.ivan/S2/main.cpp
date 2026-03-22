#include "../common/list.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <stdexcept>

namespace karpenko {

template<typename T>
class Stack
{
public:
  Stack() = default;
  Stack(const Stack&) = default;
  Stack(Stack&&) = default;
  Stack& operator=(const Stack&) = default;
  Stack& operator=(Stack&&) = default;

  bool empty() const noexcept
  {
    return list_.empty();
  }

  size_t size() const noexcept
  {
    return list_.size();
  }

  void push(const T& value)
  {
    list_.push_front(value);
  }

  void pop()
  {
    if (empty())
    {
      throw std::out_of_range("Stack::pop(): empty stack");
    }
    list_.pop_front();
  }

  T& top()
  {
    if (empty())
    {
      throw std::out_of_range("Stack::top(): empty stack");
    }
    return list_.front();
  }

  const T& top() const
  {
    if (empty())
    {
      throw std::out_of_range("Stack::top(): empty stack");
    }
    return list_.front();
  }

private:
  List<T> list_;
};

template<typename T>
class Queue
{
public:
  Queue() = default;
  Queue(const Queue&) = default;
  Queue(Queue&&) = default;
  Queue& operator=(const Queue&) = default;
  Queue& operator=(Queue&&) = default;

  bool empty() const noexcept
  {
    return list_.empty();
  }

  size_t size() const noexcept
  {
    return list_.size();
  }

  void push(const T& value)
  {
    list_.push_back(value);
  }

  void pop()
  {
    if (empty())
    {
      throw std::out_of_range("Queue::pop(): empty queue");
    }
    list_.pop_front();
  }

  T& front()
  {
    if (empty())
    {
      throw std::out_of_range("Queue::front(): empty queue");
    }
    return list_.front();
  }

  const T& front() const
  {
    if (empty())
    {
      throw std::out_of_range("Queue::front(): empty queue");
    }
    return list_.front();
  }

  T& back()
  {
    if (empty())
    {
      throw std::out_of_range("Queue::back(): empty queue");
    }
    return list_.back();
  }

  const T& back() const
  {
    if (empty())
    {
      throw std::out_of_range("Queue::back(): empty queue");
    }
    return list_.back();
  }

private:
  List<T> list_;
};

namespace evaluator {

int priority(char op)
{
  switch (op)
  {
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

bool isNumber(const std::string& s)
{
  if (s.empty())
  {
    return false;
  }
  for (char c : s)
  {
    if (!std::isdigit(static_cast<unsigned char>(c)))
    {
      return false;
    }
  }
  return true;
}

Queue<std::string> toPostfix(const std::string& line)
{
  std::istringstream iss(line);
  std::string token;
  Stack<char> ops;
  Queue<std::string> output;

  while (iss >> token)
  {
    if (isNumber(token))
    {
      output.push(token);
    }
    else if (token == "(")
    {
      ops.push('(');
    }
    else if (token == ")")
    {
      while (!ops.empty() && ops.top() != '(')
      {
        output.push(std::string(1, ops.top()));
        ops.pop();
      }
      if (ops.empty())
      {
        throw std::runtime_error("Mismatched parentheses");
      }
      ops.pop();
    }
    else if (token.size() == 1
             && (token[0] == '+' || token[0] == '-' || token[0] == '*'
                 || token[0] == '/' || token[0] == '%'))
    {
      char op = token[0];
      while (!ops.empty()
             && ops.top() != '('
             && priority(ops.top()) >= priority(op))
      {
        output.push(std::string(1, ops.top()));
        ops.pop();
      }
      ops.push(op);
    }
    else
    {
      throw std::runtime_error("Invalid token: " + token);
    }
  }

  while (!ops.empty())
  {
    if (ops.top() == '(')
    {
      throw std::runtime_error("Mismatched parentheses");
    }
    output.push(std::string(1, ops.top()));
    ops.pop();
  }

  return output;
}

long long evaluatePostfix(Queue<std::string>& postfix)
{
  Stack<long long> values;

  while (!postfix.empty())
  {
    std::string token = postfix.front();
    postfix.pop();

    if (isNumber(token))
    {
      values.push(std::stoll(token));
    }
    else if (token.size() == 1)
    {
      char op = token[0];
      if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
      {
        if (values.size() < 2)
        {
          throw std::runtime_error("Not enough operands for binary operator");
        }
        long long rhs = values.top();
        values.pop();
        long long lhs = values.top();
        values.pop();
        long long res = 0;

        switch (op)
        {
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
            if (rhs == 0)
            {
              throw std::runtime_error("Division by zero");
            }
            res = lhs / rhs;
            break;
          case '%':
            if (rhs == 0)
            {
              throw std::runtime_error("Modulo by zero");
            }
            res = lhs % rhs;
            break;
        }
        values.push(res);
      }
      else
      {
        throw std::runtime_error("Unknown operator");
      }
    }
    else
    {
      throw std::runtime_error("Invalid token in postfix");
    }
  }

  if (values.size() != 1)
  {
    throw std::runtime_error("Invalid expression: leftover values");
  }

  return values.top();
}

long long evaluate(const std::string& line)
{
  Queue<std::string> postfix = toPostfix(line);
  return evaluatePostfix(postfix);
}

}

}

int main(int argc, char* argv[])
{
  using namespace karpenko;

  std::istream* input = &std::cin;
  std::ifstream file;

  if (argc >= 2)
  {
    file.open(argv[1]);
    if (!file)
    {
      std::cerr << "Error: cannot open file " << argv[1] << std::endl;
      return 1;
    }
    input = &file;
  }

  Stack<long long> results;
  std::string line;

  while (std::getline(*input, line))
  {
    if (line.empty())
    {
      continue;
    }

    try
    {
      long long res = evaluator::evaluate(line);
      results.push(res);
    }
    catch (const std::exception& e)
    {
      std::cerr << "Error evaluating expression: " << e.what() << std::endl;
      return 1;
    }
  }

  if (results.empty())
  {
    std::cout << "0\n";
    return 0;
  }

  bool first = true;
  while (!results.empty())
  {
    if (!first)
    {
      std::cout << ' ';
    }
    std::cout << results.top();
    results.pop();
    first = false;
  }
  std::cout << '\n';

  return 0;
}
