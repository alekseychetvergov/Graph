#pragma once

#include "matrix.hpp"

#include <cmath>
#include <functional>
#include <limits>
#include <queue>

// ============================================================================
// ИНТЕРФЕЙС ГРАФА (IGraph из проекта 31_5_zad_2_Grafph.cpp)
// ============================================================================
template <typename T>
class IGraph
{
public:
    virtual ~IGraph() {}
    IGraph() {}
    IGraph(const IGraph<T> *_oth) {}

    virtual void AddVertex(const T &vertex) = 0;
    virtual void AddEdge(const T &from, const T &to) = 0;
    virtual void AddEdge(const T &from, const T &to, double weight) = 0;
    virtual double GetEdgeWeight(const T &from, const T &to) const = 0;
    virtual int VerticesCount() const = 0;
    virtual void GetNextVertices(const T &vertex, vector<T> &vertices) = 0;
    virtual void GetPrevVertices(const T &vertex, vector<T> &vertices) = 0;

    virtual void printVertexAll() const = 0;
    virtual void printMatrAll() const = 0;

    virtual set<pair<T, T>> getAllEdge() = 0;
    virtual vector<T> getVertex() = 0;
    virtual vector<T> getVertNext(const T &vert) = 0;
};

// ============================================================================
// КЛАСС СУЩНОСТИ LISTGRAPH (Списки смежности)
// ============================================================================
template <typename T>
class ListGraph : public IGraph<T>
{
public:
    ListGraph(map<T, set<T>> _mapVerticesNext, map<T, set<T>> _mapVerticesPrev)
    {
        mapVerticesNext = _mapVerticesNext;
        mapVerticesPrev = _mapVerticesPrev;
        for (auto &it : mapVerticesNext)
        {
            for (auto &it_1 : it.second)
            {
                allEdge.insert({it.first, it_1});
                edgeWeights[{it.first, it_1}] = 1.0;
            }
        }
    }

    ListGraph() {}

    // Умный конструктор копирования: поддерживает полиморфное чтение из MatrixGraph
    ListGraph(IGraph<T> *other)
    {
        if (!other)
            return;

        ListGraph<T> *otherListGraph = dynamic_cast<ListGraph<T> *>(other);
        if (otherListGraph)
        {
            this->mapVerticesNext = otherListGraph->mapVerticesNext;
            this->mapVerticesPrev = otherListGraph->mapVerticesPrev;
            this->allEdge = otherListGraph->allEdge;
            this->edgeWeights = otherListGraph->edgeWeights;
        }
        else
        {
            std::vector<T> vertices = other->getVertex();
            for (const T &v : vertices)
            {
                this->AddVertex(v);
            }
            for (const T &v : vertices)
            {
                std::vector<T> nextVertices = other->getVertNext(v);
                for (const T &next_v : nextVertices)
                {
                    this->mapVerticesNext[v].insert(next_v);
                    this->mapVerticesPrev[next_v].insert(v);
                    this->allEdge.insert({v, next_v});
                    this->edgeWeights[{v, next_v}] = other->GetEdgeWeight(v, next_v);
                }
            }
        }
    }
    IGraph<T> &operator=(IGraph<T> *other)
    {
        if (this == other)
            return *this;
        ListGraph temp(other);
        std::swap(this->allEdge, temp.allEdge);
        std::swap(this->mapVerticesNext, temp.mapVerticesNext);
        std::swap(this->mapVerticesPrev, temp.mapVerticesPrev);
        std::swap(this->edgeWeights, temp.edgeWeights);
        return *this;
    }
    virtual void AddVertex(const T &vertex) override
    {
        if (mapVerticesNext.find(vertex) != mapVerticesNext.end())
        {
            cout << "Vertices "
                    " << vertex << "
                    " already exist.\n";
            return;
        }
        mapVerticesNext[vertex] = set<T>{};
        mapVerticesPrev[vertex] = set<T>{};
    }
    virtual void AddEdge(const T &from, const T &to) override
    {
        AddEdge(from, to, 1.0);
    }
    virtual void AddEdge(const T &from, const T &to, double weight) override
    {
        if (from == to)
        {
            cout << "You cannot create an edge by entering identical vertices.\n";
            return;
        }
        ValidateEdgeWeight(weight);
        if (mapVerticesNext.find(from) != mapVerticesNext.end() && mapVerticesPrev.find(to) != mapVerticesPrev.end())
        {
            mapVerticesNext[from].insert(to);
            mapVerticesPrev[to].insert(from);
            allEdge.insert({from, to});
            edgeWeights[{from, to}] = weight;
        }
        else
        {
            cout << "One or both vertices do not exist.\n";
        }
    }
    double GetEdgeWeight(const T &from, const T &to) const override
    {
        auto it = edgeWeights.find({from, to});
        if (it == edgeWeights.end())
            throw std::invalid_argument("Edge does not exist");
        return it->second;
    }
    virtual int VerticesCount() const override
    {
        return max(mapVerticesNext.size(), mapVerticesPrev.size());
    }
    virtual void GetNextVertices(const T &vertex, vector<T> &vertices) override
    {
        if (mapVerticesNext.find(vertex) == mapVerticesNext.end())
            return;
        for (const T &old_to : mapVerticesNext[vertex])
        {
            allEdge.erase({vertex, old_to});
            edgeWeights.erase({vertex, old_to});
            mapVerticesPrev[old_to].erase(vertex);
        }
        mapVerticesNext[vertex].clear();
        for (const T &next_v : vertices)
        {
            if (mapVerticesNext.find(next_v) != mapVerticesNext.end())
            {
                mapVerticesNext[vertex].insert(next_v);
                mapVerticesPrev[next_v].insert(vertex);
                allEdge.insert({vertex, next_v});
                edgeWeights[{vertex, next_v}] = 1.0;
            }
        }
    }
    virtual void GetPrevVertices(const T &vertex, vector<T> &vertices) override
    {
        if (mapVerticesPrev.find(vertex) == mapVerticesPrev.end())
            return;
        for (const T &old_from : mapVerticesPrev[vertex])
        {
            allEdge.erase({old_from, vertex});
            edgeWeights.erase({old_from, vertex});
            mapVerticesNext[old_from].erase(vertex);
        }
        mapVerticesPrev[vertex].clear();
        for (const T &prev_v : vertices)
        {
            if (mapVerticesPrev.find(prev_v) != mapVerticesPrev.end())
            {
                mapVerticesPrev[vertex].insert(prev_v);
                mapVerticesNext[prev_v].insert(vertex);
                allEdge.insert({prev_v, vertex});
                edgeWeights[{prev_v, vertex}] = 1.0;
            }
        }
    }
    virtual void printVertexAll() const override
    {
        for (auto &it : mapVerticesNext)
        {
            cout << "(" << it.first << ")";
            for (auto &it_1 : it.second)
                cout << " -> " << it_1;
            cout << endl;
        }
        cout << "Edges: ";
        for (auto &edge : allEdge)
            cout << edge.first << "-" << edge.second << " ";
        cout << endl;
    }
    virtual void printMatrAll() const override
    {
        cout << "ListGraph structure has no matrices to display.\n";
    }
    virtual vector<T> getVertex() override
    {
        vector<T> vecVert;
        for (auto &it : mapVerticesNext)
            vecVert.push_back(it.first);
        return vecVert;
    }
    virtual set<pair<T, T>> getAllEdge() override { return allEdge; }
    virtual vector<T> getVertNext(const T &vert) override
    {
        vector<T> vecVert;
        if (mapVerticesNext.find(vert) != mapVerticesNext.end())
        {
            for (const T &v : mapVerticesNext[vert])
                vecVert.push_back(v);
        }
        return vecVert;
    }

private:
    set<pair<T, T>> allEdge;
    map<T, set<T>> mapVerticesNext;
    map<T, set<T>> mapVerticesPrev;
    map<pair<T, T>, double> edgeWeights;

    static void ValidateEdgeWeight(double weight)
    {
        if (!std::isfinite(weight) || weight < 0)
            throw std::invalid_argument("Edge weight must be finite and non-negative");
    }
};
// ============================================================================
// КЛАСС СУЩНОСТИ MATRIXGRAPH (Полиморфное хранение любой BaseMatrix)
// ============================================================================
template <typename T>
class MatrixGraph : public IGraph<T>
{
public:
    // Главный конструктор: принимает unique_ptr на ЛЮБОГО наследника BaseMatrix
    MatrixGraph(std::unique_ptr<BaseMatrix<int>> _next,
                std::unique_ptr<BaseMatrix<int>> _prev,
                const vector<T> &_vertex = vector<T>())
    {
        vecVerticesNext = std::move(_next);
        vecVerticesPrev = std::move(_prev);
        size_t matrixSize = vecVerticesNext ? vecVerticesNext->getSize() : 0;
        if (_vertex.empty() && matrixSize > 0)
        {
            if constexpr (std::is_constructible_v<T, size_t>)
            {
                vertex.reserve(matrixSize);
                for (size_t i = 0; i < matrixSize; ++i)
                    vertex.emplace_back(i + 1);
            }
            else
            {
                throw std::invalid_argument("Vertex labels must be provided for this vertex type");
            }
        }
        else
        {
            vertex = _vertex;
        }
        if (vecVerticesNext)
        {
            for (size_t i = 0; i < vertex.size(); i++)
            {
                for (size_t j = 0; j < vertex.size(); j++)
                {
                    if (vecVerticesNext->getValue(i, j) > 0)
                    {
                        allEdge.insert({vertex[i], vertex[j]});
                        edgeWeights[{vertex[i], vertex[j]}] = 1.0;
                    }
                }
            }
        }
    }
    // Классический конструктор инициализации (по умолчанию строит Matrix1D)
    MatrixGraph(const vector<vector<int>> &_vecVertices, const vector<T> &_vertex)
    {
        vertex = _vertex;
        size_t n = vertex.size();
        vecVerticesNext = std::make_unique<Matrix1D<int>>(n);
        vecVerticesPrev = std::make_unique<Matrix1D<int>>(n);
        for (size_t i = 0; i < _vecVertices.size(); i++)
        {
            for (size_t j = 0; j < _vecVertices[i].size(); j++)
            {
                if (_vecVertices[i][j] > 0)
                {
                    vecVerticesNext->setValue(_vecVertices[i][j], i, j);
                    vecVerticesPrev->setValue(_vecVertices[i][j], j, i);
                    allEdge.insert({vertex[i], vertex[j]});
                    edgeWeights[{vertex[i], vertex[j]}] = 1.0;
                }
            }
        }
    }
    MatrixGraph()
    {
        vecVerticesNext = std::make_unique<Matrix1D<int>>(0);
        vecVerticesPrev = std::make_unique<Matrix1D<int>>(0);
    }
    // Умный конструктор копирования: использует Прототип (clone()) для сохранения типа матриц
    MatrixGraph(IGraph<T> *other)
    {
        if (!other)
            return;
        MatrixGraph<T> *otherMatrixGraph = dynamic_cast<MatrixGraph<T> *>(other);
        if (otherMatrixGraph && otherMatrixGraph->vecVerticesNext && otherMatrixGraph->vecVerticesPrev)
        {
            this->vecVerticesNext = otherMatrixGraph->vecVerticesNext->clone();
            this->vecVerticesPrev = otherMatrixGraph->vecVerticesPrev->clone();
            this->vertex = otherMatrixGraph->vertex;
            this->allEdge = otherMatrixGraph->allEdge;
            this->edgeWeights = otherMatrixGraph->edgeWeights;
        }
        else
        {
            this->vertex = other->getVertex();
            size_t n = this->vertex.size();
            this->vecVerticesNext = std::make_unique<Matrix1D<int>>(n);
            this->vecVerticesPrev = std::make_unique<Matrix1D<int>>(n);
            this->allEdge = other->getAllEdge();
            for (size_t i = 0; i < n; ++i)
            {
                std::vector<T> adjVertices = other->getVertNext(this->vertex[i]);
                for (const T &target : adjVertices)
                {
                    auto it = std::find(this->vertex.begin(), this->vertex.end(), target);
                    if (it != this->vertex.end())
                    {
                        size_t j = std::distance(this->vertex.begin(), it);
                        double weight = other->GetEdgeWeight(this->vertex[i], target);
                        this->vecVerticesNext->setValue(this->vecVerticesNext->getValue(i, j) + 1, i, j);
                        this->vecVerticesPrev->setValue(this->vecVerticesPrev->getValue(j, i) + 1, j, i);
                        this->edgeWeights[{this->vertex[i], target}] = weight;
                    }
                }
            }
        }
    }
    IGraph<T> &operator=(IGraph<T> *other)
    {
        if (this == other)
            return *this;
        MatrixGraph temp(other);
        std::swap(this->allEdge, temp.allEdge);
        std::swap(this->edgeWeights, temp.edgeWeights);
        std::swap(this->vertex, temp.vertex);
        std::swap(this->vecVerticesNext, temp.vecVerticesNext);
        std::swap(this->vecVerticesPrev, temp.vecVerticesPrev);
        return *this;
    }
    virtual void AddVertex(const T &vert) override
    {
        for (const T &v : vertex)
        {
            if (v == vert)
            {
                cout << "Vertices "
                        " << vert << "
                        " already exist.\n";
                return;
            }
        }
        vertex.push_back(vert);
        size_t newSize = vertex.size();
        vecVerticesNext->resize(newSize);
        vecVerticesPrev->resize(newSize);
    }
    virtual void AddEdge(const T &from, const T &to) override
    {
        AddEdge(from, to, 1.0);
    }
    virtual void AddEdge(const T &from, const T &to, double weight) override
    {
        ValidateEdgeWeight(weight);
        int x = -1, y = -1;
        for (size_t i = 0; i < vertex.size(); i++)
        {
            if (vertex[i] == from)
                x = i;
            if (vertex[i] == to)
                y = i;
        }
        if (from == to)
        {
            cout << "You cannot create an edge by entering identical vertices.\n";
            return;
        }
        if (x == -1 || y == -1)
        {
            cout << "Vertex not found\n";
            return;
        }
        vecVerticesNext->setValue(vecVerticesNext->getValue(x, y) + 1, x, y);
        vecVerticesPrev->setValue(vecVerticesPrev->getValue(y, x) + 1, y, x);
        allEdge.insert({from, to});
        edgeWeights[{from, to}] = weight;
    }
    double GetEdgeWeight(const T &from, const T &to) const override
    {
        auto it = edgeWeights.find({from, to});
        if (it == edgeWeights.end())
            throw std::invalid_argument("Edge does not exist");
        return it->second;
    }
    virtual int VerticesCount() const override { return vertex.size(); }
    virtual void GetNextVertices(const T &vert, vector<T> &vertices) override
    {
        int j = -1;
        for (size_t i = 0; i < vertex.size(); i++)
        {
            if (vertex[i] == vert)
            {
                j = i;
                break;
            }
        }
        if (j == -1)
            return;
        for (size_t z = 0; z < vertex.size(); z++)
        {
            if (vecVerticesNext->getValue(j, z) > 0)
            {
                allEdge.erase({vert, vertex[z]});
                edgeWeights.erase({vert, vertex[z]});
                vecVerticesNext->setValue(0, j, z);
                vecVerticesPrev->setValue(0, z, j);
            }
        }
        for (const T &target : vertices)
        {
            int q = -1;
            for (size_t c = 0; c < vertex.size(); c++)
            {
                if (vertex[c] == target)
                {
                    q = c;
                    break;
                }
            }
            if (q != -1)
            {
                vecVerticesNext->setValue(vecVerticesNext->getValue(j, q) + 1, j, q);
                vecVerticesPrev->setValue(vecVerticesPrev->getValue(q, j) + 1, q, j);
                allEdge.insert({vert, target});
                edgeWeights[{vert, target}] = 1.0;
            }
        }
    }
    virtual void GetPrevVertices(const T &vert, vector<T> &vertices) override
    {
        int j = -1;
        for (size_t i = 0; i < vertex.size(); i++)
        {
            if (vertex[i] == vert)
            {
                j = i;
                break;
            }
        }
        if (j == -1)
            return;
        for (size_t z = 0; z < vertex.size(); z++)
        {
            if (vecVerticesPrev->getValue(j, z) > 0)
            {
                allEdge.erase({vertex[z], vert});
                edgeWeights.erase({vertex[z], vert});
                vecVerticesPrev->setValue(0, j, z);
                vecVerticesNext->setValue(0, z, j);
            }
        }
        for (const T &source : vertices)
        {
            int q = -1;
            for (size_t c = 0; c < vertex.size(); c++)
            {
                if (vertex[c] == source)
                {
                    q = c;
                    break;
                }
            }
            if (q != -1)
            {
                vecVerticesPrev->setValue(vecVerticesPrev->getValue(j, q) + 1, j, q);
                vecVerticesNext->setValue(vecVerticesNext->getValue(q, j) + 1, q, j);
                allEdge.insert({source, vert});
                edgeWeights[{source, vert}] = 1.0;
            }
        }
    }
    virtual void printVertexAll() const override
    {
        for (size_t i = 0; i < vertex.size(); i++)
        {
            cout << "(" << vertex[i] << ")";
            for (size_t j = 0; j < vertex.size(); j++)
            {
                if (vecVerticesNext->getValue(i, j) > 0)
                {
                    cout << " -> " << vertex[j];
                }
            }
            cout << endl;
        }
    }
    virtual void printMatrAll() const override
    {
        cout << "Matrix Next:\n";
        vecVerticesNext->print();
        cout << "Matrix Prev:\n";
        vecVerticesPrev->print();
    }
    virtual vector<T> getVertex() override { return vertex; }
    virtual set<pair<T, T>> getAllEdge() override { return allEdge; }
    virtual vector<T> getVertNext(const T &vert) override
    {
        vector<T> vecVert;
        for (size_t i = 0; i < vertex.size(); i++)
        {
            if (vert == vertex[i])
            {
                for (size_t j = 0; j < vertex.size(); j++)
                {
                    if (vecVerticesNext->getValue(i, j) > 0)
                    {
                        vecVert.push_back(vertex[j]);
                    }
                }
                break;
            }
        }
        return vecVert;
    }

private:
    set<pair<T, T>> allEdge;
    vector<T> vertex;
    std::unique_ptr<BaseMatrix<int>> vecVerticesNext;
    std::unique_ptr<BaseMatrix<int>> vecVerticesPrev;
    map<pair<T, T>, double> edgeWeights;

    static void ValidateEdgeWeight(double weight)
    {
        if (!std::isfinite(weight) || weight < 0)
            throw std::invalid_argument("Edge weight must be finite and non-negative");
    }
};

template <typename T>
vector<T> breadthFirstSearch(IGraph<T> &graph, const T &start)
{
    const vector<T> vertices = graph.getVertex();
    if (std::find(vertices.begin(), vertices.end(), start) == vertices.end())
        throw std::invalid_argument("Start vertex does not exist");

    vector<T> order;
    set<T> visited{start};
    queue<T> pending;
    pending.push(start);
    while (!pending.empty())
    {
        T current = pending.front();
        pending.pop();
        order.push_back(current);
        for (const T &next : graph.getVertNext(current))
        {
            if (visited.insert(next).second)
                pending.push(next);
        }
    }
    return order;
}

template <typename T>
vector<T> depthFirstSearch(IGraph<T> &graph, const T &start)
{
    const vector<T> vertices = graph.getVertex();
    if (std::find(vertices.begin(), vertices.end(), start) == vertices.end())
        throw std::invalid_argument("Start vertex does not exist");

    vector<T> order;
    set<T> visited;
    vector<T> pending{start};
    while (!pending.empty())
    {
        T current = pending.back();
        pending.pop_back();
        if (!visited.insert(current).second)
            continue;
        order.push_back(current);

        vector<T> neighbors = graph.getVertNext(current);
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it)
        {
            if (visited.find(*it) == visited.end())
                pending.push_back(*it);
        }
    }
    return order;
}

template <typename T>
map<T, double> dijkstra(IGraph<T> &graph, const T &start)
{
    const vector<T> vertices = graph.getVertex();
    if (std::find(vertices.begin(), vertices.end(), start) == vertices.end())
        throw std::invalid_argument("Start vertex does not exist");

    const double infinity = std::numeric_limits<double>::infinity();
    map<T, double> distances;
    for (const T &vertex : vertices)
        distances.emplace(vertex, infinity);
    distances[start] = 0.0;

    using QueueEntry = pair<double, T>;
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> pending;
    pending.push({0.0, start});

    while (!pending.empty())
    {
        auto [distance, current] = pending.top();
        pending.pop();
        if (distance > distances[current])
            continue;

        for (const T &next : graph.getVertNext(current))
        {
            const double weight = graph.GetEdgeWeight(current, next);
            if (!std::isfinite(weight) || weight < 0)
                throw std::invalid_argument("Dijkstra requires finite, non-negative edge weights");
            const double candidate = distance + weight;
            if (candidate < distances[next])
            {
                distances[next] = candidate;
                pending.push({candidate, next});
            }
        }
    }
    return distances;
}
