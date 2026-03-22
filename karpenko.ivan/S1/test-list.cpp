#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>
#include <utility>
#include "list.hpp"

using namespace karpenko;

BOOST_AUTO_TEST_SUITE(list_tests)

BOOST_AUTO_TEST_CASE(test_empty)
{
  List< int > lst;
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_push_front)
{
  List< int > lst;
  lst.push_front(42);
  BOOST_CHECK(!lst.empty());
  BOOST_CHECK(lst.front() == 42);

  lst.push_front(24);
  BOOST_CHECK(lst.front() == 24);
}

BOOST_AUTO_TEST_CASE(test_push_back)
{
  List< int > lst;
  lst.push_back(1);
  lst.push_back(2);
  BOOST_CHECK(lst.front() == 1);
  BOOST_CHECK(lst.back() == 2);
  BOOST_CHECK(lst.size() == 2);
}

BOOST_AUTO_TEST_CASE(test_pop_front)
{
  List< int > lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.pop_front();
  BOOST_CHECK(lst.front() == 2);
  lst.pop_front();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_pop_back)
{
  List< int > lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.pop_back();
  BOOST_CHECK(lst.back() == 1);
  lst.pop_back();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_clear)
{
  List< int > lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.clear();
  BOOST_CHECK(lst.empty());
  BOOST_CHECK(lst.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_size)
{
  List< int > lst;
  BOOST_CHECK(lst.size() == 0);
  lst.push_back(1);
  BOOST_CHECK(lst.size() == 1);
  lst.push_back(2);
  BOOST_CHECK(lst.size() == 2);
  lst.pop_front();
  BOOST_CHECK(lst.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_iterator)
{
  List< int > lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);

  List< int >::iterator it = lst.begin();
  BOOST_CHECK(*it == 10);
  ++it;
  BOOST_CHECK(*it == 20);
  ++it;
  BOOST_CHECK(*it == 30);
  ++it;
  BOOST_CHECK(it == lst.end());
}

BOOST_AUTO_TEST_CASE(test_const_iterator)
{
  List< int > lst;
  lst.push_back(42);

  List< int >::const_iterator it = lst.cbegin();
  BOOST_CHECK(*it == 42);
  ++it;
  BOOST_CHECK(it == lst.cend());
}

BOOST_AUTO_TEST_CASE(test_copy_constructor)
{
  List< int > a;
  a.push_back(1);
  a.push_back(2);

  List< int > b(a);
  BOOST_CHECK(b.size() == 2);
  BOOST_CHECK(b.front() == 1);
  BOOST_CHECK(b.back() == 2);
}

BOOST_AUTO_TEST_CASE(test_move_constructor)
{
  List< int > a;
  a.push_back(99);

  List< int > b(std::move(a));
  BOOST_CHECK(a.empty());
  BOOST_CHECK(b.size() == 1);
  BOOST_CHECK(b.front() == 99);
}

BOOST_AUTO_TEST_CASE(test_assignment)
{
  List< int > a;
  a.push_back(5);
  a.push_back(6);

  List< int > b;
  b = a;
  BOOST_CHECK(b.size() == 2);
  BOOST_CHECK(b.front() == 5);
  BOOST_CHECK(b.back() == 6);
}

BOOST_AUTO_TEST_CASE(test_swap)
{
  List< int > a;
  a.push_back(1);
  a.push_back(2);

  List< int > b;
  b.push_back(3);

  a.swap(b);
  BOOST_CHECK(a.size() == 1);
  BOOST_CHECK(a.front() == 3);
  BOOST_CHECK(b.size() == 2);
  BOOST_CHECK(b.front() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
