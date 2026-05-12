#include <boost/test/unit_test.hpp>
#include <sstream>
#include <string>
#include "../common/bstree.hpp"

namespace karpenko
{

using Dictionary = BSTree< int, std::string >;
using DictStorage = BSTree< std::string, Dictionary >;

Dictionary make_first()
{
  Dictionary d;
  d.push(1, "name");
  d.push(2, "surname");
  return d;
}

Dictionary make_second()
{
  Dictionary d;
  d.push(4, "mouse");
  d.push(1, "name");
  d.push(2, "keyboard");
  return d;
}

std::string dispatch(const std::string& line, DictStorage& storage)
{
  std::string* tokens = nullptr;
  size_t count = 0;
  split(line, tokens, count);
  std::ostringstream out;

  try {
    if (count == 0) {
      delete[] tokens;
      return "";
    }

    const std::string cmd = tokens[0];

    if (cmd == "print") {
      if (count != 2) {
        throw std::logic_error("usage: print <dataset>");
      }
      const std::string name = tokens[1];
      auto it = storage.find(name);
      if (it == storage.end()) {
        throw std::logic_error("unknown dataset");
      }
      const Dictionary& dict = it->second;
      if (dict.begin() == dict.end()) {
        out << "<EMPTY>\n";
      } else {
        out << name;
        for (auto cit = dict.begin(); cit != dict.end(); ++cit) {
          out << " " << cit->first << " " << cit->second;
        }
        out << "\n";
      }
    } else if (cmd == "complement") {
      if (count != 4) {
        throw std::logic_error("usage: complement ...");
      }
      const std::string new_name = tokens[1];
      const std::string name1 = tokens[2];
      const std::string name2 = tokens[3];
      auto it1 = storage.find(name1);
      auto it2 = storage.find(name2);
      if (it1 == storage.end() || it2 == storage.end()) {
        throw std::logic_error("unknown dataset");
      }
      const Dictionary& d1 = it1->second;
      const Dictionary& d2 = it2->second;
      Dictionary result;
      for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
        if (d2.find(cit->first) == d2.end()) {
          result.push(cit->first, cit->second);
        }
      }
      storage.push(new_name, std::move(result));
    } else if (cmd == "intersect") {
      if (count != 4) {
        throw std::logic_error("usage: intersect ...");
      }
      const std::string new_name = tokens[1];
      const std::string name1 = tokens[2];
      const std::string name2 = tokens[3];
      auto it1 = storage.find(name1);
      auto it2 = storage.find(name2);
      if (it1 == storage.end() || it2 == storage.end()) {
        throw std::logic_error("unknown dataset");
      }
      const Dictionary& d1 = it1->second;
      const Dictionary& d2 = it2->second;
      Dictionary result;
      for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
        if (d2.find(cit->first) != d2.end()) {
          result.push(cit->first, cit->second);
        }
      }
      storage.push(new_name, std::move(result));
    } else if (cmd == "union") {
      if (count != 4) {
        throw std::logic_error("usage: union ...");
      }
      const std::string new_name = tokens[1];
      const std::string name1 = tokens[2];
      const std::string name2 = tokens[3];
      auto it1 = storage.find(name1);
      auto it2 = storage.find(name2);
      if (it1 == storage.end() || it2 == storage.end()) {
        throw std::logic_error("unknown dataset");
      }
      const Dictionary& d1 = it1->second;
      const Dictionary& d2 = it2->second;
      Dictionary result;
      for (auto cit = d1.begin(); cit != d1.end(); ++cit) {
        result.push(cit->first, cit->second);
      }
      for (auto cit = d2.begin(); cit != d2.end(); ++cit) {
        if (result.find(cit->first) == result.end()) {
          result.push(cit->first, cit->second);
        }
      }
      storage.push(new_name, std::move(result));
    } else {
      throw std::logic_error("unknown command");
    }
  } catch (const std::exception&) {
    out << "<INVALID COMMAND>\n";
  }

  delete[] tokens;
  return out.str();
}

}

BOOST_AUTO_TEST_CASE(print_and_invalid_commands)
{
  karpenko::DictStorage storage;
  storage.push("first", karpenko::make_first());
  storage.push("empty", karpenko::Dictionary{});

  BOOST_TEST(karpenko::dispatch("print first", storage) == "first 1 name 2 surname\n");
  BOOST_TEST(karpenko::dispatch("print empty", storage) == "<EMPTY>\n");
  BOOST_TEST(karpenko::dispatch("print missing", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(karpenko::dispatch("print first extra", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(karpenko::dispatch("unknown", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(karpenko::dispatch("print", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(karpenko::dispatch("", storage) == "");
}

BOOST_AUTO_TEST_CASE(set_operations)
{
  karpenko::DictStorage storage;
  storage.push("first", karpenko::make_first());
  storage.push("second", karpenko::make_second());

  BOOST_TEST(karpenko::dispatch("complement third second first", storage) == "");
  BOOST_TEST(karpenko::dispatch("print third", storage) == "third 4 mouse\n");
  BOOST_TEST(karpenko::dispatch("complement third second first", storage) == "");
  BOOST_TEST(karpenko::dispatch("print third", storage) == "third 4 mouse\n");
  BOOST_TEST(karpenko::dispatch("complement broken second", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(karpenko::dispatch("complement broken missing first", storage) == "<INVALID COMMAND>\n");

  storage.push("empty", karpenko::Dictionary{});
  BOOST_TEST(karpenko::dispatch("complement empty first second", storage) == "");
  BOOST_TEST(karpenko::dispatch("print empty", storage) == "<EMPTY>\n");

  BOOST_TEST(karpenko::dispatch("intersect fourth first second", storage) == "");
  BOOST_TEST(karpenko::dispatch("print fourth", storage) == "fourth 1 name 2 surname\n");
  BOOST_TEST(karpenko::dispatch("intersect bad first", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(karpenko::dispatch("union fifth first second", storage) == "");
  BOOST_TEST(karpenko::dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
  BOOST_TEST(karpenko::dispatch("union bad first second extra", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(karpenko::dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(set_operation_overwrite_aliases)
{
  {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::make_first());
    storage.push("second", karpenko::make_second());
    BOOST_TEST(karpenko::dispatch("complement second second first", storage) == "");
    BOOST_TEST(karpenko::dispatch("print second", storage) == "second 4 mouse\n");
  }
  {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::make_first());
    storage.push("second", karpenko::make_second());
    BOOST_TEST(karpenko::dispatch("intersect second second first", storage) == "");
    BOOST_TEST(karpenko::dispatch("print second", storage) == "second 1 name 2 keyboard\n");
  }
  {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::make_first());
    karpenko::Dictionary second_plus;
    second_plus.push(4, "mouse");
    second_plus.push(1, "name");
    second_plus.push(2, "keyboard");
    second_plus.push(3, "machine");
    storage.push("second", std::move(second_plus));
    BOOST_TEST(karpenko::dispatch("union first second first", storage) == "");
    BOOST_TEST(karpenko::dispatch("print first", storage) == "first 1 name 2 keyboard 3 machine 4 mouse\n");
  }
  {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::make_first());
    storage.push("third", karpenko::make_first());
    storage.push("fourth", karpenko::Dictionary{});
    BOOST_TEST(karpenko::dispatch("intersect first third fourth", storage) == "");
    BOOST_TEST(karpenko::dispatch("print first", storage) == "<EMPTY>\n");
  }
}

BOOST_AUTO_TEST_CASE(acceptance_scenario)
{
  karpenko::DictStorage storage;
  karpenko::Dictionary first;
  first.push(1, "name");
  first.push(2, "surname");
  storage.push("first", std::move(first));

  karpenko::Dictionary second;
  second.push(4, "mouse");
  second.push(1, "name");
  second.push(2, "keyboard");
  storage.push("second", std::move(second));

  std::string output;
  output += karpenko::dispatch("print second", storage);
  output += karpenko::dispatch("complement third second first", storage);
  output += karpenko::dispatch("print third", storage);
  output += karpenko::dispatch("intersect fourth first second", storage);
  output += karpenko::dispatch("print fourth", storage);
  output += karpenko::dispatch("union fifth first second", storage);
  output += karpenko::dispatch("print fifth", storage);

  const std::string expected =
    "second 1 name 2 keyboard 4 mouse\n"
    "third 4 mouse\n"
    "fourth 1 name 2 surname\n"
    "fifth 1 name 2 surname 4 mouse\n";
  BOOST_TEST(output == expected);
}
