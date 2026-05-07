#ifndef GRAPH_COLLECTION_HPP
#define GRAPH_COLLECTION_HPP

#include <string>

#include "graph.hpp"

namespace karpenko
{

  class GraphCollection
  {
  public:
    using GraphMap = BucketHashTable < std::string, Graph, Blake2Hash, std::equal_to < std::string > >;
  private:
    GraphMap graphs_;
  public:
    bool hasGraph(const std::string& name) const
    {
      return graphs_.has(name);
    }
    Graph& getGraph(const std::string& name)
    {
      auto it = graphs_.find(name);
      if (it == graphs_.end())
      {
        throw std::runtime_error("Graph not found");
      }
      return it->second;
    }
    const Graph& getGraph(const std::string& name) const
    {
      auto it = graphs_.find(name);
      if (it == graphs_.end())
      {
        throw std::runtime_error("Graph not found");
      }
      return it->second;
    }
    void addGraph(const std::string& name, const Graph& g = Graph())
    {
      if (graphs_.has(name))
      {
        throw std::runtime_error("Graph already exists");
      }
      graphs_.add(name, g);
    }
    GraphMap& getMap()
    {
      return graphs_;
    }
    const GraphMap& getMap() const
    {
      return graphs_;
    }
  };

  GraphCollection readGraphs(const std::string& filename);

}

#endif
