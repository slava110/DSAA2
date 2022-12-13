// Построение остовного дерева алгоритмом Прима
#include <cstring>
#include <iostream>
#include <algorithm>
//#include <vector>

/*class Graph {

    struct GVertex;
    struct GVertexEdge;

    struct GVertex {
        const std::vector<GVertexEdge>& edges;

        explicit GVertex(const std::vector<GVertexEdge>& edges): edges(edges) {}

        const GVertexEdge& findMinimalEdge(std::vector<GVertex> visited, std::vector<GVertexEdge> visitedEdges) const {
            const GVertexEdge* minimalEdge = nullptr;

            for (const auto &edge : edges) {
                if(std::find(visited.begin(), visited.end(), edge.second) != visited.end()) {
                    if(std::find(visitedEdges.begin(), visitedEdges.end(), edge) != visitedEdges.end()) {
                        if(edge.cost < minimalEdge->cost) {
                            minimalEdge = &edge;
                        }
                    }
                }
            }

            return *minimalEdge;
        }
    };

    struct GVertexEdge {
        GVertex* second;
        int cost;

        GVertexEdge(GVertex *first, int cost) : second(second), cost(cost) {}

    };

    std::vector<GVertex> verticies;
public:

    explicit Graph(std::vector<GVertex> verticies) : verticies(std::move(verticies)) {}

    explicit Graph(int** adjMap, int verticiesAmount) {

    }

    void addVertex(const GVertex& attachTo, int cost) {

    }

    void addEdge(const GVertex& first, const GVertex& second, int cost) {

    }

    Graph* createMinimumSpanningTree() {
        std::vector<GVertex> visited(verticies.size());
        std::vector<GVertexEdge> visitedEdges;

        visited.emplace_back(verticies.front());

        while (visited.size() != verticies.size()) {
            const GVertexEdge* minimalEdge = nullptr;

            for (const auto &vert : verticies) {
                if(std::find(visited.begin(), visited.end(), vert) != visited.end()) {
                    const GVertexEdge& candidate = vert.findMinimalEdge(visited, visitedEdges);

                    if(candidate.cost < minimalEdge->cost) {
                        minimalEdge = &candidate;
                    }
                }
            }

            visited.emplace_back(*minimalEdge->second);
            visitedEdges.emplace_back(*minimalEdge);
        }

        return new Graph(primVerticies);
    }

    void display() {

    }
};*/

int main() {
    const int V = 5;
    const int G[V][V] = {
            {1 ,0, 0, 3, 15},
            {7, 0, 1, 4, 25},
            {12, 4, 1, 7, 10},
            {10, 6, 24, 1, 42},
            {5, 3, 10, 4, 6}
    };

    int edges = 0;
    int selected[V];

    memset(selected, false, sizeof(selected));
    selected[0] = true;
    int x, y;
    std::cout << "Edge : Weight" << std::endl;

    while(edges < V - 1) {
        int min = INT_MAX;
        x = 0;
        y = 0;
        for(int i = 0; i < V; i++) {
            if(selected[i]) {
                for(int j = 0; j < V; j++) {
                    if(!selected[j] && G[i][j] && min > G[i][j]) {
                        min = G[i][j];
                        x = i;
                        y = j;
                    }
                }
            }
        }
        std::cout << x << " - " << y << " : " << G[x][y] << std::endl;
        selected[y] = true;
        edges++;
    }
}