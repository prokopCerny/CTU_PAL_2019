#include <iostream>
#include <vector>
#include <tuple>
#include <list>
#include <cstdint>
#include <stack>

using namespace std;

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
        adjacency_list[to].emplace_back(from);
    }
};

enum class Color {UNSEEN, SEEN, COMPLETED};

void find_cycles_dfs(uint32_t cur,
        uint32_t cur_parent,
        int32_t &cycleNumber,
        vector<int32_t> &cycleMembership,
        vector<Color> &state,
        vector<int32_t> &parent,
        vector<uint32_t> &thirdDegreeVertices,
        const Graph &graph) {

    if (state[cur] == Color::UNSEEN && graph.adjacency_list[cur].size() == 3) {
        thirdDegreeVertices.push_back(cur);
    }

    if (state[cur] == Color::COMPLETED) {
        return;
    }

    if (state[cur] == Color::SEEN) {
        uint32_t cycleCur = cur_parent;
        cycleMembership[cycleCur] = cycleNumber;
        while (cycleCur != cur) {
            cycleCur = parent[cycleCur];
            cycleMembership[cycleCur] = cycleNumber;
        }
        cycleNumber++;
        return;
    }

    state[cur] = Color::SEEN;
    parent[cur] = cur_parent;

    for (uint32_t neigh : graph.adjacency_list[cur]) {
        if (neigh != cur_parent) {
            find_cycles_dfs(neigh, cur, cycleNumber, cycleMembership, state, parent, thirdDegreeVertices, graph);
        }
    }

    state[cur] = Color::COMPLETED;
    return;
}

/**
 *
 * @param graph
 * @return tuple of root vertex, vector of cycle membership
 */
tuple<uint32_t, vector<int32_t>> find_cycles(const Graph &graph) {
    uint32_t root = 0;
    int32_t cycleNumber = 0;
    vector<int32_t> cycleMembership(graph.vertices, -1);
    vector<Color> state(graph.vertices, Color::UNSEEN);
    vector<int32_t> parent(graph.vertices, -1);
    vector<uint32_t> thirdDegreeVertices;
    stack<uint32_t> stack;

    find_cycles_dfs(0, 0, cycleNumber, cycleMembership, state, parent, thirdDegreeVertices, graph);

    for (uint32_t vertex : thirdDegreeVertices) {
        if (cycleMembership[vertex] == -1) {
            root = vertex;
            break;
        }
    }
    return make_tuple(root, move(cycleMembership));
}

int main() {
    std::ios_base::sync_with_stdio(false);

    uint16_t numberOfGraphs{};
    uint32_t numberOfVertices{}, numberOfEdges{};

    cin >> numberOfGraphs >> numberOfVertices >> numberOfEdges;
    for (uint16_t graphIdx = 0; graphIdx < numberOfGraphs; graphIdx++) {
        Graph curGraph(numberOfVertices);
        for (uint32_t edgeNum = 0; edgeNum < numberOfEdges; edgeNum++) {
            uint32_t from, to;
            cin >> from >> to;
            curGraph.add_edge(from-1, to-1);
        }
        vector<int32_t> cycleMembership;
        uint32_t root;
        tie(root, cycleMembership) = find_cycles(curGraph);
        cout << root+1 << endl;
//        for (int i = 0; i < cycleMembership.size(); i++) {
//            if (cycleMembership[i] != -1) {
//                cout << i+1 << ": " << cycleMembership[i] << endl;
//            }
//        }
        cout << endl;

    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}