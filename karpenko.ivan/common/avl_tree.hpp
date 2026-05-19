#ifndef KARPENKO_AVL_TREE_HPP
#define KARPENKO_AVL_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace karpenko
{

template <typename Key, typename Value>
struct AVLNode
{
    std::pair<const Key, Value> data;
    AVLNode* left;
    AVLNode* right;
    AVLNode* parent;
    int height;

    AVLNode(const Key& k, const Value& v)
        : data(k, v), left(nullptr), right(nullptr), parent(nullptr), height(1)
    {
    }

    AVLNode(const Key& k, Value&& v)
        : data(k, std::move(v)), left(nullptr), right(nullptr), parent(nullptr), height(1)
    {
    }
};

template <typename Key, typename Value, typename Compare = std::less<Key>>
class AVLTree
{
public:
    using Node = AVLNode<Key, Value>;

private:
    Node* root_;
    Compare cmp_;

    int height(Node* n) const { return n ? n->height : 0; }
    void updateHeight(Node* n) { if (n) n->height = 1 + std::max(height(n->left), height(n->right)); }
    int balanceFactor(Node* n) { return height(n->right) - height(n->left); }

public:
    AVLTree() : root_(nullptr), cmp_() {}
    ~AVLTree() { clear(root_); }

    void insert(const Key& k, const Value& v) { root_ = insert(root_, k, v, nullptr); }
    Value& at(const Key& k)
    {
        Node* n = findNode(root_, k);
        if (!n) throw std::out_of_range("key not found");
        return n->data.second;
    }
    const Value& at(const Key& k) const
    {
        Node* n = findNode(root_, k);
        if (!n) throw std::out_of_range("key not found");
        return n->data.second;
    }
    bool contains(const Key& k) const { return findNode(root_, k) != nullptr; }
    void erase(const Key& k) { root_ = erase(root_, k); }

    size_t size() const
    {
        size_t cnt = 0;
        for (auto it = begin(); it != end(); ++it) ++cnt;
        return cnt;
    }

    bool empty() const { return root_ == nullptr; }
};

}

#endif
