#ifndef STACK_HPP
#define STACK_HPP

#include "../common/list.hpp"
#include <stdexcept>

namespace karpenko {

template<typename T>
class Stack {
public:
  Stack() = default;

  bool empty() const noexcept {
    return list_.empty();
  }

  size_t size() const noexcept {
    return list_.size();
  }

  void push(const T& value) {
    list_.push_front(value);
  }

  void push(T&& value) {
    list_.push_front(std::move(value));
  }

  void pop() {
    if (empty()) {
      throw std::out_of_range("Stack::pop(): empty stack");
    }
    list_.pop_front();
  }

  T& top() {
    if (empty()) {
      throw std::out_of_range("Stack::top(): empty stack");
    }
    return list_.front();
  }

  const T& top() const {
    if (empty()) {
      throw std::out_of_range("Stack::top(): empty stack");
    }
    return list_.front();
  }

  void swap(Stack& other) noexcept {
    list_.swap(other.list_);
  }

private:
  List<T> list_;
};

template<typename T>
void swap(Stack<T>& lhs, Stack<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}

#endif
