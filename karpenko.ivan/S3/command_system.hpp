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
  Vector< std::string > names;
  for (auto it = gc.getMap().begin(); it != gc.getMap().end(); ++it)
  {
    names.pushBack(it->first);
  }
  std::sort(names.begin(), names.end());
  for (std::size_t i = 0; i < names.getSize(); ++i)
  {
    out << names[i] << '\n';
  }
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
