#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <iostream>
#include <string>

#include "../common/hash_table.hpp"

namespace karpenko {

using Vertex = std::string;
using Weight = int;

struct edge_weights_t {
    Vector< Weight > weights;
};

using InnerEdgeMap = BucketHashTable< Vertex, edge_weights_t, Blake2Hash, std::equal_to< Vertex > >;
using OuterEdgeMap = BucketHashTable< Vertex, InnerEdgeMap, Blake2Hash, std::equal_to< Vertex > >;

struct Graph {
    BucketHashTable< Vertex, bool, Blake2Hash, std::equal_to< Vertex > > vertices_;
    OuterEdgeMap out_edges_;
    OuterEdgeMap in_edges_;

    Graph() = default;
    void swap(Graph& other) noexcept
    {
        vertices_.swap(other.vertices_);
        out_edges_.swap(other.out_edges_);
        in_edges_.swap(other.in_edges_);
    }
    Graph(const Graph& other)
        : vertices_(other.vertices_),
          out_edges_(other.out_edges_),
          in_edges_(other.in_edges_)
    {
    }
    Graph& operator=(Graph other)
    {
        swap(other);
        return *this;
    }
};

void addEdgeToMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w);
bool removeWeightFromMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w);
void printEdges(std::ostream& out, const std::string& vertex, const OuterEdgeMap& edges);
void printSortedVertices(std::ostream& out, const Graph& g);

inline void addEdgeToMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w)
{
    auto it = edges.find(from);
    if (it == edges.end())
    {
        InnerEdgeMap inner;
        edge_weights_t ew;
        ew.weights.pushBack(w);
        inner.add(to, ew);
        edges.add(from, inner);
    }
    else
    {
        InnerEdgeMap& inner = it->second;
        auto to_it = inner.find(to);
        if (to_it == inner.end())
        {
            edge_weights_t ew;
            ew.weights.pushBack(w);
            inner.add(to, ew);
        }
        else
        {
            to_it->second.weights.pushBack(w);
        }
    }
}

inline bool removeWeightFromMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w)
{
    auto it = edges.find(from);
    if (it == edges.end())
    {
        return false;
    }
    InnerEdgeMap& inner = it->second;
    auto to_it = inner.find(to);
    if (to_it == inner.end())
    {
        return false;
    }
    Vector< Weight >& weights = to_it->second.weights;
    for (std::size_t i = 0; i < weights.getSize(); ++i)
    {
        if (weights[i] == w)
        {
            weights.erase(i);
            if (weights.getSize() == 0)
            {
                inner.remove(to);
            }
            return true;
        }
    }
    return false;
}

inline void printEdges(std::ostream& out, const std::string& vertex, const OuterEdgeMap& edges)
{
    auto it = edges.find(vertex);
    if (it == edges.end())
    {
        return;
    }
    const InnerEdgeMap& inner = it->second;
    Vector< std::string > targets;
    for (auto ti = inner.begin(); ti != inner.end(); ++ti)
    {
        targets.pushBack(ti->first);
    }
    std::sort(targets.begin(), targets.end());
    for (std::size_t t = 0; t < targets.getSize(); ++t)
    {
        const std::string& target = targets[t];
        auto wi = inner.find(target);
        Vector< Weight > sorted_w = wi->second.weights;
        std::sort(sorted_w.begin(), sorted_w.end());
        out << target;
        for (std::size_t w = 0; w < sorted_w.getSize(); ++w)
        {
            out << ' ' << sorted_w[w];
        }
        out << '\n';
    }
}

inline bool hasWeightInMap(const OuterEdgeMap& edges,
                           const Vertex& from, const Vertex& to,
                           Weight w)
{
    auto it = edges.find(from);
    if (it == edges.end())
    {
        return false;
    }
    const InnerEdgeMap& inner = it->second;
    auto to_it = inner.find(to);
    if (to_it == inner.end())
    {
        return false;
    }
    const Vector< Weight >& weights = to_it->second.weights;
    for (std::size_t i = 0; i < weights.getSize(); ++i)
    {
        if (weights[i] == w)
        {
            return true;
        }
    }
    return false;
}

inline void printSortedVertices(std::ostream& out, const Graph& g)
{
    Vector< std::string > verts;
    for (auto it = g.vertices_.begin(); it != g.vertices_.end(); ++it)
    {
        verts.pushBack(it->first);
    }
    std::sort(verts.begin(), verts.end());
    for (std::size_t i = 0; i < verts.getSize(); ++i)
    {
        out << verts[i] << '\n';
    }
}

}

#endif
