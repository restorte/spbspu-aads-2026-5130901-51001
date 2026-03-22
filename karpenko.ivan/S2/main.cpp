#include "evaluator.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  using namespace karpenko;

  std::istream* input = &std::cin;
  std::ifstream file;

  if (argc >= 2) {
    file.open(argv[1]);
    if (!file) {
      std::cerr << "Error: cannot open file " << argv[1] << std::endl;
      return 1;
    }
    input = &file;
  }

  Stack<long long> results;
  std::string line;

  while (std::getline(*input, line)) {
    if (line.empty()) {
      continue;
    }

    try {
      long long res = evaluator::evaluate(line);
      results.push(res);
    } catch (const std::exception& e) {
      std::cerr << "Error evaluating expression: " << e.what() << std::endl;
      return 1;
    }
  }

  if (results.empty()) {
    std::cout << '\n';
    return 0;
  }

  bool first = true;
  while (!results.empty()) {
    if (!first) {
      std::cout << ' ';
    }
    std::cout << results.top();
    results.pop();
    first = false;
  }
  std::cout << '\n';

  return 0;
}
