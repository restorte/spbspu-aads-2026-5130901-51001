#include "../common/list.hpp"
#include <iostream>
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

}

}

int main()
{
  using namespace karpenko;

  std::string line = "1 + 2 * 3";
  try
  {
    Queue<std::string> postfix = evaluator::toPostfix(line);
    while (!postfix.empty())
    {
      std::cout << postfix.front() << " ";
      postfix.pop();
    }
    std::cout << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
