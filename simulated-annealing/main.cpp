#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>


class Graph {
public:
    using Vertex = size_t;
    using VertexSet = std::unordered_set<Vertex>;
    using AdjacencyList = std::unordered_map<Vertex, VertexSet>;

    void AddVertex(Vertex v) {
        adjacency_list_[v];
    }

    void AddEdge(Vertex u, Vertex v) {
        adjacency_list_[u].insert(v);
        adjacency_list_[v].insert(u);
    }

    const VertexSet& AdjacentVertices(Vertex v) const {
        const auto it = adjacency_list_.find(v);
        if (it != adjacency_list_.end()) {
            return it->second;
        } else {
            return empty_set_;
        }
    }

    VertexSet AllVertices() const {
        VertexSet vs;
        vs.reserve(adjacency_list_.size());
        for (const auto& pair : adjacency_list_) {
            const auto& vertex = pair.first;
            vs.insert(vertex);
        }
        return vs;
    }

    const AdjacencyList& AsAdjacencyList() const {
        return adjacency_list_;
    }

    static const VertexSet empty_set_;
private:
    AdjacencyList adjacency_list_;
};

const Graph::VertexSet Graph::empty_set_;

class VertexCut {
public:
    explicit VertexCut(const Graph& graph)
            : graph_(graph), set_A_(graph.AllVertices()), set_B_(Graph::empty_set_) {}

    const Graph::VertexSet& GetSetA() const {
        return set_A_;
    }

    const Graph::VertexSet& GetSetB() const {
        return set_B_;
    }

    const Graph& GetGraph() const {
        return graph_;
    }


    const int GetDeltaAfterMove(Graph::Vertex v) { // Delta of Max Cut after moving vertex v from one part to other
        int delta = 0;
        if (set_A_.find(v) != set_A_.end()) {
            for (const auto& u : graph_.AdjacentVertices(v)) {
                if (set_A_.find(u) != set_A_.end()) {
                    delta++;
                } else {
                    delta--;
                }
            }
        } else if (set_B_.find(v) != set_B_.end()) {
            for (const auto& u : graph_.AdjacentVertices(v)) {
                if (set_B_.find(u) != set_B_.end()) {
                    delta++;
                } else {
                    delta--;
                }
            }
        }
        return delta;
    }

    void MoveVertex(Graph::Vertex v) {
        if (set_A_.find(v) != set_A_.end()) {
            set_A_.erase(v);
            set_B_.insert(v);
        } else if (set_B_.find(v) != set_B_.end()) {
            set_A_.insert(v);
            set_B_.erase(v);
        }
        return;
    }

    size_t GetSizeOfCut() const {
        size_t cut_size = 0;
        for (const auto& v : set_A_) {
            const auto& neighbours = graph_.AdjacentVertices(v);
            for (const auto& u : set_B_) {
                if (neighbours.find(u) != neighbours.end()) {
                    cut_size++;
                }
            }
        }
        return cut_size;
    }

private:

    const Graph& graph_;
    Graph::VertexSet set_A_, set_B_;
};


class MaxCutSolver {
public:
    virtual VertexCut Solve(const Graph& graph) const = 0;
    virtual ~MaxCutSolver() = default;
};

class GreedySolver final: public MaxCutSolver {
    VertexCut Solve(const Graph& graph) const {
        VertexCut vc(graph);
        int delta = 0;
        bool found = 1;
        Graph::Vertex will_be_moved;

        while (found) {
            found = 0;
            for (const auto &elem : vc.GetSetA()) {
                int d = vc.GetDeltaAfterMove(elem);
                if (d > delta) {
                    delta = d;
                    will_be_moved = elem;
                    found = 1;
                }
            }

            if (!found) {
                for (const auto &elem : vc.GetSetB()) {
                    int d = vc.GetDeltaAfterMove(elem);
                    if (d > delta) {
                        delta = d;
                        will_be_moved = elem;
                        found = 1;
                    }
                }
            }

            delta = 0;
            if (found)
                vc.MoveVertex(will_be_moved);
        }
        return vc;
    }
};

class  AnnealingSolver final: public MaxCutSolver {
 private:
    double T0;

 public:
    AnnealingSolver(double t) : T0(t) {}

    VertexCut Solve(const Graph& graph) const {
        VertexCut vc(graph);
        auto will_be_moved = vc.GetSetA().begin();
        double temp_T = T0;
        for (size_t i = 0; i != 5000; ++i) {
            if (will_be_moved == vc.GetSetA().end())
                will_be_moved = vc.GetSetB().begin();
            else if (will_be_moved == vc.GetSetB().end())
                return vc;
            int delta = vc.GetDeltaAfterMove(*will_be_moved);
            if (delta > 0 || double(rand()) / RAND_MAX <= std::exp(delta / temp_T)) {
                vc.MoveVertex(*will_be_moved);
                will_be_moved = vc.GetSetA().begin();
            } else {
                ++will_be_moved;
            }
            temp_T = T0 / i;
        }
        return vc;
    }
};

Graph RandomGraph(size_t size, double edge_probability) {
    Graph graph;
    for (Graph::Vertex v = 1; v <= size; ++v) {
        graph.AddVertex(v);
    }
    for (Graph::Vertex v = 1; v <= size; ++v) {
        for (Graph::Vertex u = v + 1; u <= size; ++u) {
            if (double(rand()) / RAND_MAX <= edge_probability) {
                graph.AddEdge(v, u);
            }
        }
    }
    return graph;
}

Graph StarGraph(size_t size) {
    Graph graph;
    for (Graph::Vertex v = 2; v <= size; ++v) {
        graph.AddEdge(1, v);
    }
    return graph;
}

Graph BipartiteGraph(size_t size, double edge_probability) {
    Graph graph;
    Graph::VertexSet left;
    Graph::VertexSet right;
    for (Graph::Vertex v = 1; v <= size; ++v) {
        graph.AddVertex(v);
        if (double(rand()) / RAND_MAX <= 0.5) {
            left.insert(v);
        } else {
            right.insert(v);
        }
    }

    for (const auto& v : left) {
        for (const auto& u : right) {
            if (double(rand()) / RAND_MAX <= edge_probability) {
                graph.AddEdge(v, u);
            }
        }
    }


    return graph;
}

int InitRandSeed(int argc, const char* argv[]) {
    int rand_seed;
    if (argc >= 2) {
        rand_seed = atoi(argv[1]);
    } else {
        rand_seed = time(nullptr);
    }
    srand(rand_seed);
    return rand_seed;
}

int TrySolver(const MaxCutSolver& solver, const std::vector<Graph>& graphs) {
    int result = 0;
    for (const auto& graph : graphs) {
        const auto vertex_cut = solver.Solve(graph);
        auto cost = vertex_cut.GetSizeOfCut();
        result += cost;
    }

    std::cout << result << ' ';
    return result;
}

int main(int argc, const char* argv[]) {

    // std::ofstream out;
    // out.open("output.txt");
    size_t number_of_v = 100;
    std::cout << "Using rand seed: " << InitRandSeed(argc, argv) << "\t Number of vertexes is: " << number_of_v << "\n";
    std::vector<double> v = {0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000};
    std::vector<Graph> graphs;
    for (size_t i = 0; i != 20; ++i)
        graphs.push_back(RandomGraph(number_of_v, 0.7));
    for (size_t i = 0; i != 20; ++i)
        graphs.push_back(BipartiteGraph(number_of_v, 0.7));
    for (size_t i = 0; i != 10; ++i)
        graphs.push_back(StarGraph(number_of_v));
    GreedySolver greedy_solver;

    std::cout << "Greedy solver: ";
    int x1 = TrySolver(greedy_solver, graphs);
    std::cout << std::endl;
    for (const auto &t : v) {
        AnnealingSolver annealing_solver(t);
        std::cout << "temperature: " << t << '\t';
        int x2 = TrySolver(annealing_solver, graphs);
        std::cout << "distance between solvers: " << x1 - x2 << std::endl;

    }
}
