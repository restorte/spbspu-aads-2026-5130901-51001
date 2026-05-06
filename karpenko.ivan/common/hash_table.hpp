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
};

}

#endif
