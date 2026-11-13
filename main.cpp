#include "graph.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main()
{
    ListGraph<std::string> graph;
    for (const std::string &vertex : {"start", "middle", "finish"})
        graph.AddVertex(vertex);
    graph.AddEdge("start", "middle", 2.5);
    graph.AddEdge("middle", "finish", 1.0);

    MatrixGraph<std::string> matrixGraph(&graph);
    std::cout << "Graph:\n";
    matrixGraph.printVertexAll();

    std::cout << "BFS:";
    for (const auto &vertex : breadthFirstSearch(matrixGraph, std::string("start")))
        std::cout << " " << vertex;
    std::cout << "\nDFS:";
    for (const auto &vertex : depthFirstSearch(matrixGraph, std::string("start")))
        std::cout << " " << vertex;
    std::cout << "\nDistances from start:\n";
    for (const auto &[vertex, distance] : dijkstra(matrixGraph, std::string("start")))
        std::cout << "  " << vertex << ": " << distance << '\n';
    return 0;
}