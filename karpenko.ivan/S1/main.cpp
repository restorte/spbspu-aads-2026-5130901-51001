#include <iostream>
#include <string>
#include <utility>
#include <cassert>
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace karpenko
{
  const size_t MAX = std::numeric_limits< size_t >::max();

  namespace detail
  {
    struct NodeBase
    {
      NodeBase* next;
      explicit NodeBase(NodeBase* nxt = nullptr) noexcept : next(nxt) {}
    };

    template< typename T >
    struct Node final : NodeBase
    {
      T data;
      explicit Node(NodeBase* nxt, const T& value) : NodeBase(nxt), data(value) {}
      explicit Node(NodeBase* nxt, T&& value) : NodeBase(nxt), data(std::move(value)) {}
    };
  }

  template< typename T > class List;
  template< typename T > class LIter;
  template< typename T > class LCIter;

  template< typename T >
  class LIter
  {
  public:
    LIter() noexcept : ptr_(nullptr) {}
    explicit LIter(detail::NodeBase* p) noexcept : ptr_(p) {}
    T& operator*() const noexcept
    {
      detail::Node< T >* node_ptr = static_cast< detail::Node< T >* >(ptr_);
      return node_ptr->data;
    }
    T* operator->() const noexcept
    {
      detail::Node< T >* node_ptr = static_cast< detail::Node< T >* >(ptr_);
      return &(node_ptr->data);
    }
    LIter& operator++() noexcept
    {
      ptr_ = ptr_->next;
      return *this;
    }
    LIter operator++(int) noexcept
    {
      LIter tmp = *this;
      ptr_ = ptr_->next;
      return tmp;
    }
    bool operator==(const LIter& other) const noexcept
    {
      return ptr_ == other.ptr_;
    }
    bool operator!=(const LIter& other) const noexcept
    {
      return !(*this == other);
    }
    detail::NodeBase* get_ptr() const noexcept
    {
      return ptr_;
    }
  private:
    detail::NodeBase* ptr_;
    friend class List< T >;
    friend class LCIter< T >;
  };

  template< typename T >
  class LCIter
  {
  public:
    LCIter() noexcept : ptr_(nullptr) {}
    explicit LCIter(const detail::NodeBase* p) noexcept : ptr_(p) {}
    LCIter(const LIter< T >& it) noexcept : ptr_(it.get_ptr()) {}
    const T& operator*() const noexcept
    {
      const detail::Node< T >* node_ptr = static_cast< const detail::Node< T >* >(ptr_);
      return node_ptr->data;
    }
    const T* operator->() const noexcept
    {
      const detail::Node< T >* node_ptr = static_cast< const detail::Node< T >* >(ptr_);
      return &(node_ptr->data);
    }
    LCIter& operator++() noexcept
    {
      ptr_ = ptr_->next;
      return *this;
    }
    LCIter operator++(int) noexcept
    {
      LCIter tmp = *this;
      ptr_ = ptr_->next;
      return tmp;
    }
    bool operator==(const LCIter& other) const noexcept
    {
      return ptr_ == other.ptr_;
    }
    bool operator!=(const LCIter& other) const noexcept
    {
      return !(*this == other);
    }
  private:
    const detail::NodeBase* ptr_;
    friend class List< T >;
  };

  template< typename T >
  class List
  {
  public:
    using iterator = LIter< T >;
    using const_iterator = LCIter< T >;
    using size_type = std::size_t;

    List() : head_(new detail::NodeBase), tail_(head_)
    {
      head_->next = head_;
    }

    ~List()
    {
      clear();
      delete head_;
    }

    List(const List& other) : List()
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

    List(List&& other) noexcept : head_(other.head_), tail_(other.tail_)
    {
      other.head_ = new detail::NodeBase;
      other.head_->next = other.head_;
      other.tail_ = other.head_;
    }

    List& operator=(const List& other)
    {
      if (this != &other)
      {
        List tmp(other);
        swap(tmp);
      }
      return *this;
    }

    List& operator=(List&& other) noexcept
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

    iterator begin() noexcept
    {
      return iterator(head_->next);
    }
    const_iterator begin() const noexcept
    {
      return const_iterator(head_->next);
    }
    iterator end() noexcept
    {
      return iterator(head_);
    }
    const_iterator end() const noexcept
    {
      return const_iterator(head_);
    }

    const_iterator cbegin() const noexcept
    {
      return const_iterator(head_->next);
    }
    const_iterator cend() const noexcept
    {
      return const_iterator(head_);
    }

    bool empty() const noexcept
    {
      return head_->next == head_;
    }

    T& front()
    {
      assert(!empty());
      return *begin();
    }

    const T& front() const
    {
      assert(!empty());
      return *begin();
    }

    T& back()
    {
      assert(!empty());
      return *iterator(tail_);
    }

    const T& back() const
    {
      assert(!empty());
      return *const_iterator(tail_);
    }

    void push_front(const T& value)
    {
      insert_after(begin(), value);
    }

    void push_front(T&& value)
    {
      insert_after(begin(), std::move(value));
    }

    void push_back(const T& value)
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

    void push_back(T&& value)
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

    void pop_front()
    {
      assert(!empty());
      detail::NodeBase* old = head_->next;
      head_->next = old->next;
      if (tail_ == old)
      {
        tail_ = head_;
      }
      detail::Node< T >* node_to_delete = static_cast< detail::Node< T >* >(old);
      delete node_to_delete;
    }

    void pop_back()
    {
      assert(!empty());
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

    iterator insert_after(iterator pos, const T& value)
    {
      detail::Node< T >* new_node = new detail::Node< T >(pos.get_ptr()->next, value);
      pos.get_ptr()->next = new_node;
      if (tail_ == pos.get_ptr())
      {
        tail_ = new_node;
      }
      return iterator(new_node);
    }

    iterator insert_after(iterator pos, T&& value)
    {
      detail::Node< T >* new_node = new detail::Node< T >(pos.get_ptr()->next, std::move(value));
      pos.get_ptr()->next = new_node;
      if (tail_ == pos.get_ptr())
      {
        tail_ = new_node;
      }
      return iterator(new_node);
    }

    iterator erase_after(iterator pos)
    {
      assert(pos.get_ptr() != head_);
      assert(pos.get_ptr()->next != head_);
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

    void clear() noexcept
    {
      while (!empty())
      {
        pop_front();
      }
    }

    void swap(List& other) noexcept
    {
      detail::NodeBase* temp_head = head_;
      detail::NodeBase* temp_tail = tail_;
      head_ = other.head_;
      tail_ = other.tail_;
      other.head_ = temp_head;
      other.tail_ = temp_tail;
    }

    size_type size() const noexcept
    {
      size_type count = 0;
      for (const_iterator it = begin(); it != end(); ++it)
      {
        ++count;
      }
      return count;
    }

  private:
    detail::NodeBase* head_;
    detail::NodeBase* tail_;
  };

  template< typename T >
  void swap(List< T >& lhs, List< T >& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  bool read_sequence(
    std::istream& in,
    std::string& name,
    List< size_t >& numbers)
  {
    if (!(in >> name))
    {
      return false;
    }
    numbers.clear();
    while (in.peek() != '\n' && in.peek() != EOF)
    {
      size_t value = 0;
      if (in >> value)
      {
        numbers.push_back(value);
      }
      else
      {
        in.clear();
        in.ignore();
        break;
      }
    }
    in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    return true;
  }
}

int main()
{
  using namespace karpenko;

  List< std::pair< std::string, List< size_t > > > sequences;

  while (true)
  {
    std::string name;
    List< size_t > numbers;
    if (!read_sequence(std::cin, name, numbers))
    {
      break;
    }
    sequences.push_back(std::make_pair(name, std::move(numbers)));
  }

  if (sequences.empty())
  {
    std::cout << "0\n";
    return 0;
  }

  bool first = true;
  for (List< std::pair< std::string, List< size_t > > >::iterator it = sequences.begin(); 
       it != sequences.end(); ++it)
  {
    if (!first)
    {
      std::cout << ' ';
    }
    std::cout << it->first;
    first = false;
  }
  std::cout << '\n';

  size_t max_len = 0;
  for (List< std::pair< std::string, List< size_t > > >::const_iterator it = sequences.cbegin(); 
       it != sequences.cend(); ++it)
  {
    size_t len = it->second.size();
    if (len > max_len)
    {
      max_len = len;
    }
  }

  List< List< size_t > > transposed;
  bool has_numbers = false;

  for (size_t pos = 0; pos < max_len; ++pos)
  {
    List< size_t > new_seq;
    
    for (List< std::pair< std::string, List< size_t > > >::const_iterator seq_it = sequences.cbegin(); 
         seq_it != sequences.cend(); ++seq_it)
    {
      List< size_t >::const_iterator num_it = seq_it->second.begin();
      size_t curr = 0;
      
      while (curr < pos && num_it != seq_it->second.end())
      {
        ++curr;
        ++num_it;
      }
      
      if (num_it != seq_it->second.end())
      {
        new_seq.push_back(*num_it);
        has_numbers = true;
      }
    }
    
    if (!new_seq.empty())
    {
      transposed.push_back(std::move(new_seq));
    }
  }

  if (!has_numbers)
  {
    std::cout << "0\n";
    return 0;
  }

  for (List< List< size_t > >::iterator tit = transposed.begin(); 
       tit != transposed.end(); ++tit)
  {
    bool first_in_row = true;
    for (List< size_t >::const_iterator nit = tit->begin(); 
         nit != tit->end(); ++nit)
    {
      if (!first_in_row)
      {
        std::cout << ' ';
      }
      std::cout << *nit;
      first_in_row = false;
    }
    std::cout << '\n';
  }

  List< size_t > sums;
  
  for (List< List< size_t > >::const_iterator tit = transposed.cbegin(); 
       tit != transposed.cend(); ++tit)
  {
    size_t sum = 0;
    
    try
    {
      for (List< size_t >::const_iterator nit = tit->begin(); 
           nit != tit->end(); ++nit)
      {
        if (sum > MAX - *nit)
        {
          throw std::overflow_error("Sum overflow");
        }
        sum += *nit;
      }
    }
    catch (const std::overflow_error& e)
    {
      std::cerr << "Formed lists with exit code 1 and error message in standard error because of overflow\n";
      return 1;
    }
    
    sums.push_back(sum);
  }

  first = true;
  for (List< size_t >::const_iterator sit = sums.cbegin(); 
       sit != sums.cend(); ++sit)
  {
    if (!first)
    {
      std::cout << ' ';
    }
    std::cout << *sit;
    first = false;
  }
  std::cout << '\n';

  return 0;
}
