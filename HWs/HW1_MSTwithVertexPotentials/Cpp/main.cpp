#include <iostream>
#include <cstdint>
#include <vector>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <cstdlib>


struct VertexCosts {
    uint32_t edge_distance_to_potential;
    uint32_t potential;
};

class VertexGrid {
public:
    uint32_t rows;
    uint32_t columns;
    std::vector<VertexCosts> costGrid;
    std::unordered_map<uint64_t, uint64_t> additionalEdges{};

    VertexGrid(uint32_t rows, uint32_t columns) : rows(rows), columns(columns){
        costGrid.resize(rows * columns);
        VertexCosts infin{UINT32_MAX, UINT32_MAX};
        std::fill(costGrid.begin(), costGrid.end(), infin);
    }

    uint64_t getIdx(uint32_t row, uint32_t col) {
        return row*columns+col;
    }

    uint32_t getRow(uint64_t index) {
        return index / columns;
    }

    uint32_t getCol(uint64_t index) {
        return index % columns;
    }
};

class Graph {
public:
    uint64_t vertices;
    std::vector<std::list<std::pair<uint64_t, uint32_t>>> adjacency_list; //pair (node's linear index, cost)

    explicit Graph(uint64_t vertices) : vertices(vertices) {
        adjacency_list.resize(vertices);
    }

    Graph(Graph&& other) = default;

    void add_edge(uint64_t from, uint64_t to, uint32_t cost) {
        adjacency_list[from].emplace_back(to, cost);
    }

};

uint32_t getEdgeCost(uint32_t d1, uint32_t d2, uint32_t pot1, uint32_t pot2) {
    int64_t potDiff = (int64_t) pot1 - (int64_t) pot2;
    return d1 + d2 + (uint32_t) std::abs(potDiff);
}

Graph findPotentialsCreateGraph(VertexGrid &grid, std::vector<uint64_t>& sortedVectorsWithPotential) {
    std::array<std::pair<int8_t, int8_t>, 4> offsets = {{{-1, 0}, {0, -1}, {0, 1}, {1, 0}}};
    std::queue<uint64_t> queue;
    std::vector<bool> computed(grid.rows * grid.columns, false);
    Graph graph(grid.rows * grid.columns);
    auto processNeighbor = [&](VertexCosts& current, uint64_t currentCoords, uint64_t neighborCoords) {
        if (!computed[neighborCoords]) {
            grid.costGrid[neighborCoords] = {current.edge_distance_to_potential+1, current.potential};
            queue.push(neighborCoords);
            computed[neighborCoords] = true;
        }

        VertexCosts neighbor = grid.costGrid[neighborCoords];
        uint32_t edge_cost = getEdgeCost(current.edge_distance_to_potential,
                                         neighbor.edge_distance_to_potential,
                                         current.potential,
                                         neighbor.potential);
        graph.add_edge(currentCoords, neighborCoords, edge_cost);
    };

    for(uint64_t& vertex: sortedVectorsWithPotential) {
        computed[vertex] = true;
        queue.push(vertex);
    }

    while (!queue.empty()) {
        uint64_t currentCoords = queue.front();
        queue.pop();
        int64_t row = grid.getRow(currentCoords);
        int64_t col = grid.getCol(currentCoords);

        VertexCosts current = grid.costGrid[currentCoords];

        for (auto& offset: offsets) {
            int64_t nRow = row + offset.first;
            int64_t nCol = col + offset.second;
            if ( (nRow >= 0) && (nRow < grid.rows) && (nCol >= 0) && (nCol < grid.columns) ) {
                processNeighbor(current, currentCoords, grid.getIdx((uint32_t) nRow, (uint32_t) nCol));
            }
        }

        if (grid.additionalEdges.find(currentCoords) != grid.additionalEdges.end()) {
            processNeighbor(current, currentCoords, grid.additionalEdges[currentCoords]);
        }
    }

    return graph;
}

uint64_t primMSTcost(const Graph& graph) {
    std::vector<uint32_t> costToPred(graph.vertices, UINT32_MAX);
    std::vector<bool> visited(graph.vertices, false);
    std::set<std::pair<uint32_t, uint64_t>> priorityQueue;

    uint64_t MSTcost = 0;

    priorityQueue.insert(std::make_pair(0, 0));
    costToPred[0] = 0;

    while (!priorityQueue.empty()) {
        std::pair<uint32_t, uint64_t> current = *(priorityQueue.begin());
        priorityQueue.erase(priorityQueue.begin());

        uint32_t final_cost = current.first;
        uint64_t node = current.second;

        visited[node] = true;

        MSTcost += final_cost;
        for(const std::pair<uint64_t, uint32_t>& neighborPair: graph.adjacency_list[node]) {
            uint64_t neighborCoords = neighborPair.first;
            uint32_t neighborEdgeCost = neighborPair.second;
            if (!visited[neighborCoords]) {
                if (costToPred[neighborCoords] > neighborEdgeCost) {
                    if (costToPred[neighborCoords] != UINT32_MAX) {
                        priorityQueue.erase(priorityQueue.find(std::make_pair(costToPred[neighborCoords], neighborCoords)));
                    }
                    costToPred[neighborCoords] = neighborEdgeCost;
                    priorityQueue.insert(std::make_pair(neighborEdgeCost, neighborCoords));
                }
            }
        }
    }
    return MSTcost;
}

int main() {
    uint32_t rows{}, cols{}, potentialVertexCount{}, addedEdges{};
    std::cin >> rows >> cols >> potentialVertexCount >> addedEdges;
    VertexGrid grid(rows, cols);
    std::vector<std::pair<uint32_t, uint64_t>> verticesWithPotential;
    verticesWithPotential.reserve(potentialVertexCount);

    for(size_t i = 0; i < potentialVertexCount; ++i) {
        uint32_t row{}, col{}, potential{};
        std::cin >> row >> col >> potential;
        --row;
        --col;
        grid.costGrid[grid.getIdx(row, col)] = {0, potential};
        verticesWithPotential.emplace_back(potential, grid.getIdx(row, col));
    }

    for (size_t i=0; i < addedEdges; ++i) {
        uint32_t fRow{}, fCol{}, tRow{}, tCol{};
        std::cin >> fRow >> fCol >> tRow >> tCol;
        uint64_t fromPos = grid.getIdx(fRow-1, fCol-1);
        uint64_t toPos = grid.getIdx(tRow-1, tCol-1);
        grid.additionalEdges[fromPos] = toPos;
        grid.additionalEdges[toPos] = fromPos;
    }

    std::sort(verticesWithPotential.begin(), verticesWithPotential.end());
    std::vector<uint64_t> sortedVerticesPotential;
    std::transform(verticesWithPotential.begin(), verticesWithPotential.end(), std::back_inserter(sortedVerticesPotential), [](std::pair<uint32_t, uint64_t>& el) {return el.second;});

    Graph graph = findPotentialsCreateGraph(grid, sortedVerticesPotential);

    std::cout << primMSTcost(graph) << std::endl;
}