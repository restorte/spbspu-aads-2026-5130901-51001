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

void handleGraphs(std::istream&, std::ostream& out, GraphCollection& gc)
{
  Vector<std::string> names;
  for (auto it = gc.getMap().begin(); it != gc.getMap().end(); ++it)
  {
    names.pushBack(it->first);
  }
  std::sort(names.begin(), names.end());
  for (std::size_t i = 0; i < names.getSize(); ++i)
  {
    out << names[i] << '\n';
  }
  if (names.getSize() == 0)
  {
    out << '\n';
  }
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
  std::string gname;
  std::string vertex;
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
  std::string gname;
  std::string vertex;
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
  std::string gname;
  std::string a;
  std::string b;
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
  gc.getGraph(gname).swap(graph);
}

void handleCut(std::istream& in, std::ostream& out, GraphCollection& gc)
{
  std::string gname;
  std::string a;
  std::string b;
  Weight w{};
  in >> gname >> a >> b >> w;
  if (!in || !gc.hasGraph(gname)
      || !gc.getGraph(gname).vertices_.has(a)
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

  gc.getGraph(gname).swap(graph);
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
  }
  gc.addGraph(gname, g);
}

void handleMerge(std::istream& in, std::ostream& out, GraphCollection& gc)
{
  std::string newg;
  std::string g1;
  std::string g2;
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
      for (std::size_t i = 0; i < ew.weights.getSize(); ++i)
      {
        addEdgeToMap(result.out_edges_, from, to, ew.weights[i]);
        addEdgeToMap(result.in_edges_, to, from, ew.weights[i]);
      }
    }
  }
  gc.addGraph(newg, result);
}

void handleExtract(std::istream& in, std::ostream& out, GraphCollection& gc)
{
  std::string newg;
  std::string oldg;
  std::size_t k;
  in >> newg >> oldg >> k;
  if (!in || gc.hasGraph(newg) || !gc.hasGraph(oldg))
  {
    out << "<INVALID COMMAND>\n";
    return;
  }
  const Graph& source = gc.getGraph(oldg);
  Vector<std::string> selected;
  for (std::size_t i = 0; i < k; ++i)
  {
    std::string v;
    if (!(in >> v) || !source.vertices_.has(v))
    {
      out << "<INVALID COMMAND>\n";
      return;
    }
    selected.pushBack(v);
  }
  Graph result;
  for (std::size_t i = 0; i < selected.getSize(); ++i)
  {
    result.vertices_.add(selected[i], true);
  }
  for (std::size_t i = 0; i < selected.getSize(); ++i)
  {
    const std::string& from = selected[i];
    auto out_it = source.out_edges_.find(from);
    if (out_it == source.out_edges_.end())
    {
      continue;
    }
    const InnerEdgeMap& inner = out_it->second;
    for (auto in_it = inner.begin(); in_it != inner.end(); ++in_it)
    {
      const std::string& to = in_it->first;
      bool to_selected = false;
      for (std::size_t j = 0; j < selected.getSize(); ++j)
      {
        if (selected[j] == to)
        {
          to_selected = true;
          break;
        }
      }
      if (!to_selected)
      {
        continue;
      }
      const edge_weights_t& ew = in_it->second;
      for (std::size_t wi = 0; wi < ew.weights.getSize(); ++wi)
      {
        addEdgeToMap(result.out_edges_, from, to, ew.weights[wi]);
        addEdgeToMap(result.in_edges_, to, from, ew.weights[wi]);
      }
    }
  }
  gc.addGraph(newg, result);
}

}

class CommandSystem
{
public:
  using Handler = std::function< void( std::istream&, std::ostream&, GraphCollection& ) >;

private:
  BucketHashTable< std::string, Handler, Blake2Hash, std::equal_to< std::string > > handlers_;

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
