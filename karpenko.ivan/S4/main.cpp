#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "../common/bstree.hpp"
namespace karpenko {
    using DictValue = BSTree<int, std::string>;
    using DictStorage = BSTree<std::string, DictValue>;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename\n";
        return 1;
    }

    using namespace karpenko;

    DictStorage dictionaries;
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string* tokens = nullptr;
        size_t count = 0;
        split(line, tokens, count);
        if (count < 1 || (count - 1) % 2 != 0) {
            delete[] tokens;
            continue;
        }

        std::string datasetName = tokens[0];
        DictValue dict;
        for (size_t i = 1; i < count; i += 2) {
            int key = std::stoi(tokens[i]);
            std::string value = tokens[i + 1];
            dict.push(key, std::move(value));
        }
        dictionaries.push(datasetName, std::move(dict));
        delete[] tokens;
    }
    file.close();

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::string* tokens = nullptr;
        size_t count = 0;
        split(line, tokens, count);
        if (count == 0) {
            delete[] tokens;
            continue;
        }

        try {
            std::string cmd = tokens[0];
            if (cmd == "print") {
                if (count != 2) throw std::logic_error("usage: print <dataset>");
                std::string name = tokens[1];
                auto it = dictionaries.find(name);
                if (it == dictionaries.end()) throw std::logic_error("unknown dataset");
                const DictValue& dict = it->second;
                if (dict.begin() == dict.end()) {
                    std::cout << "<EMPTY>\n";
                } else {
                    std::cout << name;
                    for (auto cit = dict.begin(); cit != dict.end(); ++cit)
                        std::cout << " " << cit->first << " " << cit->second;
                    std::cout << "\n";
                }
            } else if (cmd == "complement") {
                if (count != 4) throw std::logic_error("usage: complement <newdataset> <dataset1> <dataset2>");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (dictionaries.find(newName) != dictionaries.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = dictionaries.find(name1), it2 = dictionaries.find(name2);
                if (it1 == dictionaries.end() || it2 == dictionaries.end())
                    throw std::logic_error("unknown dataset(s)");
                const DictValue& d1 = it1->second, & d2 = it2->second;
                DictValue result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    if (d2.find(cit->first) == d2.end())
                        result.push(cit->first, cit->second);
                dictionaries.push(newName, std::move(result));
            } else if (cmd == "intersect") {
                if (count != 4) throw std::logic_error("usage: intersect <newdataset> <dataset1> <dataset2>");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (dictionaries.find(newName) != dictionaries.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = dictionaries.find(name1), it2 = dictionaries.find(name2);
                if (it1 == dictionaries.end() || it2 == dictionaries.end())
                    throw std::logic_error("unknown dataset(s)");
                const DictValue& d1 = it1->second, & d2 = it2->second;
                DictValue result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    if (d2.find(cit->first) != d2.end())
                        result.push(cit->first, cit->second);
                dictionaries.push(newName, std::move(result));
            } else if (cmd == "union") {
                if (count != 4) throw std::logic_error("usage: union <newdataset> <dataset1> <dataset2>");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (dictionaries.find(newName) != dictionaries.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = dictionaries.find(name1), it2 = dictionaries.find(name2);
                if (it1 == dictionaries.end() || it2 == dictionaries.end())
                    throw std::logic_error("unknown dataset(s)");
                const DictValue& d1 = it1->second, & d2 = it2->second;
                DictValue result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    result.push(cit->first, cit->second);
                for (auto cit = d2.begin(); cit != d2.end(); ++cit)
                    if (result.find(cit->first) == result.end())
                        result.push(cit->first, cit->second);
                dictionaries.push(newName, std::move(result));
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
