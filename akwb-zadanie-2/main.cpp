#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include <string>
#include <map>
#include <sstream>


struct Graph {
  Graph() {};
  Graph(std::ifstream& input_file) {
    for (std::string line; std::getline(input_file, line); ) {
      std::istringstream stream(line);
      std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream),
                                       std::istream_iterator<std::string>() };

      successors[tokens[0]];
      predecessors[tokens[0]];

      if (tokens.size() == 3 && tokens[1] == "->") {
        successors[tokens[2]];
        successors[tokens[0]].insert(tokens[2]);
        predecessors[tokens[2]].insert(tokens[0]);
      }
    }
  }

  std::pair<bool, bool> adjoint_linear() {
    bool linear = true;
    for (auto i = successors.begin(); i != successors.end(); ++i) {
      for (auto j = successors.begin(); j != i; ++j) {
        std::vector<std::string> successors_intersection;
        std::set_intersection(i->second.begin(), i->second.end(),
                              j->second.begin(), j->second.end(),
                              std::back_inserter(successors_intersection));

        if (!successors_intersection.empty() && i->second != j->second)
          return { false, false };

        std::vector<std::string> predecessors_intersection;
        std::set_intersection(predecessors[i->first].begin(), predecessors[i->first].end(),
                              predecessors[j->first].begin(), predecessors[j->first].end(),
                              std::back_inserter(predecessors_intersection));

        if (!successors_intersection.empty() && !predecessors_intersection.empty())
          linear = false;
      }
    }

    return { true, linear };
  }

  Graph get_original() {
    int index = 1;
    std::map<std::string, std::pair<int, int>> edges;
    for (const auto& each : successors)
      edges[each.first] = { index++, index++ };

    for (const auto& [vertex, _successors] : successors)
      for (const auto& successor : _successors) {
        int replacement = edges[vertex].second;
        int old_value = edges[successor].first;

        std::cout << vertex << "(" << edges[vertex].first << ", " << edges[vertex].second << ") "
          << " -> " << successor << "(" << edges[successor].first << ", " << edges[successor].second << "):\n";

        for (auto& [edge_vertex, indexes] : edges) {
          if (indexes.first == old_value) {
            std::cout << " replacing " << edge_vertex << " (" << indexes.first << ", " << indexes.second << ") with "
              << edge_vertex << "(" << replacement << ", " << indexes.second << ")\n";
            indexes.first = replacement;
          }
          if (indexes.second == old_value) {
            std::cout << " replacing " << edge_vertex << " (" << indexes.first << ", " << indexes.second << ") with "
              << edge_vertex << "(" << indexes.first << ", " << replacement << ")\n";
            indexes.second = replacement;
          }
        }
      }

    Graph original_graph;
    for (const auto& [vertex, _edges] : edges) {
      std::cout << _edges.first << " -> " << _edges.second << " [label=\"" << vertex << "\"]\n";
      original_graph.successors[std::to_string(_edges.second)];
      original_graph.successors[std::to_string(_edges.first)]
                        .insert(std::to_string(_edges.second));
    }

    return original_graph;
  }

  friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

  std::map<std::string, std::multiset<std::string>> successors{};
  std::map<std::string, std::multiset<std::string>> predecessors{};
};

std::ostream& operator<<(std::ostream& stream, const Graph& graph) {
  for (const auto& [vertex, successors] : graph.successors)
    if (!successors.empty())
      for (const auto& successor : successors)
        stream << vertex << " -> " << successor << "\n";
    else
      stream << vertex << "\n";
  return stream;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: akwb-zadanie-2.exe graph_filename\n";
    return -1;
  }

  std::ifstream input_file(argv[1]);
  Graph graph(input_file);

  auto [adjoint, linear] = graph.adjoint_linear();
  std::cout << "graf " << (adjoint ? "jest" : "nie jest") << " sprzezony\n";
  std::cout << "graf " << (linear ? "jest" : "nie jest") << " liniowy\n";

  if (adjoint) {
    std::ofstream output_file(std::string(argv[1]) + ".out");
    output_file << graph.get_original();
  }

  return 0;
}