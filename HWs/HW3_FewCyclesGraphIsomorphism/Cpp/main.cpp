#include <iostream>
#include <vector>
#include <tuple>
#include <list>
#include <cstdint>
#include <stack>
#include <string>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <algorithm>

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
        vector<uint32_t> &cycleSizes,
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
        cycleSizes.push_back(1);
        uint32_t cycleCur = cur_parent;
        cycleMembership[cycleCur] = cycleNumber;
        while (cycleCur != cur) {
            cycleCur = parent[cycleCur];
            cycleMembership[cycleCur] = cycleNumber;
            cycleSizes[cycleNumber]++;
        }
        cycleNumber++;
        return;
    }

    state[cur] = Color::SEEN;
    parent[cur] = cur_parent;

    for (uint32_t neigh : graph.adjacency_list[cur]) {
        if (neigh != cur_parent) {
            find_cycles_dfs(neigh, cur, cycleNumber, cycleMembership, cycleSizes, state, parent, thirdDegreeVertices, graph);
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
tuple<uint32_t, vector<int32_t>, vector<uint32_t>> find_cycles(const Graph &graph) {
    uint32_t root = 0;
    int32_t cycleNumber = 0;
    vector<int32_t> cycleMembership(graph.vertices, -1);
    vector<uint32_t> cycleSizes;
    vector<Color> state(graph.vertices, Color::UNSEEN);
    vector<int32_t> parent(graph.vertices, -1);
    vector<uint32_t> thirdDegreeVertices;
    stack<uint32_t> stack;

    find_cycles_dfs(0, 0, cycleNumber, cycleMembership, cycleSizes, state, parent, thirdDegreeVertices, graph);

    for (uint32_t vertex : thirdDegreeVertices) {
        if (cycleMembership[vertex] == -1) {
            root = vertex;
            break;
        }
    }
    return make_tuple(root, move(cycleMembership), move(cycleSizes));
}

/**
 * Creates certificates for cycles using BFS traversal. Passes all public tests and 9/10 private tests
 * @param vertex
 * @param graph
 * @param cycleMembership
 * @param cycleSizes
 * @param visited
 * @return
 */
string get_child_certificate_cycle_bfs(uint32_t vertex, const Graph &graph, const vector<int32_t> &cycleMembership, const vector<uint32_t> &cycleSizes, vector<bool> &visited) {
    visited[vertex] = true;
    if (graph.adjacency_list[vertex].size() == 1) {
        return "";
    } else {
        vector<list<string>> cycleSubcertificates;
        queue<pair<uint32_t, uint32_t>> queue;
        queue.emplace(vertex, 0);
        while (!queue.empty()) {
            uint32_t cur;
            uint32_t depth;
            tie(cur, depth) = queue.front();
            queue.pop();
            if (depth+1 > cycleSubcertificates.size()) {
                cycleSubcertificates.emplace_back();
            }
            for (uint32_t neigh: graph.adjacency_list[cur]) {
                if (!visited[neigh]) {
                    if (cycleMembership[neigh] != cycleMembership[cur]) {
                        string c = get_child_certificate_cycle_bfs(neigh, graph, cycleMembership, cycleSizes, visited);
                        if (!c.empty()) {
                            cycleSubcertificates[depth].push_back(c);
                        }
                    } else {
                        queue.emplace(neigh, depth+1);
                        visited[neigh] = true;
                    }
                }
            }
        }

        stringstream certificate;
        certificate << cycleSizes[cycleMembership[vertex]] << "(";
        for (int i = 0; i < cycleSubcertificates.size(); i++) {
            if (!cycleSubcertificates[i].empty()) {
                certificate << i << ":" << "{";
                cycleSubcertificates[i].sort();
                for (auto &subcert : cycleSubcertificates[i]) {
                    if (!subcert.empty()) {
                        certificate << subcert << ",";
                    }
                }
                certificate << "}";
            }
        }
        certificate << ")";
        return certificate.str();

    }
}


/**
 * creates cycle certificates by walking the cycle in both directions and selecting the lexicographically smaller resulting certificate.
 * Uses cache for speedup
 * @param vertex
 * @param graph
 * @param cycleMembership
 * @param cycleSizes
 * @param visited
 * @param cache
 * @return
 */
string get_child_certificate_double_cycle_walk(uint32_t vertex, const Graph &graph, const vector<int32_t> &cycleMembership, const vector<uint32_t> &cycleSizes, vector<bool> &visited, unordered_map<uint32_t, string> &cache) {
    visited[vertex] = true;
    if (graph.adjacency_list[vertex].size() == 1) {
        return "";
    } else if (cache.find(vertex) != cache.end()) {
        return cache[vertex];
    } else {
        vector<list<string>> cycleSubcertificates;
        vector<uint32_t> cycleNeighs;
        for (uint32_t n : graph.adjacency_list[vertex]) {
            if (cycleMembership[n] == cycleMembership[vertex]) {
                cycleNeighs.push_back(n);
            }
        }
        list<string> possibleCertificates;
        for (uint32_t dir: cycleNeighs) {
            vector<bool> cycleVisited(graph.vertices, false);
            cycleVisited[vertex] = true;
            queue<pair<uint32_t, uint32_t>> queue;
            queue.emplace(dir, 0);
            stringstream curCert;
            while (!queue.empty()) {
                uint32_t cur;
                uint32_t depth;
                tie(cur, depth) = queue.front();
                queue.pop();
                cycleVisited[cur] = true;
                for (uint32_t neigh: graph.adjacency_list[cur]) {
                    if (!cycleVisited[neigh]) {
                        if (cycleMembership[neigh] != cycleMembership[cur]) {
                            string c = get_child_certificate_double_cycle_walk(neigh, graph, cycleMembership,
                                                                               cycleSizes, visited, cache);
                            if (!c.empty()) {
                                curCert << "b" << c << "b";
                            } else {
                                curCert << "a";
                            }
                        } else {
                            queue.emplace(neigh, depth+1);
                            visited[neigh] = true;
                        }
                    }
                }
            }
            possibleCertificates.push_back(curCert.str());
        }

        stringstream certificate;
        certificate << cycleSizes[cycleMembership[vertex]] << "(";
        possibleCertificates.sort();
        certificate << possibleCertificates.front() << ")";
        cache[vertex] = certificate.str();
        return cache[vertex];

    }
}

/**
 * wrapper for using double cycle walk
 * @param vertex
 * @param graph
 * @param cycleMembership
 * @param cycleSizes
 * @param visited
 * @return
 */
string get_child_certificate2_cached(uint32_t vertex, const Graph &graph, const vector<int32_t> &cycleMembership, const vector<uint32_t> &cycleSizes, vector<bool> &visited) {
    unordered_map<uint32_t, string> cache;
    return get_child_certificate_double_cycle_walk(vertex, graph, cycleMembership, cycleSizes, visited, cache);
}

string create_graph_certificate(const Graph &graph, uint32_t root, const vector<int32_t> &cycleMembership, const vector<uint32_t> &cycleSizes) {
    list<string> childCertificates;
    vector<bool> visited(graph.vertices, false);
    visited[root] = true;
    for (uint32_t neigh : graph.adjacency_list[root]) {
        childCertificates.push_back(get_child_certificate2_cached(neigh, graph, cycleMembership, cycleSizes, visited));
    }
    childCertificates.sort();
    stringstream certificate;
    for (auto &str : childCertificates) {
        certificate << str;
    }
    return certificate.str();
}

int main() {
    std::ios_base::sync_with_stdio(false);

    uint16_t numberOfGraphs{};
    uint32_t numberOfVertices{}, numberOfEdges{};

    unordered_map<string, uint32_t> counts;

    cin >> numberOfGraphs >> numberOfVertices >> numberOfEdges;


    for (uint16_t graphIdx = 0; graphIdx < numberOfGraphs; graphIdx++) {
        Graph curGraph(numberOfVertices);
        for (uint32_t edgeNum = 0; edgeNum < numberOfEdges; edgeNum++) {
            uint32_t from, to;
            cin >> from >> to;
            curGraph.add_edge(from-1, to-1);
        }
        vector<int32_t> cycleMembership;
        vector<uint32_t> cycleSizes;
        uint32_t root;
        tie(root, cycleMembership, cycleSizes) = find_cycles(curGraph);
        string c = create_graph_certificate(curGraph, root, cycleMembership, cycleSizes);
//        cout << c << endl;
        counts[c]++;

    }
    vector<uint32_t> countsToSort;
    for (const auto& kv : counts) {
        countsToSort.push_back(kv.second);
    }
    sort(countsToSort.begin(), countsToSort.end());
    for (uint32_t n : countsToSort) {
        cout << n << " ";
    }
    cout << endl;
    return 0;
}