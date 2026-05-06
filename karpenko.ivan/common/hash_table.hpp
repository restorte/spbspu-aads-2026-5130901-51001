#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>

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

// ── реализация конструкторов / деструктора / pushBack ──

template <class T>
Vector<T>::Vector() : data_(nullptr), size_(0), capacity_(0)
{
}

template <class T>
Vector<T>::Vector(size_t size, const T& val)
  : data_(nullptr), size_(size), capacity_(size)
{
  if (size_ == 0) return;
  data_ = static_cast<T*>(::operator new(sizeof(T) * size_));
  for (size_t i = 0; i < size_; ++i)
  {
    new (data_ + i) T(val);
  }
}

template <class T>
Vector<T>::Vector(size_t size)
  : data_(nullptr), size_(0), capacity_(0)
{
  reserve(size);
}

template <class T>
Vector<T>::Vector(std::initializer_list<T> il)
  : data_(nullptr), size_(0), capacity_(0)
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
Vector<T>::Vector(Vector<T>&& rhs) noexcept
  : data_(rhs.data_), size_(rhs.size_), capacity_(rhs.capacity_)
{
  rhs.data_ = nullptr;
  rhs.size_ = 0;
  rhs.capacity_ = 0;
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
Vector<T>::Vector(const Vector<T>& rhs)
  : data_(nullptr), size_(0), capacity_(0)
{
  reserve(rhs.size_);
  size_ = rhs.size_;
  for (size_t i = 0; i < size_; ++i)
  {
    new (data_ + i) T(rhs.data_[i]);
  }
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
Vector<T>::~Vector()
{
  destroyRange(0, size_);
  deallocate();
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
void Vector<T>::reserve(size_t k)
{
  if (k <= capacity_) return;
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
void Vector<T>::shrinkToFit()
{
  if (size_ == capacity_) return;
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
void Vector<T>::repeatPushBack(const T& val, size_t k)
{
  if (k == 0) return;
  reserve(size_ + k);
  for (size_t i = 0; i < k; ++i)
  {
    new (data_ + size_ + i) T(val);
  }
  size_ += k;
}

template <class T>
template <class IT>
void Vector<T>::rangedPushBack(IT beg, size_t count)
{
  if (count == 0) return;
  reserve(size_ + count);
  for (size_t i = 0; i < count; ++i, ++beg)
  {
    new (data_ + size_ + i) T(*beg);
  }
  size_ += count;
}

template <class T>
void Vector<T>::repeatInsert(size_t id, const T& val, size_t k)
{
  if (id > size_) throw std::out_of_range("Vector::repeatInsert: position out of range");
  if (k == 0) return;
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
bool Vector<T>::operator==(const Vector<T>& rhs) const noexcept
{
  if (size_ != rhs.size_) return false;
  for (size_t i = 0; i < size_; ++i)
  {
    if (data_[i] != rhs.data_[i]) return false;
  }
  return true;
}

template <class T>
bool Vector<T>::operator!=(const Vector<T>& rhs) const noexcept
{
  return !(*this == rhs);
}

}

#endif
