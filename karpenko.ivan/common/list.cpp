#include "list.hpp"
#include <limits>
#include <stdexcept>
#include <iostream>

namespace karpenko
{
  template< typename T >
  List< T >::List() : head_(new detail::NodeBase), tail_(head_)
  {
    head_->next = head_;
  }

  template< typename T >
  List< T >::~List()
  {
    clear();
    delete head_;
  }

  template< typename T >
  List< T >::List(const List& other) : List()
  {
    try
    {
      for (const_iterator it = other.begin(); it != other.end(); ++it)
      {
        push_back(*it);
      }
    }
    catch (...)
    {
      clear();
      throw;
    }
  }

  template< typename T >
  List< T >::List(List&& other) noexcept : head_(other.head_), tail_(other.tail_)
  {
    other.head_ = new detail::NodeBase;
    other.head_->next = other.head_;
    other.tail_ = other.head_;
  }

  template< typename T >
  List< T >& List< T >::operator=(const List& other)
  {
    if (this != &other)
    {
      List tmp(other);
      swap(tmp);
    }
    return *this;
  }

  template< typename T >
  List< T >& List< T >::operator=(List&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      delete head_;
      head_ = other.head_;
      tail_ = other.tail_;
      other.head_ = new detail::NodeBase;
      other.head_->next = other.head_;
      other.tail_ = other.head_;
    }
    return *this;
  }

  template< typename T >
  typename List< T >::iterator List< T >::begin() noexcept
  {
    return iterator(head_->next);
  }

  template< typename T >
  typename List< T >::const_iterator List< T >::begin() const noexcept
  {
    return const_iterator(head_->next);
  }

  template< typename T >
  typename List< T >::iterator List< T >::end() noexcept
  {
    return iterator(head_);
  }

  template< typename T >
  typename List< T >::const_iterator List< T >::end() const noexcept
  {
    return const_iterator(head_);
  }

  template< typename T >
  typename List< T >::const_iterator List< T >::cbegin() const noexcept
  {
    return const_iterator(head_->next);
  }

  template< typename T >
  typename List< T >::const_iterator List< T >::cend() const noexcept
  {
    return const_iterator(head_);
  }

  template< typename T >
  bool List< T >::empty() const noexcept
  {
    return head_->next == head_;
  }

  template< typename T >
  T& List< T >::front()
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    return *begin();
  }

  template< typename T >
  const T& List< T >::front() const
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    return *begin();
  }

  template< typename T >
  T& List< T >::back()
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    return *iterator(tail_);
  }

  template< typename T >
  const T& List< T >::back() const
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    return *const_iterator(tail_);
  }

  template< typename T >
  void List< T >::push_front(const T& value)
  {
    insert_after(iterator(head_), value);
  }

  template< typename T >
  void List< T >::push_front(T&& value)
  {
    insert_after(iterator(head_), std::move(value));
  }

  template< typename T >
  void List< T >::push_back(const T& value)
  {
    if (empty())
    {
      push_front(value);
    }
    else
    {
      insert_after(iterator(tail_), value);
    }
  }

  template< typename T >
  void List< T >::push_back(T&& value)
  {
    if (empty())
    {
      push_front(std::move(value));
    }
    else
    {
      insert_after(iterator(tail_), std::move(value));
    }
  }

  template< typename T >
  void List< T >::pop_front()
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    detail::NodeBase* old = head_->next;
    head_->next = old->next;
    if (tail_ == old)
    {
      tail_ = head_;
    }
    detail::Node< T >* node_to_delete = static_cast< detail::Node< T >* >(old);
    delete node_to_delete;
  }

  template< typename T >
  void List< T >::pop_back()
  {
    if (empty())
    {
      throw std::out_of_range("List is empty");
    }
    if (head_->next == tail_)
    {
      pop_front();
      return;
    }
    detail::NodeBase* prev = head_->next;
    while (prev->next != tail_)
    {
      prev = prev->next;
    }
    detail::Node< T >* node_to_delete = static_cast< detail::Node< T >* >(tail_);
    delete node_to_delete;
    tail_ = prev;
    tail_->next = head_;
  }

  template< typename T >
  typename List< T >::iterator List< T >::insert_after(iterator pos, const T& value)
  {
    detail::Node< T >* new_node = new detail::Node< T >(pos.get_ptr()->next, value);
    pos.get_ptr()->next = new_node;
    if (tail_ == pos.get_ptr())
    {
      tail_ = new_node;
    }
    return iterator(new_node);
  }

  template< typename T >
  typename List< T >::iterator List< T >::insert_after(iterator pos, T&& value)
  {
    detail::Node< T >* new_node = new detail::Node< T >(pos.get_ptr()->next, std::move(value));
    pos.get_ptr()->next = new_node;
    if (tail_ == pos.get_ptr())
    {
      tail_ = new_node;
    }
    return iterator(new_node);
  }

  template< typename T >
  typename List< T >::iterator List< T >::erase_after(iterator pos)
  {
    if (pos.get_ptr() == head_ || pos.get_ptr()->next == head_)
    {
      throw std::out_of_range("Invalid position for erase_after");
    }
    detail::NodeBase* to_delete = pos.get_ptr()->next;
    pos.get_ptr()->next = to_delete->next;
    if (tail_ == to_delete)
    {
      tail_ = pos.get_ptr();
    }
    detail::Node< T >* node_to_delete = static_cast< detail::Node< T >* >(to_delete);
    delete node_to_delete;
    return iterator(pos.get_ptr()->next);
  }

  template< typename T >
  void List< T >::clear() noexcept
  {
    while (!empty())
    {
      pop_front();
    }
  }

  template< typename T >
  void List< T >::swap(List& other) noexcept
  {
    detail::NodeBase* temp_head = head_;
    detail::NodeBase* temp_tail = tail_;
    head_ = other.head_;
    tail_ = other.tail_;
    other.head_ = temp_head;
    other.tail_ = temp_tail;
  }

  template< typename T >
  typename List< T >::size_type List< T >::size() const noexcept
  {
    size_type count = 0;
    for (const_iterator it = begin(); it != end(); ++it)
    {
      ++count;
    }
    return count;
  }

  template< typename T >
  void swap(List< T >& lhs, List< T >& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  template class List< int >;
  template class List< size_t >;
  template class List< std::pair< std::string, List< size_t > > >;
  template class List< List< size_t > >;
}
