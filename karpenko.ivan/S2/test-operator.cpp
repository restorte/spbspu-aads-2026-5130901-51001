#include <boost/test/unit_test.hpp>
#include "evaluator.hpp"

using namespace karpenko;

BOOST_AUTO_TEST_SUITE(OperatorSuite)

BOOST_AUTO_TEST_CASE(test_bitwise_or)
{
  BOOST_CHECK_EQUAL(evaluator::evaluate("5 | 3"), 7);
  BOOST_CHECK_EQUAL(evaluator::evaluate("1 | 2 | 4"), 7);
  BOOST_CHECK_EQUAL(evaluator::evaluate("( 1 | 2 ) | 4"), 7);
  BOOST_CHECK_EQUAL(evaluator::evaluate("0 | 123"), 123);
}

BOOST_AUTO_TEST_CASE(test_bitwise_or_with_other_ops)
{
  BOOST_CHECK_EQUAL(evaluator::evaluate("5 + 3 | 2"), 10);
  BOOST_CHECK_EQUAL(evaluator::evaluate("5 | 3 + 2"), 5);
  BOOST_CHECK_EQUAL(evaluator::evaluate("2 * 3 | 4"), 6);
}

BOOST_AUTO_TEST_CASE(test_invalid_expression)
{
  BOOST_CHECK_THROW(evaluator::evaluate("5 |"), std::exception);
  BOOST_CHECK_THROW(evaluator::evaluate("| 3"), std::exception);
  BOOST_CHECK_THROW(evaluator::evaluate("( 5 | 3"), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
