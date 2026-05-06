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
};

}

#endif
