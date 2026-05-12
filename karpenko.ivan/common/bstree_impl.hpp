#ifndef KARPENKO_BSTREE_IMPL_HPP
#define KARPENKO_BSTREE_IMPL_HPP

#include "bstree.hpp"
#include <stdexcept>
#include <utility>

namespace karpenko {

void split(const std::string& s, std::string*& out, size_t& count)
{
  count = 0;
  size_t pos = 0;
  const size_t n = s.size();

  while (pos < n)
  {
    while (pos < n && s[pos] == ' ')
    {
      ++pos;
    }
    if (pos < n)
    {
      ++count;
      while (pos < n && s[pos] != ' ')
      {
        ++pos;
      }
    }
  }

  out = new std::string[count];
  pos = 0;
  size_t idx = 0;
  while (pos < n)
  {
    while (pos < n && s[pos] == ' ')
    {
      ++pos;
    }
    size_t start = pos;
    while (pos < n && s[pos] != ' ')
    {
      ++pos;
    }
    if (start < pos)
    {
      out[idx++] = s.substr(start, pos - start);
    }
  }
}

template <typename Key, typename Value, typename Compare>
BSTree<Key, Value, Compare>::BSTree()
  : fake_root_(new Node()),
    cmp_(Compare())
{
  fake_root_->parent_ = nullptr;
  fake_root_->left_ = nullptr;
  fake_root_->right_ = nullptr;
}

template <typename Key, typename Value, typename Compare>
BSTree<Key, Value, Compare>::~BSTree()
{
  clear_tree(fake_root_->left_);
  delete fake_root_;
}

template <typename Key, typename Value, typename Compare>
BSTree<Key, Value, Compare>::BSTree(BSTree&& other) noexcept
  : fake_root_(other.fake_root_),
    cmp_(std::move(other.cmp_))
{
  other.fake_root_ = new Node();
}

template <typename Key, typename Value, typename Compare>
BSTree<Key, Value, Compare>& BSTree<Key, Value, Compare>::operator=(BSTree&& other) noexcept
{
  if (this != &other)
  {
    clear_tree(fake_root_->left_);
    delete fake_root_;
    fake_root_ = other.fake_root_;
    cmp_ = std::move(other.cmp_);
    other.fake_root_ = new Node();
  }
  return *this;
}

template <typename Key, typename Value, typename Compare>
void BSTree<Key, Value, Compare>::push(const Key& k, Value v)
{
  if (fake_root_->left_ == nullptr)
  {
    fake_root_->left_ = new Node(k, std::move(v));
    fake_root_->left_->parent_ = fake_root_;
    return;
  }

  Node* current = fake_root_->left_;
  Node* parent = fake_root_;

  while (current)
  {
    parent = current;
    if (cmp_(k, current->data_.first))
    {
      current = current->left_;
    }
    else if (cmp_(current->data_.first, k))
    {
      current = current->right_;
    }
    else
    {
      current->data_.second = std::move(v);
      return;
    }
  }

  Node* new_node = new Node(k, std::move(v));
  if (cmp_(k, parent->data_.first))
  {
    parent->left_ = new_node;
  }
  else
  {
    parent->right_ = new_node;
  }
  new_node->parent_ = parent;
}

template <typename Key, typename Value, typename Compare>
Value BSTree<Key, Value, Compare>::get(const Key& k) const
{
  Node* n = find_node(k);
  if (!n)
  {
    throw std::out_of_range("key not found");
  }
  return n->data_.second;
}

template <typename Key, typename Value, typename Compare>
Value BSTree<Key, Value, Compare>::drop(const Key& k)
{
  Node* n = find_node(k);
  if (!n)
  {
    throw std::out_of_range("key not found");
  }
  Value ret = n->data_.second;
  remove_node(n);
  return ret;
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::find(const Key& k) const
{
  Node* n = find_node(k);
  return const_iterator(n ? n : fake_root_);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::begin() const
{
  Node* cur = fake_root_->left_;
  if (!cur)
  {
    return end();
  }
  while (cur->left_)
  {
    cur = cur->left_;
  }
  return const_iterator(cur);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::end() const
{
  return const_iterator(fake_root_);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::rotateLeft(const_iterator it)
{
  Node* x = const_cast<Node*>(it.node_);
  if (!x || x == fake_root_ || !x->parent_ || x->parent_ == fake_root_)
  {
    throw std::logic_error("impossible left rotation");
  }
  Node* p = x->parent_;
  if (x != p->right_)
  {
    throw std::logic_error("node is not a right child for left rotation");
  }

  p->right_ = x->left_;
  if (x->left_)
  {
    x->left_->parent_ = p;
  }
  x->left_ = p;
  x->parent_ = p->parent_;
  if (p->parent_->left_ == p)
  {
    p->parent_->left_ = x;
  }
  else
  {
    p->parent_->right_ = x;
  }
  p->parent_ = x;
  return const_iterator(x);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::rotateRight(const_iterator it)
{
  Node* x = const_cast<Node*>(it.node_);
  if (!x || x == fake_root_ || !x->parent_ || x->parent_ == fake_root_)
  {
    throw std::logic_error("impossible right rotation");
  }
  Node* p = x->parent_;
  if (x != p->left_)
  {
    throw std::logic_error("node is not a left child for right rotation");
  }

  p->left_ = x->right_;
  if (x->right_)
  {
    x->right_->parent_ = p;
  }
  x->right_ = p;
  x->parent_ = p->parent_;
  if (p->parent_->left_ == p)
  {
    p->parent_->left_ = x;
  }
  else
  {
    p->parent_->right_ = x;
  }
  p->parent_ = x;
  return const_iterator(x);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::rotateLargeLeft(const_iterator it)
{
  Node* x = const_cast<Node*>(it.node_);
  if (!x || x == fake_root_ || !x->parent_ || x->parent_ == fake_root_)
  {
    throw std::logic_error("impossible large left rotation");
  }
  Node* p = x->parent_;
  Node* g = p->parent_;
  if (p != g->right_ || x != p->left_)
  {
    throw std::logic_error("wrong structure for large left rotation");
  }

  rotateRight(const_iterator(x));
  return rotateLeft(const_iterator(x));
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::const_iterator
BSTree<Key, Value, Compare>::rotateLargeRight(const_iterator it)
{
  Node* x = const_cast<Node*>(it.node_);
  if (!x || x == fake_root_ || !x->parent_ || x->parent_ == fake_root_)
  {
    throw std::logic_error("impossible large right rotation");
  }
  Node* p = x->parent_;
  Node* g = p->parent_;
  if (p != g->left_ || x != p->right_)
  {
    throw std::logic_error("wrong structure for large right rotation");
  }

  rotateLeft(const_iterator(x));
  return rotateRight(const_iterator(x));
}

template <typename Key, typename Value, typename Compare>
size_t BSTree<Key, Value, Compare>::height() const
{
  return node_height(fake_root_->left_);
}

template <typename Key, typename Value, typename Compare>
size_t BSTree<Key, Value, Compare>::height(const_iterator it) const
{
  return node_height(it.node_);
}

template <typename Key, typename Value, typename Compare>
typename BSTree<Key, Value, Compare>::Node*
BSTree<Key, Value, Compare>::find_node(const Key& k) const
{
  Node* cur = fake_root_->left_;
  while (cur)
  {
    if (cmp_(k, cur->data_.first))
    {
      cur = cur->left_;
    }
    else if (cmp_(cur->data_.first, k))
    {
      cur = cur->right_;
    }
    else
    {
      return cur;
    }
  }
  return nullptr;
}

template <typename Key, typename Value, typename Compare>
void BSTree<Key, Value, Compare>::remove_node(Node* n)
{
  if (!n || n == fake_root_)
  {
    throw std::logic_error("invalid node for removal");
  }

  if (n->left_ && n->right_)
  {
    Node* succ = n->right_;
    while (succ->left_)
    {
      succ = succ->left_;
    }
    const_cast<Key&>(n->data_.first) = succ->data_.first;
    n->data_.second = std::move(succ->data_.second);
    n = succ;
  }

  Node* child = n->left_ ? n->left_ : n->right_;
  if (child)
  {
    child->parent_ = n->parent_;
  }
  if (n->parent_->left_ == n)
  {
    n->parent_->left_ = child;
  }
  else
  {
    n->parent_->right_ = child;
  }
  delete n;
}

template <typename Key, typename Value, typename Compare>
void BSTree<Key, Value, Compare>::clear_tree(Node* node)
{
  if (!node)
  {
    return;
  }
  clear_tree(node->left_);
  clear_tree(node->right_);
  delete node;
}

template <typename Key, typename Value, typename Compare>
size_t BSTree<Key, Value, Compare>::node_height(const Node* node) const
{
  if (!node || node == fake_root_)
  {
    return 0;
  }
  size_t lh = node_height(node->left_);
  size_t rh = node_height(node->right_);
  return 1 + (lh > rh ? lh : rh);
}

}

#endif
