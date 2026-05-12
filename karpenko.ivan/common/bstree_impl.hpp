#ifndef KARPENKO_BSTREE_IMPL_HPP
#define KARPENKO_BSTREE_IMPL_HPP

#include "bstree.hpp"
#include <stdexcept>
#include <utility>

namespace karpenko
{

template < typename Key, typename Value, typename Compare >
BSTree< Key, Value, Compare >::BSTree()
  : fake_root(new Node()), cmp(Compare())
{
  fake_root->parent = nullptr;
  fake_root->left = nullptr;
  fake_root->right = nullptr;
}

template < typename Key, typename Value, typename Compare >
BSTree< Key, Value, Compare >::~BSTree()
{
  clear_tree(fake_root->left);
  delete fake_root;
}

template < typename Key, typename Value, typename Compare >
BSTree< Key, Value, Compare >::BSTree(BSTree&& other) noexcept
  : fake_root(other.fake_root), cmp(std::move(other.cmp))
{
  other.fake_root = new Node();
}

template < typename Key, typename Value, typename Compare >
BSTree< Key, Value, Compare >& BSTree< Key, Value, Compare >::operator=(BSTree&& other) noexcept
{
  if (this != &other) {
    clear_tree(fake_root->left);
    delete fake_root;
    fake_root = other.fake_root;
    cmp = std::move(other.cmp);
    other.fake_root = new Node();
  }
  return *this;
}

template < typename Key, typename Value, typename Compare >
void BSTree< Key, Value, Compare >::push(const Key& k, Value v)
{
  if (fake_root->left == nullptr) {
    Node* new_node = new Node(k, std::move(v));
    fake_root->left = new_node;
    new_node->parent = fake_root;
    return;
  }

  Node* current = fake_root->left;
  Node* parent = fake_root;

  while (current) {
    parent = current;
    if (cmp(k, current->data.first)) {
      current = current->left;
    } else if (cmp(current->data.first, k)) {
      current = current->right;
    } else {
      current->data.second = std::move(v);
      return;
    }
  }

  Node* new_node = new Node(k, std::move(v));
  if (cmp(k, parent->data.first)) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }
  new_node->parent = parent;
}

template < typename Key, typename Value, typename Compare >
Value BSTree< Key, Value, Compare >::get(const Key& k) const
{
  Node* n = find_node(k);
  if (!n) {
    throw std::out_of_range("key not found");
  }
  return n->data.second;
}

template < typename Key, typename Value, typename Compare >
Value BSTree< Key, Value, Compare >::drop(const Key& k)
{
  Node* n = find_node(k);
  if (!n) {
    throw std::out_of_range("key not found");
  }
  Value ret = n->data.second;
  remove_node(n);
  return ret;
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::find(const Key& k) const
{
  Node* n = find_node(k);
  return const_iterator(n ? n : fake_root);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::begin() const
{
  Node* cur = fake_root->left;
  if (!cur) {
    return end();
  }
  while (cur->left) {
    cur = cur->left;
  }
  return const_iterator(cur);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::end() const
{
  return const_iterator(fake_root);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::rotateLeft(const_iterator it)
{
  Node* x = const_cast< Node* >(it.node);
  if (!x || x == fake_root || !x->parent || x->parent == fake_root) {
    throw std::logic_error("impossible left rotation");
  }
  Node* p = x->parent;
  if (x != p->right) {
    throw std::logic_error("node is not a right child for left rotation");
  }

  p->right = x->left;
  if (x->left) {
    x->left->parent = p;
  }
  x->left = p;
  x->parent = p->parent;
  if (p->parent->left == p) {
    p->parent->left = x;
  } else {
    p->parent->right = x;
  }
  p->parent = x;
  return const_iterator(x);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::rotateRight(const_iterator it)
{
  Node* x = const_cast< Node* >(it.node);
  if (!x || x == fake_root || !x->parent || x->parent == fake_root) {
    throw std::logic_error("impossible right rotation");
  }
  Node* p = x->parent;
  if (x != p->left) {
    throw std::logic_error("node is not a left child for right rotation");
  }

  p->left = x->right;
  if (x->right) {
    x->right->parent = p;
  }
  x->right = p;
  x->parent = p->parent;
  if (p->parent->left == p) {
    p->parent->left = x;
  } else {
    p->parent->right = x;
  }
  p->parent = x;
  return const_iterator(x);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::rotateLargeLeft(const_iterator it)
{
  Node* x = const_cast< Node* >(it.node);
  if (!x || x == fake_root || !x->parent || x->parent == fake_root) {
    throw std::logic_error("impossible large left rotation");
  }
  Node* p = x->parent;
  Node* g = p->parent;
  if (p != g->right || x != p->left) {
    throw std::logic_error("wrong structure for large left rotation");
  }

  rotateRight(const_iterator(x));
  return rotateLeft(const_iterator(x));
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::const_iterator
BSTree< Key, Value, Compare >::rotateLargeRight(const_iterator it)
{
  Node* x = const_cast< Node* >(it.node);
  if (!x || x == fake_root || !x->parent || x->parent == fake_root) {
    throw std::logic_error("impossible large right rotation");
  }
  Node* p = x->parent;
  Node* g = p->parent;
  if (p != g->left || x != p->right) {
    throw std::logic_error("wrong structure for large right rotation");
  }

  rotateLeft(const_iterator(x));
  return rotateRight(const_iterator(x));
}

template < typename Key, typename Value, typename Compare >
size_t BSTree< Key, Value, Compare >::height() const
{
  return node_height(fake_root->left);
}

template < typename Key, typename Value, typename Compare >
size_t BSTree< Key, Value, Compare >::height(const_iterator it) const
{
  return node_height(it.node);
}

template < typename Key, typename Value, typename Compare >
typename BSTree< Key, Value, Compare >::Node*
BSTree< Key, Value, Compare >::find_node(const Key& k) const
{
  Node* cur = fake_root->left;
  while (cur) {
    if (cmp(k, cur->data.first)) {
      cur = cur->left;
    } else if (cmp(cur->data.first, k)) {
      cur = cur->right;
    } else {
      return cur;
    }
  }
  return nullptr;
}

template < typename Key, typename Value, typename Compare >
void BSTree< Key, Value, Compare >::remove_node(Node* n)
{
  if (!n || n == fake_root) {
    throw std::logic_error("invalid node for removal");
  }

  if (n->left && n->right) {
    Node* succ = n->right;
    while (succ->left) {
      succ = succ->left;
    }
    const_cast< Key& >(n->data.first) = succ->data.first;
    n->data.second = std::move(succ->data.second);
    n = succ;
  }

  Node* child = n->left ? n->left : n->right;
  if (child) {
    child->parent = n->parent;
  }
  if (n->parent->left == n) {
    n->parent->left = child;
  } else {
    n->parent->right = child;
  }
  delete n;
}

template < typename Key, typename Value, typename Compare >
void BSTree< Key, Value, Compare >::clear_tree(Node* node)
{
  if (!node) {
    return;
  }
  clear_tree(node->left);
  clear_tree(node->right);
  delete node;
}

template < typename Key, typename Value, typename Compare >
size_t BSTree< Key, Value, Compare >::node_height(const Node* node) const
{
  if (!node || node == fake_root) {
    return 0;
  }
  size_t lh = node_height(node->left);
  size_t rh = node_height(node->right);
  return 1 + (lh > rh ? lh : rh);
}

}

#endif
