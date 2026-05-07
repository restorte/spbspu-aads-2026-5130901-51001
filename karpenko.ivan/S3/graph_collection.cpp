#include "graph_collection.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace karpenko
{

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
        std::string from, to;
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
