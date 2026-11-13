#include "graph.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

TEST(MatrixTests, ResizeCloneAndBounds)
{
    Matrix1D<std::string> matrix(2);
    matrix.setValue("edge", 0, 1);
    matrix.resize(3);
    EXPECT_EQ(matrix.getSize(), 3);
    EXPECT_EQ(matrix.getValue(0, 1), "edge");

    auto clonedMatrix = matrix.clone();
    EXPECT_EQ(clonedMatrix->getValue(0, 1), "edge");

    Matrix<std::string> matrix2D(2);
    matrix2D.setValue("value", 1, 0);
    matrix2D.resize(3);
    EXPECT_EQ(matrix2D.getValue(1, 0), "value");

    EXPECT_THROW(matrix.getValue(3, 0), std::invalid_argument);
}

TEST(GraphTests, ListGraphAndConversionsPreserveEdges)
{
    ListGraph<std::string> listGraph;
    listGraph.AddVertex("alpha");
    listGraph.AddVertex("beta");
    listGraph.AddVertex("gamma");
    listGraph.AddEdge("alpha", "beta");
    listGraph.AddEdge("beta", "gamma");

    EXPECT_EQ(listGraph.VerticesCount(), 3);
    EXPECT_EQ(listGraph.getAllEdge().count({"alpha", "beta"}), 1);
    EXPECT_EQ(listGraph.getVertNext("beta"), (std::vector<std::string>{"gamma"}));

    MatrixGraph<std::string> matrixGraph(&listGraph);
    EXPECT_EQ(matrixGraph.getVertNext("alpha"), (std::vector<std::string>{"beta"}));
    EXPECT_EQ(matrixGraph.getAllEdge(), listGraph.getAllEdge());

    ListGraph<std::string> convertedBack(&matrixGraph);
    EXPECT_EQ(convertedBack.getAllEdge(), listGraph.getAllEdge());

    std::vector<std::string> replacement{"gamma"};
    matrixGraph.GetNextVertices("alpha", replacement);
    EXPECT_EQ(matrixGraph.getVertNext("alpha"), replacement);

    std::vector<std::string> newPredecessors{"alpha"};
    convertedBack.GetPrevVertices("gamma", newPredecessors);
    EXPECT_EQ(convertedBack.getVertNext("alpha"), (std::vector<std::string>{"beta", "gamma"}));
}

TEST(GraphTests, MatrixGraphSupportsCustomVertexLabels)
{
    auto next = std::make_unique<Matrix1D<int>>(2);
    auto prev = std::make_unique<Matrix1D<int>>(2);
    MatrixGraph<std::string> graph(
        std::move(next),
        std::move(prev),
        std::vector<std::string>{"left", "right"});

    graph.AddEdge("left", "right");
    EXPECT_EQ(graph.getAllEdge().count({"left", "right"}), 1);
    graph.AddVertex("extra");
    EXPECT_EQ(graph.VerticesCount(), 3);
}

TEST(GraphAlgorithms, TraverseListGraphInBreadthAndDepthFirstOrder)
{
    ListGraph<std::string> graph;
    for (const std::string &vertex : {"A", "B", "C", "D", "isolated"})
        graph.AddVertex(vertex);
    graph.AddEdge("A", "B");
    graph.AddEdge("A", "C");
    graph.AddEdge("B", "D");
    graph.AddEdge("C", "D");

    EXPECT_EQ(
        breadthFirstSearch(graph, std::string("A")),
        (std::vector<std::string>{"A", "B", "C", "D"}));
    EXPECT_EQ(
        depthFirstSearch(graph, std::string("A")),
        (std::vector<std::string>{"A", "B", "D", "C"}));
    EXPECT_THROW(breadthFirstSearch(graph, std::string("missing")), std::invalid_argument);
    EXPECT_THROW(depthFirstSearch(graph, std::string("missing")), std::invalid_argument);
}

TEST(GraphAlgorithms, DijkstraFindsWeightedShortestPathsAndPreservesWeights)
{
    ListGraph<std::string> listGraph;
    for (const std::string &vertex : {"A", "B", "C", "D", "isolated"})
        listGraph.AddVertex(vertex);
    listGraph.AddEdge("A", "B", 4.0);
    listGraph.AddEdge("A", "C", 1.0);
    listGraph.AddEdge("C", "B", 1.0);
    listGraph.AddEdge("B", "D", 1.0);
    listGraph.AddEdge("C", "D", 5.0);

    const auto listDistances = dijkstra(listGraph, std::string("A"));
    EXPECT_DOUBLE_EQ(listDistances.at("A"), 0.0);
    EXPECT_DOUBLE_EQ(listDistances.at("B"), 2.0);
    EXPECT_DOUBLE_EQ(listDistances.at("C"), 1.0);
    EXPECT_DOUBLE_EQ(listDistances.at("D"), 3.0);
    EXPECT_TRUE(std::isinf(listDistances.at("isolated")));
    EXPECT_DOUBLE_EQ(listGraph.GetEdgeWeight("A", "B"), 4.0);

    MatrixGraph<std::string> matrixGraph(&listGraph);
    EXPECT_DOUBLE_EQ(matrixGraph.GetEdgeWeight("A", "B"), 4.0);
    const auto matrixDistances = dijkstra(matrixGraph, std::string("A"));
    EXPECT_DOUBLE_EQ(matrixDistances.at("B"), 2.0);
    EXPECT_DOUBLE_EQ(matrixDistances.at("D"), 3.0);
    EXPECT_EQ(
        breadthFirstSearch(matrixGraph, std::string("A")),
        (std::vector<std::string>{"A", "B", "C", "D"}));
    EXPECT_EQ(
        depthFirstSearch(matrixGraph, std::string("A")),
        (std::vector<std::string>{"A", "B", "D", "C"}));
}

TEST(GraphAlgorithms, RejectNegativeWeights)
{
    ListGraph<int> graph;
    graph.AddVertex(1);
    graph.AddVertex(2);
    EXPECT_THROW(graph.AddEdge(1, 2, -1.0), std::invalid_argument);

    graph.AddEdge(1, 2);
    EXPECT_THROW(graph.AddEdge(1, 2, std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
    EXPECT_DOUBLE_EQ(graph.GetEdgeWeight(1, 2), 1.0);
    EXPECT_THROW(graph.GetEdgeWeight(2, 1), std::invalid_argument);
}
