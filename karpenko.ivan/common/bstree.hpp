#ifndef KARPENKO_BSTREE_HPP
#define KARPENKO_BSTREE_HPP

#include <functional>
#include <cstddef>
#include <string>
#include "bst_iterator.hpp"

namespace karpenko
{

inline void split(const std::string& s, std::string*& out, size_t& count)
{
  count = 0;
  size_t pos = 0;
  const size_t n = s.size();

  while (pos < n) {
    while (pos < n && s[pos] == ' ') {
      ++pos;
    }
    if (pos < n) {
      ++count;
      while (pos < n && s[pos] != ' ') {
        ++pos;
      }
    }
  }

  out = new std::string[count];
  pos = 0;
  size_t idx = 0;
  while (pos < n) {
    while (pos < n && s[pos] == ' ') {
      ++pos;
    }
    const size_t start = pos;
    while (pos < n && s[pos] != ' ') {
      ++pos;
    }
    if (start < pos) {
      out[idx++] = s.substr(start, pos - start);
    }
  }
}

template < typename Key, typename Value, typename Compare = std::less< Key > >
class BSTree
{
public:
  using Node = TreeNode< Key, Value >;
  using iterator = BSTIterator< Key, Value >;
  using const_iterator = BSTConstIterator< Key, Value >;

  BSTree();
  ~BSTree();

  BSTree(const BSTree&) = delete;
  BSTree& operator=(const BSTree&) = delete;

  BSTree(BSTree&& other) noexcept;
  BSTree& operator=(BSTree&& other) noexcept;

  void push(const Key& k, Value v);
  Value get(const Key& k) const;
  Value drop(const Key& k);

  const_iterator find(const Key& k) const;
  const_iterator begin() const;
  const_iterator end() const;

  const_iterator rotateLeft(const_iterator it);
  const_iterator rotateRight(const_iterator it);
  const_iterator rotateLargeLeft(const_iterator it);
  const_iterator rotateLargeRight(const_iterator it);

  size_t height() const;
  size_t height(const_iterator it) const;

private:
  Node* fake_root;
  Compare cmp;

  Node* find_node(const Key& k) const;
  void remove_node(Node* n);
  void clear_tree(Node* node);
  size_t node_height(const Node* node) const;
};

}

#include "bstree_impl.hpp"

#endif
