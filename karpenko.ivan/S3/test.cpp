#define BOOST_TEST_MODULE BucketHashTableTests
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <string>

#include "../common/hash_table.hpp"

using namespace karpenko;

BOOST_AUTO_TEST_CASE(empty_table)
{
    BucketHashTable< std::string, int > t(8, 4, 8);
    BOOST_TEST(t.size() == 0);
    BOOST_TEST(t.bucket_count() == 8);
    BOOST_TEST(!t.has("any"));
}

BOOST_AUTO_TEST_CASE(add_and_has)
{
    BucketHashTable< std::string, int > t(4, 2, 4);
    t.add("one", 1);
    BOOST_TEST(t.has("one"));
    BOOST_TEST(t.size() == 1);

    t.add("two", 2);
    BOOST_TEST(t.has("two"));
    BOOST_TEST(t.size() == 2);

    t.add("one", 10);
    BOOST_TEST(t.size() == 2);
    auto it = t.find("one");
    BOOST_TEST(it->second == 10);
}

BOOST_AUTO_TEST_CASE(find_and_iteration)
{
    BucketHashTable< std::string, int > t(4, 2, 4);
    t.add("a", 100);
    t.add("b", 200);
    t.add("c", 300);

    auto it = t.find("b");
    BOOST_TEST(it->second == 200);

    BOOST_TEST(!t.has("x"));

    int sum = 0;
    int cnt = 0;
    for (const auto& kv : t)
    {
        sum += kv.second;
        ++cnt;
    }
    BOOST_TEST(cnt == 3);
    BOOST_TEST(sum == 600);
}

BOOST_AUTO_TEST_CASE(remove_and_tombstone)
{
    BucketHashTable< std::string, int > t(4, 2, 4);
    t.add("x", 1);
    t.add("y", 2);
    BOOST_TEST(t.size() == 2);

    bool removed = t.remove("x");
    BOOST_TEST(removed);
    BOOST_TEST(t.size() == 1);
    BOOST_TEST(!t.has("x"));

    removed = t.remove("x");
    BOOST_TEST(!removed);
    BOOST_TEST(t.has("y"));
}

BOOST_AUTO_TEST_CASE(rehash_table)
{
    BucketHashTable< std::string, int > t(4, 4, 8);
    for (int i = 0; i < 10; ++i)
    {
        t.add("key" + std::to_string(i), i);
    }
    BOOST_TEST(t.size() == 10u);

    t.rehash(16, 4, 32);
    BOOST_TEST(t.size() == 10u);
    for (int i = 0; i < 10; ++i)
    {
        BOOST_TEST(t.has("key" + std::to_string(i)));
    }
}

BOOST_AUTO_TEST_CASE(overflow_throws)
{
    BucketHashTable< std::string, int > t(1, 1, 1);
    t.add("a", 1);
    t.add("b", 2);
    BOOST_CHECK_THROW(t.add("c", 3), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(collisions)
{
    struct ZeroHash {
        std::size_t operator()(const std::string&) const
        {
            return 0;
        }
    };
    BucketHashTable< std::string, int, ZeroHash > t(4, 2, 4);
    t.add("aaa", 1);
    t.add("bbb", 2);
    t.add("ccc", 3);
    t.add("ddd", 4);

    BOOST_TEST(t.has("aaa"));
    BOOST_TEST(t.has("bbb"));
    BOOST_TEST(t.has("ccc"));
    BOOST_TEST(t.has("ddd"));

    t.remove("bbb");
    BOOST_TEST(!t.has("bbb"));
    BOOST_TEST(t.has("aaa"));
    BOOST_TEST(t.has("ccc"));
    BOOST_TEST(t.has("ddd"));

    t.add("bbb", 20);
    BOOST_TEST(t.has("bbb"));
    BOOST_TEST(t.find("bbb")->second == 20);
}
