#ifndef KARPENKO_BST_ITERATOR_HPP
#define KARPENKO_BST_ITERATOR_HPP

#include "tree_node.hpp"
#include <ostream>

namespace karpenko
{

template < typename Key, typename Value >
class BSTIterator
{
public:
  using Node = TreeNode< Key, Value >;
  Node* node;

  explicit BSTIterator(Node* n)
    : node(n)
  {
  }

  std::pair< const Key, Value >& operator*() const
  {
    return node->data;
  }

  std::pair< const Key, Value >* operator->() const
  {
    return &node->data;
  }

  BSTIterator& operator++()
  {
    if (node->right) {
      node = node->right;
      while (node->left) {
        node = node->left;
      }
    } else {
      Node* parent = node->parent;
      while (parent && node == parent->right) {
        node = parent;
        parent = parent->parent;
      }
      node = parent;
    }
    return *this;
  }

  BSTIterator operator++(int)
  {
    BSTIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const BSTIterator& other) const
  {
    return node == other.node;
  }

  bool operator!=(const BSTIterator& other) const
  {
    return node != other.node;
  }
};

template < typename Key, typename Value >
class BSTConstIterator
{
public:
  using Node = TreeNode< Key, Value >;
  const Node* node;

  explicit BSTConstIterator(const Node* n)
    : node(n)
  {
  }

  const std::pair< const Key, Value >& operator*() const
  {
    return node->data;
  }

  const std::pair< const Key, Value >* operator->() const
  {
    return &node->data;
  }

  BSTConstIterator& operator++()
  {
    if (node->right) {
      node = node->right;
      while (node->left) {
        node = node->left;
      }
    } else {
      const Node* parent = node->parent;
      while (parent && node == parent->right) {
        node = parent;
        parent = parent->parent;
      }
      node = parent;
    }
    return *this;
  }

  BSTConstIterator operator++(int)
  {
    BSTConstIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const BSTConstIterator& other) const
  {
    return node == other.node;
  }

  bool operator!=(const BSTConstIterator& other) const
  {
    return node != other.node;
  }
};

template < typename Key, typename Value >
std::ostream& operator<<(std::ostream& os, const BSTIterator< Key, Value >& it)
{
  if (it.node) {
    os << "iterator( key=" << it.node->data.first << " )";
  } else {
    os << "end-iterator";
  }
  return os;
}

template < typename Key, typename Value >
std::ostream& operator<<(std::ostream& os, const BSTConstIterator< Key, Value >& it)
{
  if (it.node) {
    os << "const_iterator( key=" << it.node->data.first << " )";
  } else {
    os << "end-const_iterator";
  }
  return os;
}

}

#endif
