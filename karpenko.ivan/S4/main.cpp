#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "../common/bstree.hpp"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename\n";
    return 1;
  }

  karpenko::BSTree< std::string, karpenko::BSTree< int, std::string > > dictionaries;
  std::ifstream file(argv[1]);

  if (!file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << "\n";
    return 1;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::string* tokens = nullptr;
    size_t count = 0;
    karpenko::split(line, tokens, count);

    if (count < 1 || (count - 1) % 2 != 0) {
      delete[] tokens;
      continue;
    }

    const std::string dataset_name = tokens[0];
    karpenko::BSTree< int, std::string > dict;

    for (size_t i = 1; i < count; i += 2) {
      const int key = std::stoi(tokens[i]);
      const std::string value = tokens[i + 1];
      dict.push(key, std::move(value));
    }

    dictionaries.push(dataset_name, std::move(dict));
    delete[] tokens;
  }
  file.close();

  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }

    std::string* tokens = nullptr;
    size_t count = 0;
    karpenko::split(line, tokens, count);

    if (count == 0) {
      delete[] tokens;
      continue;
    }

    try {
      const std::string cmd = tokens[0];

      if (cmd == "print") {
        if (count != 2) {
          throw std::logic_error("usage: print <dataset>");
        }
        const std::string name = tokens[1];
        auto it = dictionaries.find(name);
        if (it == dictionaries.end()) {
          throw std::logic_error("unknown dataset");
        }
        const auto& dict = it->second;
        if (dict.begin() == dict.end()) {
          std::cout << "<EMPTY>\n";
        } else {
          std::cout << name;
          for (auto cit = dict.begin(); cit != dict.end(); ++cit) {
            std::cout << " " << cit->first << " " << cit->second;
          }
          std::cout << "\n";
        }
      } else if (cmd == "complement") {
        if (count != 4) {
          throw std::logic_error("usage: complement <newdataset> <dataset1> <dataset2>");
        }
        const std::string new_name = tokens[1];
        const std::string name1 = tokens[2];
        const std::string name2 = tokens[3];
        auto it1 = dictionaries.find(name1);
        auto it2 = dictionaries.find(name2);
        if (it1 == dictionaries.end() || it2 == dictionaries.end()) {
          throw std::logic_error("unknown dataset(s)");
        }
        const auto& d1 = it1->second;
        const auto& d2 = it2->second;
        karpenko::BSTree< int, std::string > result;
        for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
          if (d2.find(cit->first) == d2.end()) {
            result.push(cit->first, cit->second);
          }
        }
        dictionaries.push(new_name, std::move(result));
      } else if (cmd == "intersect") {
        if (count != 4) {
          throw std::logic_error("usage: intersect <newdataset> <dataset1> <dataset2>");
        }
        const std::string new_name = tokens[1];
        const std::string name1 = tokens[2];
        const std::string name2 = tokens[3];
        auto it1 = dictionaries.find(name1);
        auto it2 = dictionaries.find(name2);
        if (it1 == dictionaries.end() || it2 == dictionaries.end()) {
          throw std::logic_error("unknown dataset(s)");
        }
        const auto& d1 = it1->second;
        const auto& d2 = it2->second;
        karpenko::BSTree< int, std::string > result;
        for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
          if (d2.find(cit->first) != d2.end()) {
            result.push(cit->first, cit->second);
          }
        }
        dictionaries.push(new_name, std::move(result));
      } else if (cmd == "union") {
        if (count != 4) {
          throw std::logic_error("usage: union <newdataset> <dataset1> <dataset2>");
        }
        const std::string new_name = tokens[1];
        const std::string name1 = tokens[2];
        const std::string name2 = tokens[3];
        auto it1 = dictionaries.find(name1);
        auto it2 = dictionaries.find(name2);
        if (it1 == dictionaries.end() || it2 == dictionaries.end()) {
          throw std::logic_error("unknown dataset(s)");
        }
        const auto& d1 = it1->second;
        const auto& d2 = it2->second;
        karpenko::BSTree< int, std::string > result;
        for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
          result.push(cit->first, cit->second);
        }
        for (auto cit = d2.begin(); cit != d2.end(); ++cit) {
          if (result.find(cit->first) == result.end()) {
            result.push(cit->first, cit->second);
          }
        }
        dictionaries.push(new_name, std::move(result));
      } else {
        throw std::logic_error("unknown command");
      }
    } catch (const std::exception&) {
      std::cout << "<INVALID COMMAND>\n";
    }
    delete[] tokens;
  }

  return 0;
}
