#define BOOST_TEST_MODULE test_bstree
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <string>
#include "../common/bstree.hpp"

namespace karpenko
{

template < class Tree >
std::string keys(const Tree& tree)
{
  std::string result;
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    result += std::to_string(it->first);
  }
  return result;
}

}

BOOST_AUTO_TEST_CASE(empty_tree)
{
  const karpenko::BSTree< int, std::string > tree;
  BOOST_TEST((tree.begin() == tree.end()));
  BOOST_TEST(tree.height() == 0);
}

BOOST_AUTO_TEST_CASE(insert_search_and_iterators)
{
  karpenko::BSTree< int, std::string > tree;
  tree.push(2, "two");
  BOOST_TEST(tree.get(2) == "two");
  tree.push(1, "one");
  tree.push(3, "three");
  BOOST_TEST(karpenko::keys(tree) == "123");

  tree.push(2, "changed");
  BOOST_TEST(tree.get(2) == "changed");

  BOOST_TEST(tree.find(1) != tree.end());
  BOOST_TEST(tree.find(4) == tree.end());
  BOOST_CHECK_THROW(tree.get(4), std::out_of_range);

  auto it = tree.begin();
  BOOST_TEST(it->first == 1);
  BOOST_TEST((++it)->first == 2);
  BOOST_TEST((it++)->first == 2);
  BOOST_TEST(it->first == 3);

  auto end = tree.end();
  BOOST_TEST(++it == end);

  const auto& ctree = tree;
  auto cit = ctree.begin();
  BOOST_TEST(cit->first == 1);
  BOOST_TEST(cit->second == "one");
}

BOOST_AUTO_TEST_CASE(move_semantics)
{
  karpenko::BSTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  karpenko::BSTree< int, std::string > moved(std::move(tree));
  BOOST_TEST((tree.begin() == tree.end()));
  BOOST_TEST(moved.begin() != moved.end());
  BOOST_TEST(karpenko::keys(moved) == "123");

  karpenko::BSTree< int, std::string > assigned;
  assigned = std::move(moved);
  BOOST_TEST((moved.begin() == moved.end()));
  BOOST_TEST(karpenko::keys(assigned) == "123");
}

BOOST_AUTO_TEST_CASE(drop_and_height)
{
  karpenko::BSTree< int, std::string > tree;
  tree.push(4, "four");
  tree.push(2, "two");
  tree.push(6, "six");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(5, "five");
  tree.push(7, "seven");

  BOOST_TEST(tree.height() == 3);

  BOOST_TEST(tree.drop(1) == "one");
  BOOST_TEST(karpenko::keys(tree) == "234567");
  tree.push(1, "one");

  tree.drop(2);
  BOOST_TEST(karpenko::keys(tree) == "134567");

  tree.drop(6);
  BOOST_TEST(karpenko::keys(tree) == "13457");

  tree.drop(7);
  BOOST_TEST(karpenko::keys(tree) == "1345");

  tree.drop(5);
  BOOST_TEST(karpenko::keys(tree) == "134");

  tree.drop(4);
  BOOST_TEST(karpenko::keys(tree) == "13");

  karpenko::BSTree< int, std::string > single;
  single.push(10, "ten");
  BOOST_TEST(single.height() == 1);

  single.push(9, "nine");
  single.push(8, "eight");
  auto it = single.begin();
  BOOST_TEST(single.height(it) == 1);
  BOOST_TEST(single.height(single.end()) == 0);
}

BOOST_AUTO_TEST_CASE(rotations)
{
  {
    karpenko::BSTree< int, std::string > t;
    t.push(2, "two");
    t.push(1, "one");
    t.push(4, "four");
    t.push(3, "three");

    auto it = t.find(4);
    auto raised = t.rotateLeft(it);
    BOOST_TEST(raised->first == 4);
    BOOST_TEST(karpenko::keys(t) == "1234");

    auto stable = t.begin();
    ++stable;
    BOOST_TEST(stable->first == 2);
    BOOST_TEST(stable->second == "two");
  }

  {
    karpenko::BSTree< int, std::string > t;
    t.push(3, "three");
    t.push(1, "one");
    t.push(4, "four");
    t.push(2, "two");

    auto raised = t.rotateRight(t.begin());
    BOOST_TEST(raised->first == 1);
    BOOST_TEST(karpenko::keys(t) == "1234");
  }

  {
    karpenko::BSTree< int, std::string > t;
    t.push(1, "one");
    t.push(4, "four");
    t.push(2, "two");
    t.push(3, "three");

    auto inner = t.begin();
    ++inner;
    auto raised = t.rotateLargeLeft(inner);
    BOOST_TEST(raised->first == 2);
    BOOST_TEST(karpenko::keys(t) == "1234");
  }

  {
    karpenko::BSTree< int, std::string > t;
    t.push(4, "four");
    t.push(1, "one");
    t.push(3, "three");
    t.push(2, "two");

    auto inner = t.begin();
    ++inner;
    ++inner;
    auto raised = t.rotateLargeRight(inner);
    BOOST_TEST(raised->first == 3);
    BOOST_TEST(karpenko::keys(t) == "1234");
  }

  {
    karpenko::BSTree< int, std::string > t;
    t.push(1, "one");
    t.push(2, "two");
    BOOST_CHECK_THROW(t.rotateLeft(t.begin()), std::logic_error);
    BOOST_CHECK_THROW(t.rotateRight(t.begin()), std::logic_error);
  }
}
