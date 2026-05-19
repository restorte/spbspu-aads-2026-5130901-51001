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

    Node* rotateRight(Node* y)
    {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        if (T2) T2->parent = y;
        x->parent = y->parent;
        y->parent = x;

        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x)
    {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        if (T2) T2->parent = x;
        y->parent = x->parent;
        x->parent = y;

        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balance(Node* n)
    {
        if (!n) return nullptr;
        updateHeight(n);
        int bf = balanceFactor(n);
        if (bf == 2)
        {
            if (balanceFactor(n->right) < 0)
                n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        if (bf == -2)
        {
            if (balanceFactor(n->left) > 0)
                n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        return n;
    }

    Node* insert(Node* n, const Key& k, const Value& v, Node* parent)
    {
        if (!n)
        {
            Node* node = new Node(k, v);
            node->parent = parent;
            return node;
        }
        if (cmp_(k, n->data.first))
            n->left = insert(n->left, k, v, n);
        else if (cmp_(n->data.first, k))
            n->right = insert(n->right, k, v, n);
        else
            n->data.second = v;
        return balance(n);
    }

    Node* findNode(Node* n, const Key& k) const
    {
        if (!n) return nullptr;
        if (cmp_(k, n->data.first)) return findNode(n->left, k);
        if (cmp_(n->data.first, k)) return findNode(n->right, k);
        return n;
    }

    Node* minimum(Node* n) const
    {
        while (n && n->left) n = n->left;
        return n;
    }

    Node* erase(Node* n, const Key& k)
    {
        if (!n) return nullptr;
        if (cmp_(k, n->data.first))
            n->left = erase(n->left, k);
        else if (cmp_(n->data.first, k))
            n->right = erase(n->right, k);
        else
        {
            if (!n->left || !n->right)
            {
                Node* child = n->left ? n->left : n->right;
                if (child) child->parent = n->parent;
                delete n;
                return child;
            }
            else
            {
                Node* succ = minimum(n->right);
                const_cast<Key&>(n->data.first) = succ->data.first;
                n->data.second = std::move(succ->data.second);
                n->right = erase(n->right, succ->data.first);
            }
        }
        return balance(n);
    }

    void clear(Node* n)
    {
        if (!n) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }

    Node* copyNode(Node* n, Node* parent)
    {
        if (!n) return nullptr;
        Node* newNode = new Node(n->data.first, n->data.second);
        newNode->parent = parent;
        newNode->left = copyNode(n->left, newNode);
        newNode->right = copyNode(n->right, newNode);
        newNode->height = n->height;
        return newNode;
    }

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
