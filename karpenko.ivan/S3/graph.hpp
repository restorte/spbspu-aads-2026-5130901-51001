#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <iostream>
#include <string>

#include "hash_table.hpp"
#include "list.hpp"

namespace karpenko
{

  using Vertex = std::string;
  using Weight = int;

  struct edge_weights_t
  {
    List < Weight > weights;
  };

  using InnerEdgeMap = BucketHashTable < Vertex, edge_weights_t, Blake2Hash, std::equal_to < Vertex > >;
  using OuterEdgeMap = BucketHashTable < Vertex, InnerEdgeMap, Blake2Hash, std::equal_to < Vertex > >;

  struct Graph
  {
    BucketHashTable < Vertex, bool, Blake2Hash, std::equal_to < Vertex > > vertices_;
    OuterEdgeMap out_edges_;
    OuterEdgeMap in_edges_;

    Graph() = default;

    Graph(const Graph& other)
      : vertices_(other.vertices_), out_edges_(other.out_edges_), in_edges_(other.in_edges_)
    {
    }

    Graph& operator=(Graph other)
    {
      swap(other);
      return *this;
    }

    void swap(Graph& other) noexcept
    {
      vertices_.swap(other.vertices_);
      out_edges_.swap(other.out_edges_);
      in_edges_.swap(other.in_edges_);
    }
  };

  void addEdgeToMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w);
  bool removeWeightFromMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w);
  void printEdges(std::ostream& out, const std::string& vertex, const OuterEdgeMap& edges);
  void printSortedVertices(std::ostream& out, const Graph& g);
  bool hasWeightInMap(const OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w);

}

#endif
