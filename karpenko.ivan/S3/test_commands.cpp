#define BOOST_TEST_MODULE CommandTests
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>
#include "command_system.hpp"

using namespace karpenko;

static GraphCollection loadTestGraphs()
{
    return readGraphs("test.txt");
}

static std::string executeCommand(const std::string& cmd, GraphCollection& gc)
{
    std::istringstream in;
    std::ostringstream out;
    CommandSystem cs;
    cs.execute(cmd, out, gc);
    return out.str();
}

BOOST_AUTO_TEST_CASE(test_graphs)
{
    auto gc = loadTestGraphs();
    std::string result = executeCommand("graphs", gc);
    BOOST_TEST(result == "gr1\ngr2\n");
}

BOOST_AUTO_TEST_CASE(test_vertexes)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("vertexes gr1", gc);
    BOOST_TEST(res == "a\nb\nc\n");

    res = executeCommand("vertexes gr99", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");

    res = executeCommand("vertexes", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_outbound)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("outbound gr1 c", gc);
    BOOST_TEST(res == "a 30\nb 20\n");

    res = executeCommand("outbound gr2 c", gc);
    BOOST_TEST(res == "");

    res = executeCommand("outbound gr99 a", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");

    res = executeCommand("outbound gr1 x", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_inbound)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("inbound gr2 b", gc);
    BOOST_TEST(res == "a 1\nb 2\n");

    res = executeCommand("inbound gr2 a", gc);
    BOOST_TEST(res == "");

    res = executeCommand("inbound gr99 a", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");

    res = executeCommand("inbound gr1 x", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_bind)
{
    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("bind gr2 b c 100", gc);
        BOOST_TEST(res == "");

        res = executeCommand("vertexes gr2", gc);
        BOOST_TEST(res == "a\nb\nc\n");

        res = executeCommand("outbound gr2 b", gc);
        BOOST_TEST(res == "b 2\nc 100\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("bind gr99 a b 1", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }
}

BOOST_AUTO_TEST_CASE(test_cut)
{
    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("cut gr99 a b 1", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("cut gr2 x y 1", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("cut gr2 b b 999", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }
}

BOOST_AUTO_TEST_CASE(test_create)
{
    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("create gr3", gc);
        BOOST_TEST(res == "");

        res = executeCommand("graphs", gc);
        BOOST_TEST(res == "gr1\ngr2\ngr3\n");
    }

    {
        auto gc = loadTestGraphs();
        executeCommand("create gr3", gc);
        std::string res = executeCommand("create gr3", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("create gr4 3 x y z", gc);
        BOOST_TEST(res == "");

        res = executeCommand("vertexes gr4", gc);
        BOOST_TEST(res == "x\ny\nz\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("create gr5 3 a", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }
}

BOOST_AUTO_TEST_CASE(test_merge)
{
    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("merge gr3 gr2 gr2", gc);
        BOOST_TEST(res == "");

        res = executeCommand("inbound gr3 b", gc);
        BOOST_TEST(res == "a 1 1\nb 2 2\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("merge gr4 gr2 gr99", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        executeCommand("create gr3", gc);
        std::string res = executeCommand("merge gr3 gr1 gr2", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }
}

BOOST_AUTO_TEST_CASE(test_extract)
{
    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("extract gr3 gr2 2 a c", gc);
        BOOST_TEST(res == "");

        res = executeCommand("outbound gr3 a", gc);
        BOOST_TEST(res == "c 3\n");

        res = executeCommand("outbound gr3 c", gc);
        BOOST_TEST(res == "");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("extract gr4 gr99 1 a", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        std::string res = executeCommand("extract gr4 gr2 1 x", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }

    {
        auto gc = loadTestGraphs();
        executeCommand("create gr3", gc);
        std::string res = executeCommand("extract gr3 gr2 1 a", gc);
        BOOST_TEST(res == "<INVALID COMMAND>\n");
    }
}

BOOST_AUTO_TEST_CASE(test_unknown_command)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("fly gr1", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_bind_cut_sequence)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("bind gr1 a c 99", gc);
    BOOST_TEST(res == "");
    res = executeCommand("outbound gr1 a", gc);
    BOOST_TEST(res == "b 40\nc 99\n");
}

BOOST_AUTO_TEST_CASE(test_cut_nonexistent)
{
    auto gc = loadTestGraphs();
    std::string res = executeCommand("cut gr1 a b 999", gc);
    BOOST_TEST(res == "<INVALID COMMAND>\n");
}
