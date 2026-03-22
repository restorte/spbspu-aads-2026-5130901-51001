#define BOOST_TEST_MODULE QueueTest

#include "queue.hpp"
#include <boost/test/included/unit_test.hpp>

using namespace karpenko;

BOOST_AUTO_TEST_SUITE(QueueSuite)

BOOST_AUTO_TEST_CASE(test_push_pop) {
  Queue<int> q;
  BOOST_CHECK(q.empty());
  q.push(10);
  BOOST_CHECK(!q.empty());
  BOOST_CHECK_EQUAL(q.front(), 10);
  BOOST_CHECK_EQUAL(q.back(), 10);
  q.push(20);
  BOOST_CHECK_EQUAL(q.front(), 10);
  BOOST_CHECK_EQUAL(q.back(), 20);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 20);
  q.pop();
  BOOST_CHECK(q.empty());
}

BOOST_AUTO_TEST_CASE(test_exception_on_empty) {
  Queue<int> q;
  BOOST_CHECK_THROW(q.pop(), std::out_of_range);
  BOOST_CHECK_THROW(q.front(), std::out_of_range);
  BOOST_CHECK_THROW(q.back(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_copy) {
  Queue<int> q1;
  q1.push(1);
  q1.push(2);
  Queue<int> q2 = q1;
  BOOST_CHECK_EQUAL(q2.size(), 2);
  BOOST_CHECK_EQUAL(q2.front(), 1);
  q2.pop();
  BOOST_CHECK_EQUAL(q2.front(), 2);
  BOOST_CHECK_EQUAL(q1.front(), 1);
}

BOOST_AUTO_TEST_CASE(test_move) {
  Queue<int> q1;
  q1.push(100);
  Queue<int> q2 = std::move(q1);
  BOOST_CHECK(q1.empty());
  BOOST_CHECK_EQUAL(q2.front(), 100);
}

BOOST_AUTO_TEST_SUITE_END()
