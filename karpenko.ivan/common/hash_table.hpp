#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/hash2/blake2.hpp>

namespace karpenko
{

struct Blake2Hash
{
  std::size_t operator()(const std::string& s) const
  {
    return hash_bytes(s.data(), s.size());
  }

  std::size_t hash_bytes(const char* data, std::size_t len) const
  {
    boost::hash2::blake2b_512 hasher;
    hasher.update(data, len);
    auto digest = hasher.result();
    std::size_t result = 0;
    std::memcpy(&result, digest.data(), std::min(sizeof(result), digest.size()));
    return result;
  }
};

template <typename Key, typename Value,
          typename Hash = Blake2Hash,
          typename Equal = std::equal_to<Key>>
class BucketHashTable
{
public:
  using value_type = std::pair<Key, Value>;

private:
  struct Slot
  {
    enum State { EMPTY, OCCUPIED, TOMBSTONE };
    value_type data;
    State state;
    Slot() : state(EMPTY) {}
    ~Slot() = default;

    Slot(const Slot& other) : data(other.data), state(other.state) {}
    Slot& operator=(const Slot& other)
    {
      if (this != &other) { data = other.data; state = other.state; }
      return *this;
    }

    Slot(Slot&& other) noexcept
      : data(std::move(other.data)), state(other.state)
    {
      other.state = EMPTY;
    }
    Slot& operator=(Slot&& other) noexcept
    {
      if (this != &other) { data = std::move(other.data); state = other.state; other.state = EMPTY; }
      return *this;
    }
  };

  Slot* slots_;
  std::size_t bucket_count_;
  std::size_t bucket_size_;
  std::size_t total_slots_;
  std::size_t overflow_start_;
  std::size_t overflow_size_;
  std::size_t size_;
  Hash hasher_;
  Equal equal_;

  std::size_t home_bucket(const Key& k) const { return hasher_(k) % bucket_count_; }
  std::size_t bucket_offset(std::size_t bucket) const { return bucket * bucket_size_; }

  Slot* find_slot_any(const Key& key)
  {
    std::size_t b = home_bucket(key);
    Slot* s = find_in_bucket(key, b);
    return s ? s : find_in_overflow(key);
  }

  const Slot* find_slot_any(const Key& key) const
  {
    std::size_t b = home_bucket(key);
    const Slot* s = find_in_bucket(key, b);
    return s ? s : find_in_overflow(key);
  }

  Slot* find_slot(const Key& key)
  {
    Slot* s = find_slot_any(key);
    return (s && s->state == Slot::OCCUPIED) ? s : nullptr;
  }

  const Slot* find_slot(const Key& key) const
  {
    const Slot* s = find_slot_any(key);
    return (s && s->state == Slot::OCCUPIED) ? s : nullptr;
  }

  Slot* find_in_bucket(const Key& key, std::size_t bucket)
  {
    std::size_t start = bucket_offset(bucket);
    for (std::size_t i = 0; i < bucket_size_; ++i)
    {
      Slot& s = slots_[start + i];
      if (s.state == Slot::EMPTY) break;
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE) && equal_(s.data.first, key))
        return &s;
    }
    return nullptr;
  }

  const Slot* find_in_bucket(const Key& key, std::size_t bucket) const
  {
    std::size_t start = bucket_offset(bucket);
    for (std::size_t i = 0; i < bucket_size_; ++i)
    {
      const Slot& s = slots_[start + i];
      if (s.state == Slot::EMPTY) break;
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE) && equal_(s.data.first, key))
        return &s;
    }
    return nullptr;
  }

  Slot* find_empty_in_bucket(std::size_t bucket)
  {
    std::size_t start = bucket_offset(bucket);
    for (std::size_t i = 0; i < bucket_size_; ++i)
      if (slots_[start + i].state == Slot::EMPTY) return &slots_[start + i];
    return nullptr;
  }

  Slot* find_in_overflow(const Key& key)
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
    {
      Slot& s = slots_[overflow_start_ + i];
      if (s.state == Slot::EMPTY) break;
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE) && equal_(s.data.first, key))
        return &s;
    }
    return nullptr;
  }

  const Slot* find_in_overflow(const Key& key) const
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
    {
      const Slot& s = slots_[overflow_start_ + i];
      if (s.state == Slot::EMPTY) break;
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE) && equal_(s.data.first, key))
        return &s;
    }
    return nullptr;
  }

  Slot* find_empty_in_overflow()
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
      if (slots_[overflow_start_ + i].state == Slot::EMPTY) return &slots_[overflow_start_ + i];
    return nullptr;
  }

public:
  BucketHashTable(std::size_t bucket_count = 16, std::size_t bucket_size = 4, std::size_t overflow_size = 16,
                  Hash h = Hash(), Equal e = Equal())
    : slots_(nullptr), bucket_count_(bucket_count ? bucket_count : 1),
      bucket_size_(bucket_size ? bucket_size : 1),
      total_slots_(bucket_count_ * bucket_size_ + (overflow_size ? overflow_size : 1)),
      overflow_start_(bucket_count_ * bucket_size_),
      overflow_size_(overflow_size ? overflow_size : 1),
      size_(0), hasher_(h), equal_(e)
  {
    slots_ = static_cast<Slot*>(::operator new(sizeof(Slot) * total_slots_));
    for (std::size_t i = 0; i < total_slots_; ++i) new (slots_ + i) Slot();
  }

  ~BucketHashTable()
  {
    if (slots_)
    {
      for (std::size_t i = 0; i < total_slots_; ++i) slots_[i].~Slot();
      ::operator delete(slots_);
    }
  }

  BucketHashTable(const BucketHashTable& other)
    : bucket_count_(other.bucket_count_), bucket_size_(other.bucket_size_),
      total_slots_(other.total_slots_), overflow_start_(other.overflow_start_),
      overflow_size_(other.overflow_size_), size_(other.size_),
      hasher_(other.hasher_), equal_(other.equal_)
  {
    slots_ = static_cast<Slot*>(::operator new(sizeof(Slot) * total_slots_));
    for (std::size_t i = 0; i < total_slots_; ++i) new (slots_ + i) Slot(other.slots_[i]);
  }

  BucketHashTable(BucketHashTable&& other) noexcept
    : slots_(other.slots_), bucket_count_(other.bucket_count_), bucket_size_(other.bucket_size_),
      total_slots_(other.total_slots_), overflow_start_(other.overflow_start_),
      overflow_size_(other.overflow_size_), size_(other.size_),
      hasher_(std::move(other.hasher_)), equal_(std::move(other.equal_))
  {
    other.slots_ = nullptr;
    other.size_ = 0;
  }

  void swap(BucketHashTable& other) noexcept
  {
    std::swap(slots_, other.slots_);
    std::swap(bucket_count_, other.bucket_count_);
    std::swap(bucket_size_, other.bucket_size_);
    std::swap(total_slots_, other.total_slots_);
    std::swap(overflow_start_, other.overflow_start_);
    std::swap(overflow_size_, other.overflow_size_);
    std::swap(size_, other.size_);
    std::swap(hasher_, other.hasher_);
    std::swap(equal_, other.equal_);
  }

  BucketHashTable& operator=(BucketHashTable other) { swap(other); return *this; }

  class iterator
  {
    friend class BucketHashTable;
    Slot* ptr_;
    Slot* end_;
    void skip_empty() { while (ptr_ != end_ && ptr_->state != Slot::OCCUPIED) ++ptr_; }
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename BucketHashTable::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::forward_iterator_tag;

    iterator() : ptr_(nullptr), end_(nullptr) {}
    iterator(Slot* p, Slot* e) : ptr_(p), end_(e) { skip_empty(); }
    reference operator*() { return ptr_->data; }
    pointer operator->() { return &ptr_->data; }
    iterator& operator++() { ++ptr_; skip_empty(); return *this; }
    iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
    bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const iterator& other) const { return !(*this == other); }
  };

  class const_iterator
  {
    friend class BucketHashTable;
    const Slot* ptr_;
    const Slot* end_;
    void skip_empty() { while (ptr_ != end_ && ptr_->state != Slot::OCCUPIED) ++ptr_; }
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = const typename BucketHashTable::value_type;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    const_iterator() : ptr_(nullptr), end_(nullptr) {}
    const_iterator(const Slot* p, const Slot* e) : ptr_(p), end_(e) { skip_empty(); }
    reference operator*() const { return ptr_->data; }
    pointer operator->() const { return &ptr_->data; }
    const_iterator& operator++() { ++ptr_; skip_empty(); return *this; }
    const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
    bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const const_iterator& other) const { return !(*this == other); }
  };

  iterator begin() { return iterator(slots_, slots_ + total_slots_); }
  iterator end()   { return iterator(slots_ + total_slots_, slots_ + total_slots_); }
  const_iterator begin() const { return const_iterator(slots_, slots_ + total_slots_); }
  const_iterator end()   const { return const_iterator(slots_ + total_slots_, slots_ + total_slots_); }

  bool has(const Key& key) const { return find_slot(key) != nullptr; }

  void add(const Key& key, const Value& val)
  {
    Slot* exist = find_slot_any(key);
    if (exist && exist->state == Slot::OCCUPIED)
    {
      exist->data.second = val;
      return;
    }
    if (exist && exist->state == Slot::TOMBSTONE)
    {
      exist->data.~value_type();
      new (&exist->data) value_type(key, val);
      exist->state = Slot::OCCUPIED;
      ++size_;
      return;
    }
    std::size_t b = home_bucket(key);
    Slot* dest = find_empty_in_bucket(b);
    if (!dest) dest = find_empty_in_overflow();
    if (!dest) throw std::runtime_error("Hash table overflow");
    dest->data.~value_type();
    new (&dest->data) value_type(key, val);
    dest->state = Slot::OCCUPIED;
    ++size_;
  }

  bool remove(const Key& key)
  {
    Slot* s = find_slot_any(key);
    if (s && s->state == Slot::OCCUPIED)
    {
      s->state = Slot::TOMBSTONE;
      --size_;
      return true;
    }
    return false;
  }

  iterator find(const Key& key)
  {
    Slot* s = find_slot(key);
    return s ? iterator(s, slots_ + total_slots_) : end();
  }

  const_iterator find(const Key& key) const
  {
    const Slot* s = find_slot(key);
    return s ? const_iterator(s, slots_ + total_slots_) : end();
  }

  void rehash(std::size_t new_bucket_count, std::size_t new_bucket_size, std::size_t new_overflow_size)
  {
    BucketHashTable tmp(new_bucket_count, new_bucket_size, new_overflow_size, hasher_, equal_);
    for (auto it = begin(); it != end(); ++it) tmp.add(it->first, it->second);
    swap(tmp);
  }

  std::size_t size() const { return size_; }
  std::size_t bucket_count() const { return bucket_count_; }
};

}

#endif
