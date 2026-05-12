#ifndef KARPENKO_TREE_NODE_HPP
#define KARPENKO_TREE_NODE_HPP

#include <utility>

namespace karpenko
{

template < typename Key, typename Value >
struct TreeNode
{
  std::pair< const Key, Value > data;
  TreeNode* left;
  TreeNode* right;
  TreeNode* parent;

  TreeNode(const Key& k, Value v)
    : data(k, std::move(v)), left(nullptr), right(nullptr), parent(nullptr)
  {
  }

  TreeNode()
    : data(Key(), Value()), left(nullptr), right(nullptr), parent(nullptr)
  {
  }
};

}

#endif
