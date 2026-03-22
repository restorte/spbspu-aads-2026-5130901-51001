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

private:
  List<T> list_;
};

}

int main()
{
  karpenko::Queue<int> q;
  q.push(10);
  q.push(20);
  std::cout << "Front: " << q.front() << std::endl;
  q.pop();
  std::cout << "Front after pop: " << q.front() << std::endl;
  return 0;
}
