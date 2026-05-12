#define BOOST_TEST_MODULE test_commands
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>
#include "../common/bstree.hpp"

namespace karpenko {
    using Dictionary = BSTree<int, std::string>;
    using DictStorage = BSTree<std::string, Dictionary>;

    Dictionary makeFirst() {
        Dictionary d;
        d.push(1, "name");
        d.push(2, "surname");
        return d;
    }

    Dictionary makeSecond() {
        Dictionary d;
        d.push(4, "mouse");
        d.push(1, "name");
        d.push(2, "keyboard");
        return d;
    }

    std::string dispatch(const std::string& line, DictStorage& storage) {
        std::string* tokens = nullptr;
        size_t count = 0;
        split(line, tokens, count);
        std::ostringstream out;

        try {
            if (count == 0) {
                delete[] tokens;
                return "";
            }
            std::string cmd = tokens[0];
            if (cmd == "print") {
                if (count != 2) throw std::logic_error("usage: print <dataset>");
                std::string name = tokens[1];
                auto it = storage.find(name);
                if (it == storage.end()) throw std::logic_error("unknown dataset");
                const Dictionary& dict = it->second;
                if (dict.begin() == dict.end()) {
                    out << "<EMPTY>\n";
                } else {
                    out << name;
                    for (auto cit = dict.begin(); cit != dict.end(); ++cit)
                        out << " " << cit->first << " " << cit->second;
                    out << "\n";
                }
            } else if (cmd == "complement") {
                if (count != 4) throw std::logic_error("usage: complement ...");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (storage.find(newName) != storage.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = storage.find(name1), it2 = storage.find(name2);
                if (it1 == storage.end() || it2 == storage.end())
                    throw std::logic_error("unknown dataset");
                const Dictionary& d1 = it1->second, & d2 = it2->second;
                Dictionary result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    if (d2.find(cit->first) == d2.end())
                        result.push(cit->first, cit->second);
                storage.push(newName, std::move(result));
            } else if (cmd == "intersect") {
                if (count != 4) throw std::logic_error("usage: intersect ...");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (storage.find(newName) != storage.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = storage.find(name1), it2 = storage.find(name2);
                if (it1 == storage.end() || it2 == storage.end())
                    throw std::logic_error("unknown dataset");
                const Dictionary& d1 = it1->second, & d2 = it2->second;
                Dictionary result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    if (d2.find(cit->first) != d2.end())
                        result.push(cit->first, cit->second);
                storage.push(newName, std::move(result));
            } else if (cmd == "union") {
                if (count != 4) throw std::logic_error("usage: union ...");
                std::string newName = tokens[1], name1 = tokens[2], name2 = tokens[3];
                if (storage.find(newName) != storage.end())
                    throw std::logic_error("dataset already exists");
                auto it1 = storage.find(name1), it2 = storage.find(name2);
                if (it1 == storage.end() || it2 == storage.end())
                    throw std::logic_error("unknown dataset");
                const Dictionary& d1 = it1->second, & d2 = it2->second;
                Dictionary result;
                for (auto cit = d1.begin(); cit != d1.end(); ++cit)
                    result.push(cit->first, cit->second);
                for (auto cit = d2.begin(); cit != d2.end(); ++cit)
                    if (result.find(cit->first) == result.end())
                        result.push(cit->first, cit->second);
                storage.push(newName, std::move(result));
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

BOOST_AUTO_TEST_CASE(print_and_invalid_commands) {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::makeFirst());
    storage.push("empty", karpenko::Dictionary{});

    BOOST_TEST(karpenko::dispatch("print first", storage) == "first 1 name 2 surname\n");
    BOOST_TEST(karpenko::dispatch("print empty", storage) == "<EMPTY>\n");
    BOOST_TEST(karpenko::dispatch("print missing", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("print first extra", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("unknown", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("print", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("", storage) == "");
}

BOOST_AUTO_TEST_CASE(set_operations) {
    karpenko::DictStorage storage;
    storage.push("first", karpenko::makeFirst());
    storage.push("second", karpenko::makeSecond());

    BOOST_TEST(karpenko::dispatch("complement third second first", storage) == "");
    BOOST_TEST(karpenko::dispatch("print third", storage) == "third 4 mouse\n");
    BOOST_TEST(karpenko::dispatch("complement third second first", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("complement broken second", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("complement broken missing first", storage) == "<INVALID COMMAND>\n");

    storage.push("empty", karpenko::Dictionary{});
    BOOST_TEST(karpenko::dispatch("complement empty first second", storage) == "<INVALID COMMAND>\n");
    BOOST_TEST(karpenko::dispatch("print empty", storage) == "<EMPTY>\n");

    BOOST_TEST(karpenko::dispatch("intersect fourth first second", storage) == "");
    BOOST_TEST(karpenko::dispatch("print fourth", storage) == "fourth 1 name 2 surname\n");
    BOOST_TEST(karpenko::dispatch("intersect bad first", storage) == "<INVALID COMMAND>\n");

    BOOST_TEST(karpenko::dispatch("union fifth first second", storage) == "");
    BOOST_TEST(karpenko::dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
    BOOST_TEST(karpenko::dispatch("union bad first second extra", storage) == "<INVALID COMMAND>\n");

    BOOST_TEST(karpenko::dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(set_operation_overwrite_aliases) {
    {
        karpenko::DictStorage storage;
        storage.push("first", karpenko::makeFirst());
        storage.push("second", karpenko::makeSecond());
        BOOST_TEST(karpenko::dispatch("complement second second first", storage) == "<INVALID COMMAND>\n");
        BOOST_TEST(karpenko::dispatch("print second", storage) == "second 1 name 2 keyboard 4 mouse\n");
    }
    {
        karpenko::DictStorage storage;
        storage.push("first", karpenko::makeFirst());
        storage.push("second", karpenko::makeSecond());
        BOOST_TEST(karpenko::dispatch("intersect second second first", storage) == "<INVALID COMMAND>\n");
        BOOST_TEST(karpenko::dispatch("print second", storage) == "second 1 name 2 keyboard 4 mouse\n");
    }
    {
        karpenko::DictStorage storage;
        storage.push("first", karpenko::makeFirst());
        karpenko::Dictionary secondPlus;
        secondPlus.push(4, "mouse");
        secondPlus.push(1, "name");
        secondPlus.push(2, "keyboard");
        secondPlus.push(3, "machine");
        storage.push("second", std::move(secondPlus));
        BOOST_TEST(karpenko::dispatch("union first second first", storage) == "<INVALID COMMAND>\n");
    }
    {
        karpenko::DictStorage storage;
        storage.push("first", karpenko::makeFirst());
        storage.push("third", karpenko::makeFirst());
        storage.push("fourth", karpenko::Dictionary{});
        BOOST_TEST(karpenko::dispatch("intersect first third fourth", storage) == "<INVALID COMMAND>\n");
        BOOST_TEST(karpenko::dispatch("print first", storage) == "first 1 name 2 surname\n");
    }
}

BOOST_AUTO_TEST_CASE(acceptance_scenario) {
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
