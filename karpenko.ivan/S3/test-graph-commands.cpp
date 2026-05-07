#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <sstream>
#include <string>
#include "command_system.hpp"
#include "graph.hpp"
#include "graph_collection.hpp"
#include "../common/list.hpp"

namespace
{
  std::string dispatchLine(const std::string& line, karpenko::GraphCollection& storage)
  {
    std::istringstream in;
    std::ostringstream out;
    karpenko::CommandSystem cs;
    cs.execute(line, out, storage);
    return out.str();
  }

  karpenko::GraphCollection makeDefaultGraphs()
  {
    karpenko::GraphCollection gc;
    karpenko::Graph g1;
    g1.vertices_.add("a", true);
    g1.vertices_.add("b", true);
    g1.vertices_.add("c", true);
    addEdgeToMap(g1.out_edges_, "a", "b", 40);
    addEdgeToMap(g1.in_edges_, "b", "a", 40);
    addEdgeToMap(g1.out_edges_, "b", "c", 50);
    addEdgeToMap(g1.in_edges_, "c", "b", 50);
    addEdgeToMap(g1.out_edges_, "c", "a", 30);
    addEdgeToMap(g1.in_edges_, "a", "c", 30);
    addEdgeToMap(g1.out_edges_, "c", "b", 20);
    addEdgeToMap(g1.in_edges_, "b", "c", 20);
    gc.addGraph("gr1", g1);

    karpenko::Graph g2;
    g2.vertices_.add("a", true);
    g2.vertices_.add("b", true);
    g2.vertices_.add("c", true);
    addEdgeToMap(g2.out_edges_, "a", "b", 1);
    addEdgeToMap(g2.in_edges_, "b", "a", 1);
    addEdgeToMap(g2.out_edges_, "b", "b", 2);
    addEdgeToMap(g2.in_edges_, "b", "b", 2);
    addEdgeToMap(g2.out_edges_, "a", "c", 3);
    addEdgeToMap(g2.in_edges_, "c", "a", 3);
    gc.addGraph("gr2", g2);

    return gc;
  }
}

BOOST_AUTO_TEST_CASE(graph_bind_cut_and_sorted_queries)
{
  karpenko::Graph graph;
  graph.vertices_.add("b", true);
  graph.vertices_.add("c", true);
  graph.vertices_.add("d", true);
  graph.vertices_.add("a", true);

  addEdgeToMap(graph.out_edges_, "b", "c", 5);
  addEdgeToMap(graph.out_edges_, "b", "c", 5);
  addEdgeToMap(graph.out_edges_, "b", "c", 1);
  addEdgeToMap(graph.in_edges_, "c", "b", 5);
  addEdgeToMap(graph.in_edges_, "c", "b", 5);
  addEdgeToMap(graph.in_edges_, "c", "b", 1);
  addEdgeToMap(graph.out_edges_, "a", "b", 0);
  addEdgeToMap(graph.in_edges_, "b", "a", 0);

  BOOST_TEST(graph.vertices_.has("a"));
  BOOST_TEST(graph.vertices_.has("b"));
  BOOST_TEST(graph.vertices_.has("c"));
  BOOST_TEST(graph.vertices_.has("d"));

  std::ostringstream oss;
  printSortedVertices(oss, graph);
  BOOST_TEST(oss.str() == "a\nb\nc\nd\n");

  oss.str("");
  printEdges(oss, "b", graph.out_edges_);
  BOOST_TEST(oss.str() == "c 1 5 5\n");

  oss.str("");
  printEdges(oss, "b", graph.in_edges_);
  BOOST_TEST(oss.str() == "a 0\n");

  bool cut = removeWeightFromMap(graph.out_edges_, "b", "c", 5);
  BOOST_TEST(cut);
  removeWeightFromMap(graph.in_edges_, "c", "b", 5);
  oss.str("");
  printEdges(oss, "b", graph.out_edges_);
  BOOST_TEST(oss.str() == "c 1 5\n");

  BOOST_TEST(!removeWeightFromMap(graph.out_edges_, "b", "c", 7));

  removeWeightFromMap(graph.out_edges_, "b", "c", 1);
  removeWeightFromMap(graph.in_edges_, "c", "b", 1);
  removeWeightFromMap(graph.out_edges_, "b", "c", 5);
  removeWeightFromMap(graph.in_edges_, "c", "b", 5);
  oss.str("");
  printEdges(oss, "b", graph.out_edges_);
  BOOST_TEST(oss.str() == "\n");
  BOOST_TEST(graph.vertices_.has("b"));
  BOOST_TEST(graph.vertices_.has("c"));
  oss.str("");
  printEdges(oss, "d", graph.out_edges_);
  BOOST_TEST(oss.str() == "\n");
  oss.str("");
  printEdges(oss, "d", graph.in_edges_);
  BOOST_TEST(oss.str() == "\n");
}

BOOST_AUTO_TEST_CASE(graph_storage_operations)
{
  karpenko::GraphCollection gc;
  gc.addGraph("b");
  BOOST_TEST(gc.hasGraph("b"));
  BOOST_CHECK_THROW(gc.addGraph("b"), std::runtime_error);

  gc.getGraph("b").vertices_.add("v", true);
  BOOST_TEST(gc.getGraph("b").vertices_.has("v"));

  karpenko::Graph g;
  g.vertices_.add("x", true);
  gc.addGraph("a", g);

  std::ostringstream oss;
  karpenko::List<std::string> names;
  for (auto it = gc.getMap().begin(); it != gc.getMap().end(); ++it)
    names.push_back(it->first);
  std::size_t n = names.size();
  std::string* arr = new std::string[n];
  std::size_t i = 0;
  for (auto it = names.begin(); it != names.end(); ++it, ++i)
    arr[i] = *it;
  std::sort(arr, arr + n);
  for (i = 0; i < n; ++i)
    oss << arr[i] << '\n';
  delete[] arr;
  BOOST_TEST(oss.str() == "a\nb\n");
}

BOOST_AUTO_TEST_CASE(command_parser_helpers)
{
  karpenko::GraphCollection gc;
  BOOST_TEST(dispatchLine("", gc) == "");
  BOOST_TEST(dispatchLine("#comment", gc) == "");
  BOOST_TEST(dispatchLine("unknown", gc) == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(commands_dispatch_graph_queries_and_mutations)
{
  karpenko::GraphCollection gc = makeDefaultGraphs();
  BOOST_TEST(dispatchLine("graphs", gc) == "gr1\ngr2\n");
  BOOST_TEST(dispatchLine("vertexes gr2", gc) == "a\nb\nc\n");
  BOOST_TEST(dispatchLine("vertexes missing", gc) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatchLine("outbound gr2 a", gc) == "b 1\nc 3\n");
  BOOST_TEST(dispatchLine("inbound gr2 b", gc) == "a 1\nb 2\n");

  BOOST_TEST(dispatchLine("bind gr2 b d 0", gc) == "");
  BOOST_TEST(gc.getGraph("gr2").vertices_.has("d"));
  BOOST_TEST(dispatchLine("outbound gr2 b", gc) == "b 2\nd 0\n");
  BOOST_TEST(dispatchLine("cut gr2 b d 0", gc) == "");
  BOOST_TEST(dispatchLine("cut gr2 b d 0", gc) == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(commands_dispatch_create_merge_extract_and_invalid)
{
  karpenko::GraphCollection gc = makeDefaultGraphs();

  BOOST_TEST(dispatchLine("create empty", gc) == "");
  BOOST_TEST(gc.hasGraph("empty"));
  BOOST_TEST(dispatchLine("vertexes empty", gc) == "\n");
  BOOST_TEST(dispatchLine("create zero 0", gc) == "");
  BOOST_TEST(gc.hasGraph("zero"));
  BOOST_TEST(dispatchLine("create made 3 x y z", gc) == "");
  BOOST_TEST(dispatchLine("vertexes made", gc) == "x\ny\nz\n");
  BOOST_TEST(dispatchLine("create bad 2 x y z", gc) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatchLine("create badzero 0 x", gc) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatchLine("merge merged gr2 gr2", gc) == "");
  BOOST_TEST(dispatchLine("inbound merged b", gc) == "a 1 1\nb 2 2\n");
  BOOST_TEST(dispatchLine("merge gr2 gr1 gr1", gc) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatchLine("extract sub gr2 2 a c", gc) == "");
  BOOST_TEST(dispatchLine("vertexes sub", gc) == "a\nc\n");
  BOOST_TEST(dispatchLine("outbound sub a", gc) == "c 3\n");
  BOOST_TEST(dispatchLine("extract broken gr2 2 a missing", gc) == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(commands_output_formatting)
{
  karpenko::GraphCollection gc;
  gc.addGraph("g");
  BOOST_TEST(dispatchLine("graphs", gc) == "g\n");
  BOOST_TEST(dispatchLine("bind g a b 2", gc) == "");
  std::string outbound = dispatchLine("outbound g a", gc);
  BOOST_TEST(outbound == "b 2\n");
  BOOST_TEST(!outbound.empty());
  BOOST_TEST(outbound[outbound.size()-1] == '\n');
  BOOST_TEST(outbound.size() >= 2);
  BOOST_TEST(outbound[outbound.size()-2] != ' ');
  BOOST_TEST(dispatchLine("outbound g b", gc) == "\n");
  BOOST_TEST(dispatchLine("inbound g a", gc) == "\n");

  karpenko::GraphCollection emptyStorage;
  BOOST_TEST(dispatchLine("graphs", emptyStorage) == "\n");
}
