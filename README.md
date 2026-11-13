# Graph

Учебный проект на C++20 с ориентированными графами, шаблонным типом меток вершин и двумя способами хранения: списком смежности и матрицей смежности.

## Реализации графа

### `ListGraph<T>`

Хранит исходящие и входящие рёбра в ассоциативных контейнерах. Удобен для разреженных графов: память используется преимущественно для существующих вершин и рёбер.

Возможности:

- добавление вершин и рёбер;
- рёбра с весами (`AddEdge(from, to, weight)`); обычный `AddEdge(from, to)` задаёт вес `1`;
- получение вершин, соседей и набора рёбер;
- замена исходящих или входящих рёбер вершины;
- копирование из другого `IGraph<T>`, в том числе из `MatrixGraph<T>`.

### `MatrixGraph<T>`

Хранит смежность в матрицах `BaseMatrix<int>`. Тип `T` задаёт метки вершин, но не тип элементов матрицы. Поддерживаются матричные реализации `Matrix1D<int>` и `Matrix<int>`.

Возможности:

- те же основные операции, что и у `ListGraph<T>`;
- рёбра с весами;
- динамическое увеличение матриц при добавлении вершины;
- создание графа с пользовательскими метками;
- копирование из `ListGraph<T>` и клонирование матричного графа с сохранением реализации матрицы.

Обе реализации запрещают петли (рёбра из вершины в саму себя). Тип `T` должен поддерживать сравнение и вывод через `operator<<`.

## Использование

Заголовок `graph.hpp` включает `matrix.hpp`. Пример создания графа со строковыми метками и преобразования списка смежности в матричный граф:

```cpp
#include "graph.hpp"

#include <iostream>
#include <string>

int main()
{
    ListGraph<std::string> list;
    list.AddVertex("start");
    list.AddVertex("finish");
    list.AddEdge("start", "finish");

    MatrixGraph<std::string> matrix(&list);
    matrix.printVertexAll();

    return 0;
}
```

Для запроса соседей используйте `getVertNext(vertex)`. Методы `GetNextVertices` и `GetPrevVertices` **заменяют** исходящие или входящие рёбра вершины соответственно, используя переданный вектор:

```cpp
std::vector<std::string> newNeighbors{"finish"};
list.GetNextVertices("start", newNeighbors);
```

## Обходы и кратчайшие пути

Алгоритмы доступны как функции из `graph.hpp` и работают с обоими типами графа:

- `breadthFirstSearch(graph, start)` — поиск в ширину;
- `depthFirstSearch(graph, start)` — поиск в глубину;
- `dijkstra(graph, start)` — расстояния от начальной вершины до всех остальных.

Обходы возвращают вершины, достижимые из `start`, в порядке посещения. Дейкстра возвращает `std::map<T, double>`; для недостижимых вершин расстояние равно бесконечности. Начальная вершина должна существовать. Вес каждого ребра должен быть конечным и неотрицательным.

```cpp
ListGraph<std::string> graph;
graph.AddVertex("A");
graph.AddVertex("B");
graph.AddVertex("C");
graph.AddEdge("A", "B", 2.5);
graph.AddEdge("B", "C", 1.0);

const auto bfsOrder = breadthFirstSearch(graph, std::string("A"));
const auto dfsOrder = depthFirstSearch(graph, std::string("A"));
const auto distances = dijkstra(graph, std::string("A"));
std::cout << "Расстояние A -> C: " << distances.at("C") << '\n';
```

## Сборка и запуск без IDE

Нужны CMake, компилятор с поддержкой C++20 и доступ в интернет при первой конфигурации для загрузки GoogleTest.

В PowerShell из корня проекта:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Запустить демонстрационную программу:

```powershell
.\build\Graph.exe
```

Если используется многоконфигурационный генератор и исполняемый файл находится в папке конфигурации:

```powershell
.\build\Debug\Graph.exe
```

В Visual Studio Code можно также выбрать имеющийся пресет CMake из `CMakePresets.json`.

## Сборка с CMake Presets

Для Windows доступен пресет Visual Studio 2022 (MSVC), для Linux — GCC с Unix Makefiles. Нужны соответствующий компилятор и CMake; при первой конфигурации также нужен доступ в интернет для загрузки GoogleTest.

Windows, из корня проекта:

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-msvc-debug
ctest --preset windows-msvc-debug
```

Linux, из корня проекта:

```sh
cmake --preset linux-gcc
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

## Структура проекта

- `matrix.hpp` — интерфейс и реализации матриц.
- `graph.hpp` — `IGraph<T>`, `ListGraph<T>` и `MatrixGraph<T>`.
- `main.cpp` — демонстрационная программа.
- `tests/test_graph.cpp` — тесты GoogleTest.
- `CMakeLists.txt` — сборка приложения и тестов.
