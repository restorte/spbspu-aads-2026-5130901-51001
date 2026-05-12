#include <boost/test/unit_test.hpp>
#include <string>
#include "../common/bstree.hpp"

BOOST_AUTO_TEST_CASE(split_function)
{
  std::string* tokens = nullptr;
  size_t count = 0;

  karpenko::split("  print first ", tokens, count);
  BOOST_REQUIRE_EQUAL(count, 2);
  BOOST_TEST(tokens[0] == "print");
  BOOST_TEST(tokens[1] == "first");
  delete[] tokens;

  karpenko::split("   ", tokens, count);
  BOOST_TEST(count == 0);
  delete[] tokens;

  karpenko::split("hello", tokens, count);
  BOOST_REQUIRE_EQUAL(count, 1);
  BOOST_TEST(tokens[0] == "hello");
  delete[] tokens;

  karpenko::split(" one  two   three ", tokens, count);
  BOOST_REQUIRE_EQUAL(count, 3);
  BOOST_TEST(tokens[0] == "one");
  BOOST_TEST(tokens[1] == "two");
  BOOST_TEST(tokens[2] == "three");
  delete[] tokens;
}
