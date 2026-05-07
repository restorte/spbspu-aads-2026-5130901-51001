#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include "../common/hash_table.hpp"

namespace
{
  using HashTable = karpenko::BucketHashTable < std::string, int, karpenko::Blake2Hash, std::equal_to < std::string > >;

  struct ConstantHash
  {
    std::size_t operator()(const std::string&) const
    {
      return 0;
    }
  };
  using CollisionTable = karpenko::BucketHashTable < std::string, int, ConstantHash, std::equal_to < std::string > >;
}

BOOST_AUTO_TEST_CASE(blake2_hash_is_stable_and_distinguishes_keys)
{
  const karpenko::Blake2Hash hash;
  BOOST_TEST(hash("same") == hash("same"));
  BOOST_TEST(hash("same") != hash("other"));

  const std::string s1 = "abc";
  BOOST_TEST(hash(s1) == hash("abc"));
}

BOOST_AUTO_TEST_CASE(hash_table_basic_operations)
{
  HashTable table(23);
  table.add("one", 1);
  BOOST_TEST(table.has("one"));
  BOOST_TEST(!table.has("missing"));
  auto it = table.find("one");
  BOOST_TEST(it->second == 1);

  table.add("one", 11);
  BOOST_TEST(table.find("one")->second == 11);

  bool removed = table.remove("one");
  BOOST_TEST(removed);
  BOOST_TEST(!table.has("one"));
  BOOST_TEST(!table.remove("one"));
}

BOOST_AUTO_TEST_CASE(hash_table_tombstone_iterator_and_rehash)
{
  CollisionTable table(11);
  table.add("a", 1);
  table.add("b", 2);
  table.add("c", 3);
  table.remove("b");
  BOOST_TEST(table.has("c"));
  table.add("d", 4);
  BOOST_TEST(table.has("d"));
  BOOST_TEST(table.find("d")->second == 4);

  std::size_t visited = 0;
  int sum = 0;
  for (auto it = table.begin(); it != table.end(); ++it)
  {
    BOOST_TEST(it->first != "b");
    ++visited;
    sum += it->second;
  }
  BOOST_TEST(visited == 3);
  BOOST_TEST(sum == 8);

  const CollisionTable& constTable = table;
  std::size_t constVisited = 0;
  for (auto it = constTable.begin(); it != constTable.end(); ++it)
  {
    BOOST_TEST(it->first != "b");
    ++constVisited;
  }
  BOOST_TEST(constVisited == visited);

  table.rehash(17, 4, 8);
  BOOST_TEST(table.has("a"));
  BOOST_TEST(table.has("c"));
  BOOST_TEST(table.has("d"));
  BOOST_TEST(!table.has("b"));
  table.add("e", 5);
  BOOST_TEST(table.has("e"));
}

BOOST_AUTO_TEST_CASE(hash_table_full_table_throws)
{
  CollisionTable table(1, 1, 1);
  table.add("a", 1);
  table.add("b", 2);
  BOOST_CHECK_THROW(table.add("c", 3), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(hash_table_copy_move_and_clear)
{
  CollisionTable original(17);
  original.add("a", 1);
  original.add("b", 2);

  CollisionTable copy(original);
  copy.add("a", 10);
  BOOST_TEST(original.find("a")->second == 1);
  BOOST_TEST(copy.find("a")->second == 10);

  CollisionTable assigned(17);
  assigned.add("x", 9);
  assigned = original;
  assigned.add("b", 20);
  BOOST_TEST(original.find("b")->second == 2);
  BOOST_TEST(assigned.find("b")->second == 20);

  CollisionTable moved(std::move(copy));
  BOOST_TEST(moved.has("a"));
  BOOST_TEST(moved.has("b"));

  CollisionTable moveAssigned(17);
  moveAssigned = std::move(moved);
  BOOST_TEST(moveAssigned.has("a"));
  BOOST_TEST(moveAssigned.has("b"));
}
