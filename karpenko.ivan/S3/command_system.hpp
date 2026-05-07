#ifndef COMMAND_SYSTEM_HPP
#define COMMAND_SYSTEM_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "graph_collection.hpp"

namespace karpenko
{

  namespace
  {

    std::string* listToSortedArray(const List < std::string >& lst, std::size_t& outSize)
    {
      outSize = lst.size();
      if (outSize == 0)
      {
        return nullptr;
      }
      std::string* arr = static_cast < std::string* >(::operator new(sizeof(std::string) * outSize));
      std::size_t i = 0;
      for (auto it = lst.begin(); it != lst.end(); ++it, ++i)
      {
        new (arr + i) std::string(*it);
      }
      std::sort(arr, arr + outSize);
      return arr;
    }

    void handleGraphs(std::istream&, std::ostream& out, GraphCollection& gc)
    {
      List < std::string > names;
      for (auto it = gc.getMap().begin(); it != gc.getMap().end(); ++it)
      {
        names.push_back(it->first);
      }
      std::size_t n = 0;
      std::string* arr = listToSortedArray(names, n);
      if (n == 0)
      {
        out << '\n';
      }
      else
      {
        for (std::size_t i = 0; i < n; ++i)
        {
          out << arr[i] << '\n';
        }
      }
      for (std::size_t i = 0; i < n; ++i)
      {
        arr[i].~basic_string();
      }
      ::operator delete(arr);
    }

    void handleVertexes(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname;
      in >> gname;
      if (!in || !gc.hasGraph(gname))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      printSortedVertices(out, gc.getGraph(gname));
    }

    void handleOutbound(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname, vertex;
      in >> gname >> vertex;
      if (!in || !gc.hasGraph(gname) || !gc.getGraph(gname).vertices_.has(vertex))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      const Graph& g = gc.getGraph(gname);
      printEdges(out, vertex, g.out_edges_);
    }

    void handleInbound(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname, vertex;
      in >> gname >> vertex;
      if (!in || !gc.hasGraph(gname) || !gc.getGraph(gname).vertices_.has(vertex))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      const Graph& g = gc.getGraph(gname);
      printEdges(out, vertex, g.in_edges_);
    }

    void handleBind(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname, a, b;
      Weight w{};
      in >> gname >> a >> b >> w;
      if (!in || !gc.hasGraph(gname))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Graph graph = gc.getGraph(gname);
      graph.vertices_.add(a, true);
      graph.vertices_.add(b, true);
      addEdgeToMap(graph.out_edges_, a, b, w);
      addEdgeToMap(graph.in_edges_, b, a, w);
      gc.getGraph(gname) = graph;
    }

    void handleCut(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname, a, b;
      Weight w{};
      in >> gname >> a >> b >> w;
      if (!in || !gc.hasGraph(gname) || !gc.getGraph(gname).vertices_.has(a)
          || !gc.getGraph(gname).vertices_.has(b))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Graph graph = gc.getGraph(gname);
      if (!hasWeightInMap(graph.out_edges_, a, b, w)
          || !hasWeightInMap(graph.in_edges_, b, a, w))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      removeWeightFromMap(graph.out_edges_, a, b, w);
      removeWeightFromMap(graph.in_edges_, b, a, w);
      gc.getGraph(gname) = graph;
    }

    void handleCreate(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string gname;
      in >> gname;
      if (!in || gc.hasGraph(gname))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Graph g;
      in >> std::ws;
      if (in.peek() != std::char_traits < char >::eof())
      {
        std::size_t k;
        if (in >> k)
        {
          for (std::size_t i = 0; i < k; ++i)
          {
            std::string v;
            if (!(in >> v))
            {
              out << "<INVALID COMMAND>\n";
              return;
            }
            g.vertices_.add(v, true);
          }
          in >> std::ws;
          if (in.peek() != std::char_traits < char >::eof())
          {
            out << "<INVALID COMMAND>\n";
            return;
          }
        }
        else
        {
          out << "<INVALID COMMAND>\n";
          return;
        }
      }
      gc.addGraph(gname, g);
    }

    void handleMerge(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string newg, g1, g2;
      in >> newg >> g1 >> g2;
      if (!in || gc.hasGraph(newg) || !gc.hasGraph(g1) || !gc.hasGraph(g2))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Graph result = gc.getGraph(g1);
      const Graph& other = gc.getGraph(g2);
      for (auto it = other.vertices_.begin(); it != other.vertices_.end(); ++it)
      {
        result.vertices_.add(it->first, true);
      }
      for (auto out_it = other.out_edges_.begin(); out_it != other.out_edges_.end(); ++out_it)
      {
        const Vertex& from = out_it->first;
        const InnerEdgeMap& inner = out_it->second;
        for (auto inner_it = inner.begin(); inner_it != inner.end(); ++inner_it)
        {
          const Vertex& to = inner_it->first;
          const edge_weights_t& ew = inner_it->second;
          for (auto wi = ew.weights.cbegin(); wi != ew.weights.cend(); ++wi)
          {
            addEdgeToMap(result.out_edges_, from, to, *wi);
            addEdgeToMap(result.in_edges_, to, from, *wi);
          }
        }
      }
      gc.addGraph(newg, result);
    }

    void handleExtract(std::istream& in, std::ostream& out, GraphCollection& gc)
    {
      std::string newg, oldg;
      std::size_t k;
      in >> newg >> oldg >> k;
      if (!in || gc.hasGraph(newg) || !gc.hasGraph(oldg))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      const Graph& source = gc.getGraph(oldg);
      List < std::string > selected;
      for (std::size_t i = 0; i < k; ++i)
      {
        std::string v;
        if (!(in >> v) || !source.vertices_.has(v))
        {
          out << "<INVALID COMMAND>\n";
          return;
        }
        selected.push_back(v);
      }
      Graph result;
      for (auto it = selected.begin(); it != selected.end(); ++it)
      {
        result.vertices_.add(*it, true);
      }
      for (auto sit = selected.begin(); sit != selected.end(); ++sit)
      {
        const std::string& from = *sit;
        auto out_it = source.out_edges_.find(from);
        if (out_it == source.out_edges_.end())
        {
          continue;
        }
        const InnerEdgeMap& inner = out_it->second;
        for (auto in_it = inner.begin(); in_it != inner.end(); ++in_it)
        {
          const std::string& to = in_it->first;
          bool found = false;
          for (auto sit2 = selected.begin(); sit2 != selected.end(); ++sit2)
          {
            if (*sit2 == to)
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            continue;
          }
          const edge_weights_t& ew = in_it->second;
          for (auto wi = ew.weights.cbegin(); wi != ew.weights.cend(); ++wi)
          {
            addEdgeToMap(result.out_edges_, from, to, *wi);
            addEdgeToMap(result.in_edges_, to, from, *wi);
          }
        }
      }
      gc.addGraph(newg, result);
    }

  }

  class CommandSystem
  {
  public:
    using Handler = std::function < void(std::istream&, std::ostream&, GraphCollection&) >;
  private:
    BucketHashTable < std::string, Handler, Blake2Hash, std::equal_to < std::string > > handlers_;
  public:
    CommandSystem();
    void execute(const std::string& line, std::ostream& out, GraphCollection& gc);
  };

  CommandSystem::CommandSystem()
  {
    handlers_.add("graphs", handleGraphs);
    handlers_.add("vertexes", handleVertexes);
    handlers_.add("outbound", handleOutbound);
    handlers_.add("inbound", handleInbound);
    handlers_.add("bind", handleBind);
    handlers_.add("cut", handleCut);
    handlers_.add("create", handleCreate);
    handlers_.add("merge", handleMerge);
    handlers_.add("extract", handleExtract);
  }

  void CommandSystem::execute(const std::string& line, std::ostream& out, GraphCollection& gc)
  {
    if (line.empty() || line[0] == '#')
    {
      return;
    }
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    if (!iss)
    {
      return;
    }
    auto it = handlers_.find(cmd);
    if (it == handlers_.end())
    {
      out << "<INVALID COMMAND>\n";
    }
    else
    {
      try
      {
        it->second(iss, out, gc);
      }
      catch (...)
      {
        out << "<INVALID COMMAND>\n";
      }
    }
  }

}

#endif
