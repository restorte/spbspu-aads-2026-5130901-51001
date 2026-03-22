#include <boost/test/unit_test.hpp>
#include "evaluator.hpp"

BOOST_AUTO_TEST_SUITE(OperatorTest)

BOOST_AUTO_TEST_CASE(BitwiseOrTests)
{
  BOOST_CHECK(karpenko::evaluator::evaluate("5 | 3") == 7);
  BOOST_CHECK(karpenko::evaluator::evaluate("1 | 2 | 4") == 7);
  BOOST_CHECK(karpenko::evaluator::evaluate("( 1 | 2 ) | 4") == 7);
  BOOST_CHECK(karpenko::evaluator::evaluate("0 | 123") == 123);
}

BOOST_AUTO_TEST_CASE(BitwiseOrWithOtherOpsTests)
{
  BOOST_CHECK(karpenko::evaluator::evaluate("5 + 3 | 2") == 10);
  BOOST_CHECK(karpenko::evaluator::evaluate("5 | 3 + 2") == 5);
  BOOST_CHECK(karpenko::evaluator::evaluate("2 * 3 | 4") == 6);
}

BOOST_AUTO_TEST_CASE(InvalidExpressionTests)
{
  BOOST_CHECK_THROW(karpenko::evaluator::evaluate("5 |"), std::exception);
  BOOST_CHECK_THROW(karpenko::evaluator::evaluate("| 3"), std::exception);
  BOOST_CHECK_THROW(karpenko::evaluator::evaluate("( 5 | 3"), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
