#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "../common/list.hpp"
#include <stdexcept>

namespace karpenko {

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

  void push(T&& value)
  {
    list_.push_back(std::move(value));
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

  void clear() noexcept
  {
    list_.clear();
  }

  void swap(Queue& other) noexcept
  {
    list_.swap(other.list_);
  }

private:
  List<T> list_;
};

template<typename T>
void swap(Queue<T>& lhs, Queue<T>& rhs) noexcept
{
  lhs.swap(rhs);
}

}

#endif
