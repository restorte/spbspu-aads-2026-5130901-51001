#ifndef GRAPH_COLLECTION_HPP
#define GRAPH_COLLECTION_HPP

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "graph.hpp"

namespace karpenko {

class GraphCollection {
public:
  using GraphMap = BucketHashTable< std::string, Graph, Blake2Hash, std::equal_to< std::string > >;
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

GraphCollection readGraphs(const std::string& filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    throw std::runtime_error("Cannot open file: " + filename);
  }
  GraphCollection gc;
  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty())
    {
      continue;
    }
    std::istringstream iss(line);
    std::string graph_name;
    std::size_t edge_count;
    if (!(iss >> graph_name >> edge_count))
    {
      continue;
    }
    Graph g;
    for (std::size_t i = 0; i < edge_count; ++i)
    {
      if (!std::getline(file, line) || line.empty())
      {
        break;
      }
      std::istringstream edge_stream(line);
      std::string from;
      std::string to;
      Weight w;
      if (!(edge_stream >> from >> to >> w))
      {
        continue;
      }
      g.vertices_.add(from, true);
      g.vertices_.add(to, true);
      addEdgeToMap(g.out_edges_, from, to, w);
      addEdgeToMap(g.in_edges_, to, from, w);
    }
    gc.addGraph(graph_name, g);
  }
  return gc;
}

}

#endif
