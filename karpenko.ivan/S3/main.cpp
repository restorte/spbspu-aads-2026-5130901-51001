#include <iostream>
#include "command_system.hpp"

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }
  try
  {
    karpenko::GraphCollection graphs = karpenko::readGraphs(argv[1]);
    karpenko::CommandSystem cs;
    std::string line;
    while (std::getline(std::cin, line))
    {
      cs.execute(line, std::cout, graphs);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }
  return 0;
}
