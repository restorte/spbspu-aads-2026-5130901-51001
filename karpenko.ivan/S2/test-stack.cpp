#include <boost/test/unit_test.hpp>
#include "stack.hpp"

using namespace karpenko;

BOOST_AUTO_TEST_SUITE(StackSuite)

BOOST_AUTO_TEST_CASE(test_push_pop)
{
  Stack<int> s;
  BOOST_CHECK(s.empty());
  s.push(10);
  BOOST_CHECK(!s.empty());
  BOOST_CHECK_EQUAL(s.top(), 10);
  s.push(20);
  BOOST_CHECK_EQUAL(s.top(), 20);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 10);
  s.pop();
  BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(test_exception_on_empty)
{
  Stack<int> s;
  BOOST_CHECK_THROW(s.pop(), std::out_of_range);
  BOOST_CHECK_THROW(s.top(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_copy)
{
  Stack<int> s1;
  s1.push(1);
  s1.push(2);
  Stack<int> s2 = s1;
  BOOST_CHECK_EQUAL(s2.size(), 2);
  BOOST_CHECK_EQUAL(s2.top(), 2);
  s2.pop();
  BOOST_CHECK_EQUAL(s2.top(), 1);
  BOOST_CHECK_EQUAL(s1.top(), 2);
}

BOOST_AUTO_TEST_CASE(test_move)
{
  Stack<int> s1;
  s1.push(100);
  Stack<int> s2 = std::move(s1);
  BOOST_CHECK(s1.empty());
  BOOST_CHECK_EQUAL(s2.top(), 100);
}

BOOST_AUTO_TEST_SUITE_END()
