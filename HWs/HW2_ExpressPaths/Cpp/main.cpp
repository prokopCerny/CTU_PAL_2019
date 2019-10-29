#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <tuple>
#include <stack>

using std::tuple;
using std::pair;
using std::vector;
using std::list;
using std::cin;
using std::cout;
using std::stack;

class Graph {
public:
    uint64_t vertices;
    std::vector<std::list<uint32_t>> adjacency_list;

    explicit Graph(uint64_t vertices) : vertices(vertices) {
        adjacency_list.resize(vertices);
    }

    Graph(Graph &&other) = default;

    void add_edge(uint32_t from, uint32_t to) {
        adjacency_list[from].emplace_back(to);
    }
};

void tarjan_recursive(uint32_t vertex,
                      uint32_t &time,
                      Graph &graph,
                      vector<int32_t> &discovered,
                      vector<int32_t> &lowest,
                      vector<bool> &in_stack,
                      uint32_t &component_number,
                      vector<int32_t> &component,
                      vector<uint32_t> &component_sizes,
                      stack<uint32_t> &stack,
                      list<uint32_t> &topological_ordering) {
    discovered[vertex] = time;
    lowest[vertex] = time;
    time++;

    stack.push(vertex);
    in_stack[vertex] = true;

    for (uint32_t neighbor : graph.adjacency_list[vertex]) {
        if (discovered[neighbor] == -1) {
            tarjan_recursive(neighbor, time, graph, discovered, lowest, in_stack, component_number, component,
                             component_sizes, stack, topological_ordering);

            lowest[vertex] = std::min(lowest[vertex], lowest[neighbor]);
        } else if (in_stack[neighbor]) {
            lowest[vertex] = std::min(discovered[neighbor], lowest[vertex]);
        }
    }

    if (lowest[vertex] == discovered[vertex]) {
        component_sizes.push_back(0);

        while (stack.top() != vertex) {
            uint32_t cur = stack.top();
            component[cur] = component_number;
            component_sizes[component_number]++;
            in_stack[cur] = false;
            stack.pop();
        }
        in_stack[vertex] = false;
        stack.pop();
        component[vertex] = component_number;
        component_sizes[component_number]++;

        component_number++;
    }
    topological_ordering.push_front(vertex);
}

/**
 * Find strongly connected components, their sizes and topological ordering of the vertices
 * @param graph
 * @return tuple of (vector of length vertices with elements being the SCC of the vertex at the given index;
 *                  vector of component sizes for the respective index;
 *                  list of topological ordering of vertices)
 */
tuple<vector<int32_t>, vector<uint32_t>, list<uint32_t>> tarjan_scc_topo(Graph &graph) {
    vector<int32_t> component(graph.vertices, -1);
    vector<uint32_t> componentSizes;
    list<uint32_t> topological_ordering;

    vector<int32_t> discovered(graph.vertices, -1);
    vector<int32_t> lowest(graph.vertices, -1);
    vector<bool> in_stack(graph.vertices, false);
    stack<uint32_t> stack;
    uint32_t time = 0;
    uint32_t component_number = 0;

    for (uint64_t v = 0; v < graph.vertices; v++) {
        if (discovered[v] == -1) {
            tarjan_recursive(v, time, graph, discovered, lowest, in_stack, component_number, component, componentSizes,
                             stack, topological_ordering);
        }
    }
    return std::make_tuple(std::move(component), std::move(componentSizes), std::move(topological_ordering));
}

/**
 * Find longest maximum cost express path.
 * @param graph
 * @return pair of (cost of the path, length of the path)
 */
pair<uint32_t, uint32_t> find_maximum_cost_longest_express_path(Graph &graph,
                                                                vector<int32_t> &component,
                                                                vector<uint32_t> &component_sizes,
                                                                list<uint32_t> &topological_order) {
    uint32_t maxPathCost = 0;
    uint32_t maxPathLength = 0;

    vector<uint32_t> pathCostTo(graph.vertices);
    vector<uint32_t> pathLenTo(graph.vertices, 0);
    vector<bool> visited(graph.vertices, false); //maybe unnecessary
    for (uint64_t v = 0; v < graph.vertices; v++) {
        pathCostTo[v] = component_sizes[component[v]];
    }

    for (uint32_t vertex : topological_order) {
        visited[vertex] = true;
        if (pathLenTo[vertex] > 0 && pathCostTo[vertex] >= maxPathCost) {
            if (pathCostTo[vertex] == maxPathCost && pathLenTo[vertex] > maxPathLength) {
                maxPathLength = pathLenTo[vertex];
            } else {
                maxPathCost = pathCostTo[vertex];
                maxPathLength = pathLenTo[vertex];
            }
        }

        for (uint32_t neighbor : graph.adjacency_list[vertex]) {
            if (!visited[neighbor]
                && component[vertex] != component[neighbor]
                && component_sizes[component[neighbor]] >= component_sizes[component[vertex]]) {
                uint32_t costToNeighbor = pathCostTo[vertex] + component_sizes[component[neighbor]];
                uint32_t lenToNeighbor = pathLenTo[vertex] + 1;
                if (costToNeighbor > pathCostTo[neighbor]) {
                    pathCostTo[neighbor] = costToNeighbor;
                    pathLenTo[neighbor] = lenToNeighbor;
                } else if (costToNeighbor == pathCostTo[neighbor] && lenToNeighbor > pathLenTo[neighbor]) {
                    pathLenTo[neighbor] = lenToNeighbor;
                }
            }
        }
    }


    return std::make_pair(maxPathCost, maxPathLength);
}

int main() {
    std::ios_base::sync_with_stdio(false);

    uint32_t vertices, edges;
    cin >> vertices >> edges;
    Graph graph(vertices);

    for (uint32_t e = 0; e < edges; e++) {
        uint32_t from, to;
        cin >> from >> to;
        graph.add_edge(from, to);
    }

    vector<int32_t> component;
    vector<uint32_t> componentSizes;
    list<uint32_t> topological_ordering;

    std::tie(component, componentSizes, topological_ordering) = tarjan_scc_topo(graph);

    uint32_t maxPathCost;
    uint32_t maxPathLength;

    std::tie(maxPathCost, maxPathLength) = find_maximum_cost_longest_express_path(graph, component, componentSizes, topological_ordering);

    cout << maxPathCost << " " << maxPathLength << std::endl;

    return 0;
}