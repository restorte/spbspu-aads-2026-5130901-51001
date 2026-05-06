#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/hash2/blake2.hpp>

namespace karpenko
{

template <class T>
struct Vector
{
  Vector();
  Vector(size_t s, const T& val);
  ~Vector();

  Vector(std::initializer_list<T> il);

  Vector(Vector<T>&&) noexcept;
  Vector<T>& operator=(Vector<T>&&) noexcept;
  void pushFront(const T& val);
  void swap(Vector<T>&) noexcept;
  Vector(const Vector<T>&);
  Vector<T>& operator=(const Vector<T>&);
  bool isEmpty() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;
  void pushBack(const T& val);

  void reserve(size_t k);
  void shrinkToFit();
  void repeatPushBack(const T& val, size_t k);
  template <class IT>
  void rangedPushBack(IT beg, size_t count);
  void repeatInsert(size_t id, const T& val, size_t k);

  bool operator==(const Vector<T>& rhs) const noexcept;
  bool operator!=(const Vector<T>& rhs) const noexcept;

  void insert(size_t pos, const T& val);
  void insert(size_t pos, const Vector<T>& rhs, size_t b, size_t e);
  void erase(size_t pos);

  struct VectorIterator;
  using iterator = VectorIterator;

  iterator begin();
  iterator end();

  void insert(iterator pos, const T& val);
  void erase(iterator pos);

  template <class IT>
  void insert(iterator pos, IT begin, IT end);

  void erase(iterator first, iterator last);
  void eraseValue(const T& value);

  T& operator[](size_t id) noexcept;
  const T& operator[](size_t id) const noexcept;
  T& at(size_t id);
  const T& at(size_t id) const;

private:
  T* data_;
  size_t size_;
  size_t capacity_;
  explicit Vector(size_t s);

  void allocate(size_t new_capacity);
  void deallocate();
  void destroyRange(size_t start, size_t end);
  void constructRange(size_t start, size_t end, const T& val);
  template <class IT>
  void constructFromRange(size_t start, IT begin, IT end);
};

template <class T>
void Vector<T>::constructRange(size_t start, size_t end, const T& val)
{
  for (size_t i = start; i < end; ++i)
  {
    new (data_ + i) T(val);
  }
}

template <class T>
template <class IT>
void Vector<T>::constructFromRange(size_t start, IT begin, IT end)
{
  size_t i = start;
  for (IT it = begin; it != end; ++it, ++i)
  {
    new (data_ + i) T(*it);
  }
}

template <class T>
void Vector<T>::destroyRange(size_t start, size_t end)
{
  for (size_t i = start; i < end; ++i)
  {
    data_[i].~T();
  }
}

template <class T>
void Vector<T>::deallocate()
{
  if (data_ != nullptr)
  {
    ::operator delete(data_);
  }
}

template <class T>
void Vector<T>::allocate(size_t new_capacity)
{
  if (new_capacity == 0)
  {
    data_ = nullptr;
    return;
  }
  data_ = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
  capacity_ = new_capacity;
}

template <class T>
void Vector<T>::shrinkToFit()
{
  if (size_ == capacity_)
  {
    return;
  }
  if (size_ == 0)
  {
    destroyRange(0, size_);
    deallocate();
    data_ = nullptr;
    capacity_ = 0;
    return;
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * size_));
  for (size_t i = 0; i < size_; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  capacity_ = size_;
}

template <class T>
template <class IT>
void Vector<T>::rangedPushBack(IT beg, size_t count)
{
  if (count == 0)
  {
    return;
  }
  reserve(size_ + count);
  for (size_t i = 0; i < count; ++i)
  {
    new (data_ + size_ + i) T(*beg);
    ++beg;
  }
  size_ += count;
}

template <class T>
void Vector<T>::reserve(size_t k)
{
  if (k <= capacity_)
  {
    return;
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * k));
  for (size_t i = 0; i < size_; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  capacity_ = k;
}

template <class T>
void Vector<T>::repeatInsert(size_t id, const T& val, size_t k)
{
  if (id > size_)
  {
    throw std::out_of_range("Vector::repeatInsert: position out of range");
  }
  if (k == 0)
  {
    return;
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * (size_ + k)));
  for (size_t i = 0; i < id; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  for (size_t i = 0; i < k; ++i)
  {
    new (newData + id + i) T(val);
  }
  for (size_t i = id; i < size_; ++i)
  {
    new (newData + i + k) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  size_ += k;
  capacity_ = size_ + k;
}

template <class T>
Vector<T>::Vector(Vector<T>&& rhs) noexcept
  : data_(rhs.data_),
    size_(rhs.size_),
    capacity_(rhs.capacity_)
{
  rhs.data_ = nullptr;
  rhs.size_ = 0;
  rhs.capacity_ = 0;
}

template <class T>
void Vector<T>::repeatPushBack(const T& val, size_t k)
{
  if (k == 0)
  {
    return;
  }
  reserve(size_ + k);
  for (size_t i = 0; i < k; ++i)
  {
    new (data_ + size_ + i) T(val);
  }
  size_ += k;
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> il)
  : data_(nullptr),
    size_(0),
    capacity_(0)
{
  reserve(il.size());
  size_ = il.size();
  size_t i = 0;
  for (auto it = il.begin(); it != il.end(); ++it, ++i)
  {
    new (data_ + i) T(*it);
  }
}

template <class T>
void Vector<T>::pushFront(const T& val)
{
  insert(0, val);
}

template <class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
{
  if (this != &rhs)
  {
    destroyRange(0, size_);
    deallocate();
    data_ = rhs.data_;
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    rhs.data_ = nullptr;
    rhs.size_ = 0;
    rhs.capacity_ = 0;
  }
  return *this;
}

template <class T>
void Vector<T>::swap(Vector<T>& rhs) noexcept
{
  std::swap(data_, rhs.data_);
  std::swap(size_, rhs.size_);
  std::swap(capacity_, rhs.capacity_);
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs)
{
  if (this != &rhs)
  {
    Vector<T> cpy(rhs);
    swap(cpy);
  }
  return *this;
}

template <class T>
Vector<T>::Vector(const Vector<T>& rhs)
  : data_(nullptr),
    size_(0),
    capacity_(0)
{
  reserve(rhs.size_);
  size_ = rhs.size_;
  for (size_t i = 0; i < size_; ++i)
  {
    new (data_ + i) T(rhs.data_[i]);
  }
}

template <class T>
void Vector<T>::pushBack(const T& val)
{
  if (size_ == capacity_)
  {
    size_t newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;
    reserve(newCapacity);
  }
  new (data_ + size_) T(val);
  ++size_;
}

template <class T>
void Vector<T>::insert(size_t pos, const T& val)
{
  if (pos > size_)
  {
    throw std::out_of_range("Vector::insert: position out of range");
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * (size_ + 1)));
  for (size_t i = 0; i < pos; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  new (newData + pos) T(val);
  for (size_t i = pos; i < size_; ++i)
  {
    new (newData + i + 1) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  ++size_;
  capacity_ = size_;
}

template <class T>
void Vector<T>::insert(size_t pos, const Vector<T>& rhs, size_t b, size_t e)
{
  if (pos > size_)
  {
    throw std::out_of_range("Vector::insert: position out of range");
  }
  if (b > e || e > rhs.size_)
  {
    throw std::out_of_range("Vector::insert: range out of range");
  }
  size_t count = e - b;
  if (count == 0)
  {
    return;
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * (size_ + count)));
  for (size_t i = 0; i < pos; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  for (size_t i = 0; i < count; ++i)
  {
    new (newData + pos + i) T(rhs.data_[b + i]);
  }
  for (size_t i = pos; i < size_; ++i)
  {
    new (newData + i + count) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  size_ += count;
  capacity_ = size_;
}

template <class T>
void Vector<T>::erase(size_t pos)
{
  if (pos >= size_)
  {
    throw std::out_of_range("Vector::erase: position out of range");
  }
  data_[pos].~T();
  for (size_t i = pos + 1; i < size_; ++i)
  {
    new (data_ + i - 1) T(std::move(data_[i]));
    data_[i].~T();
  }
  --size_;
}

template <class T>
struct Vector<T>::VectorIterator
{
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  VectorIterator()
    : ptr_(nullptr)
  {
  }
  explicit VectorIterator(pointer ptr)
    : ptr_(ptr)
  {
  }

  reference operator*() const
  {
    return *ptr_;
  }
  pointer operator->() const
  {
    return ptr_;
  }

  VectorIterator& operator++()
  {
    ++ptr_;
    return *this;
  }
  VectorIterator operator++(int)
  {
    VectorIterator tmp = *this;
    ++ptr_;
    return tmp;
  }
  VectorIterator& operator--()
  {
    --ptr_;
    return *this;
  }
  VectorIterator operator--(int)
  {
    VectorIterator tmp = *this;
    --ptr_;
    return tmp;
  }

  VectorIterator operator+(difference_type n) const
  {
    return VectorIterator(ptr_ + n);
  }
  VectorIterator operator-(difference_type n) const
  {
    return VectorIterator(ptr_ - n);
  }
  difference_type operator-(const VectorIterator& other) const
  {
    return ptr_ - other.ptr_;
  }

  VectorIterator& operator+=(difference_type n)
  {
    ptr_ += n;
    return *this;
  }
  VectorIterator& operator-=(difference_type n)
  {
    ptr_ -= n;
    return *this;
  }

  reference operator[](difference_type n) const
  {
    return ptr_[n];
  }

  bool operator==(const VectorIterator& other) const
  {
    return ptr_ == other.ptr_;
  }
  bool operator!=(const VectorIterator& other) const
  {
    return ptr_ != other.ptr_;
  }
  bool operator<(const VectorIterator& other) const
  {
    return ptr_ < other.ptr_;
  }
  bool operator>(const VectorIterator& other) const
  {
    return ptr_ > other.ptr_;
  }
  bool operator<=(const VectorIterator& other) const
  {
    return ptr_ <= other.ptr_;
  }
  bool operator>=(const VectorIterator& other) const
  {
    return ptr_ >= other.ptr_;
  }

private:
  pointer ptr_;
};

template <class T>
typename Vector<T>::iterator Vector<T>::begin()
{
  return iterator(data_);
}

template <class T>
typename Vector<T>::iterator Vector<T>::end()
{
  return iterator(data_ + size_);
}

template <class T>
void Vector<T>::insert(iterator pos, const T& val)
{
  size_t index = pos - begin();
  insert(index, val);
}

template <class T>
template <class IT>
void Vector<T>::insert(iterator pos, IT beg, IT end)
{
  size_t index = pos - this->begin();
  size_t count = 0;
  for (IT it = beg; it != end; ++it)
  {
    ++count;
  }
  if (count == 0)
  {
    return;
  }
  T* newData = static_cast<T*>(::operator new(sizeof(T) * (size_ + count)));
  for (size_t i = 0; i < index; ++i)
  {
    new (newData + i) T(std::move(data_[i]));
    data_[i].~T();
  }
  size_t j = 0;
  for (IT it = beg; it != end; ++it, ++j)
  {
    new (newData + index + j) T(*it);
  }
  for (size_t i = index; i < size_; ++i)
  {
    new (newData + i + count) T(std::move(data_[i]));
    data_[i].~T();
  }
  deallocate();
  data_ = newData;
  size_ += count;
  capacity_ = size_;
}

template <class T>
void Vector<T>::erase(iterator pos)
{
  size_t index = pos - begin();
  erase(index);
}

template <class T>
void Vector<T>::erase(iterator first, iterator last)
{
  size_t first_idx = first - begin();
  size_t last_idx = last - begin();
  if (first_idx > last_idx)
  {
    throw std::out_of_range("Vector::erase: invalid range");
  }
  if (first_idx >= size_)
  {
    throw std::out_of_range("Vector::erase: start out of range");
  }
  if (last_idx > size_)
  {
    throw std::out_of_range("Vector::erase: end out of range");
  }
  size_t count = last_idx - first_idx;
  if (count == 0)
  {
    return;
  }
  destroyRange(first_idx, last_idx);
  for (size_t i = last_idx; i < size_; ++i)
  {
    new (data_ + i - count) T(std::move(data_[i]));
    data_[i].~T();
  }
  size_ -= count;
}

template <class T>
void Vector<T>::eraseValue(const T& value)
{
  for (size_t i = 0; i < size_;)
  {
    if (data_[i] == value)
    {
      erase(i);
    }
    else
    {
      ++i;
    }
  }
}

template <class T>
size_t Vector<T>::getCapacity() const noexcept
{
  return capacity_;
}

template <class T>
bool Vector<T>::operator==(const Vector<T>& rhs) const noexcept
{
  if (size_ != rhs.size_)
  {
    return false;
  }
  for (size_t i = 0; i < size_; ++i)
  {
    if (data_[i] != rhs.data_[i])
    {
      return false;
    }
  }
  return true;
}

template <class T>
T& Vector<T>::operator[](size_t id) noexcept
{
  return data_[id];
}

template <class T>
const T& Vector<T>::operator[](size_t id) const noexcept
{
  return data_[id];
}

template <class T>
bool Vector<T>::operator!=(const Vector<T>& rhs) const noexcept
{
  return !(*this == rhs);
}

template <class T>
T& Vector<T>::at(size_t id)
{
  if (id >= size_)
  {
    throw std::out_of_range("Vector::at: index out of range");
  }
  return data_[id];
}

template <class T>
const T& Vector<T>::at(size_t id) const
{
  if (id >= size_)
  {
    throw std::out_of_range("Vector::at: index out of range");
  }
  return data_[id];
}

template <class T>
Vector<T>::Vector(size_t size, const T& val)
  : data_(nullptr),
    size_(size),
    capacity_(size)
{
  if (size_ == 0)
  {
    return;
  }
  data_ = static_cast<T*>(::operator new(sizeof(T) * size_));
  for (size_t i = 0; i < size_; ++i)
  {
    new (data_ + i) T(val);
  }
}

template <class T>
Vector<T>::Vector(size_t size)
  : data_(nullptr),
    size_(0),
    capacity_(0)
{
  reserve(size);
}

template <class T>
bool Vector<T>::isEmpty() const noexcept
{
  return !size_;
}

template <class T>
size_t Vector<T>::getSize() const noexcept
{
  return size_;
}

template <class T>
Vector<T>::Vector()
  : data_(nullptr),
    size_(0),
    capacity_(0)
{
}

template <class T>
Vector<T>::~Vector()
{
  destroyRange(0, size_);
  deallocate();
}

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
      if (this != &other)
      {
        data = other.data;
        state = other.state;
      }
      return *this;
    }

    Slot(Slot&& other) noexcept
      : data(std::move(other.data)), state(other.state)
    {
      other.state = EMPTY;
    }
    Slot& operator=(Slot&& other) noexcept
    {
      if (this != &other)
      {
        data = std::move(other.data);
        state = other.state;
        other.state = EMPTY;
      }
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

  std::size_t home_bucket(const Key& k) const
  {
    return hasher_(k) % bucket_count_;
  }

  std::size_t bucket_offset(std::size_t bucket) const
  {
    return bucket * bucket_size_;
  }

  Slot* find_slot_any(const Key& key)
  {
    std::size_t b = home_bucket(key);
    Slot* s = find_in_bucket(key, b);
    if (s)
    {
      return s;
    }
    return find_in_overflow(key);
  }

  const Slot* find_slot_any(const Key& key) const
  {
    std::size_t b = home_bucket(key);
    const Slot* s = find_in_bucket(key, b);
    if (s)
    {
      return s;
    }
    return find_in_overflow(key);
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
      if (s.state == Slot::EMPTY)
      {
        break;
      }
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE)
          && equal_(s.data.first, key))
      {
        return &s;
      }
    }
    return nullptr;
  }

  const Slot* find_in_bucket(const Key& key, std::size_t bucket) const
  {
    std::size_t start = bucket_offset(bucket);
    for (std::size_t i = 0; i < bucket_size_; ++i)
    {
      const Slot& s = slots_[start + i];
      if (s.state == Slot::EMPTY)
      {
        break;
      }
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE)
          && equal_(s.data.first, key))
      {
        return &s;
      }
    }
    return nullptr;
  }

  Slot* find_empty_in_bucket(std::size_t bucket)
  {
    std::size_t start = bucket_offset(bucket);
    for (std::size_t i = 0; i < bucket_size_; ++i)
    {
      if (slots_[start + i].state == Slot::EMPTY)
      {
        return &slots_[start + i];
      }
    }
    return nullptr;
  }

  Slot* find_in_overflow(const Key& key)
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
    {
      Slot& s = slots_[overflow_start_ + i];
      if (s.state == Slot::EMPTY)
      {
        break;
      }
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE)
          && equal_(s.data.first, key))
      {
        return &s;
      }
    }
    return nullptr;
  }

  const Slot* find_in_overflow(const Key& key) const
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
    {
      const Slot& s = slots_[overflow_start_ + i];
      if (s.state == Slot::EMPTY)
      {
        break;
      }
      if ((s.state == Slot::OCCUPIED || s.state == Slot::TOMBSTONE)
          && equal_(s.data.first, key))
      {
        return &s;
      }
    }
    return nullptr;
  }

  Slot* find_empty_in_overflow()
  {
    for (std::size_t i = 0; i < overflow_size_; ++i)
    {
      if (slots_[overflow_start_ + i].state == Slot::EMPTY)
      {
        return &slots_[overflow_start_ + i];
      }
    }
    return nullptr;
  }

public:
  BucketHashTable(std::size_t bucket_count = 16,
                  std::size_t bucket_size = 4,
                  std::size_t overflow_size = 16,
                  Hash h = Hash(), Equal e = Equal())
    : slots_(nullptr), bucket_count_(bucket_count ? bucket_count : 1),
      bucket_size_(bucket_size ? bucket_size : 1),
      total_slots_(bucket_count_ * bucket_size_ + (overflow_size ? overflow_size : 1)),
      overflow_start_(bucket_count_ * bucket_size_),
      overflow_size_(overflow_size ? overflow_size : 1),
      size_(0), hasher_(h), equal_(e)
  {
    slots_ = static_cast<Slot*>(::operator new(sizeof(Slot) * total_slots_));
    for (std::size_t i = 0; i < total_slots_; ++i)
    {
      new (slots_ + i) Slot();
    }
  }

  ~BucketHashTable()
  {
    if (slots_)
    {
      for (std::size_t i = 0; i < total_slots_; ++i)
      {
        slots_[i].~Slot();
      }
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
    for (std::size_t i = 0; i < total_slots_; ++i)
    {
      new (slots_ + i) Slot(other.slots_[i]);
    }
  }

  BucketHashTable(BucketHashTable&& other) noexcept
    : slots_(other.slots_),
      bucket_count_(other.bucket_count_),
      bucket_size_(other.bucket_size_),
      total_slots_(other.total_slots_),
      overflow_start_(other.overflow_start_),
      overflow_size_(other.overflow_size_),
      size_(other.size_),
      hasher_(std::move(other.hasher_)),
      equal_(std::move(other.equal_))
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

  BucketHashTable& operator=(BucketHashTable other)
  {
    swap(other);
    return *this;
  }

  BucketHashTable& operator=(BucketHashTable&& other) noexcept
  {
    if (this != &other)
    {
      swap(other);
    }
    return *this;
  }

  class iterator
  {
    friend class BucketHashTable;
    Slot* ptr_;
    Slot* end_;
    void skip_empty()
    {
      while (ptr_ != end_ && ptr_->state != Slot::OCCUPIED)
      {
        ++ptr_;
      }
    }
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
    void skip_empty()
    {
      while (ptr_ != end_ && ptr_->state != Slot::OCCUPIED)
      {
        ++ptr_;
      }
    }
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

  bool has(const Key& key) const
  {
    return find_slot(key) != nullptr;
  }

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
    if (!dest)
    {
      dest = find_empty_in_overflow();
    }
    if (!dest)
    {
      throw std::runtime_error("Hash table overflow");
    }

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
    if (s)
    {
      return iterator(s, slots_ + total_slots_);
    }
    return end();
  }

  const_iterator find(const Key& key) const
  {
    const Slot* s = find_slot(key);
    if (s)
    {
      return const_iterator(s, slots_ + total_slots_);
    }
    return end();
  }

  void rehash(std::size_t new_bucket_count, std::size_t new_bucket_size,
              std::size_t new_overflow_size)
  {
    BucketHashTable tmp(new_bucket_count, new_bucket_size, new_overflow_size,
                        hasher_, equal_);
    for (auto it = begin(); it != end(); ++it)
    {
      tmp.add(it->first, it->second);
    }
    swap(tmp);
  }

  std::size_t size() const { return size_; }
  std::size_t bucket_count() const { return bucket_count_; }
};

}

#endif
