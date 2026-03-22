#include "../common/list.hpp"
#include <iostream>
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

}

int main()
{
  karpenko::Stack<int> s;
  s.push(10);
  s.push(20);
  std::cout << "Top: " << s.top() << std::endl;
  s.pop();
  std::cout << "Top after pop: " << s.top() << std::endl;
  return 0;
}
