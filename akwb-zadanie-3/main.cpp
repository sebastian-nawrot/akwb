#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>


struct Sequence {
  std::string sequence_id;
  std::vector<char> nucleotides;
  std::vector<int> reliabilities;
};

std::vector<Sequence> parse_sequences(std::ifstream& fasta_input, std::ifstream& qual_input) {
  std::vector<Sequence> sequences;
  std::string fasta_line, qual_line;
  while (std::getline(fasta_input, fasta_line, '>') && std::getline(qual_input, qual_line, '>')) {
    if (fasta_line.empty() || qual_line.empty())
      continue;

    std::string sequence_id = fasta_line.substr(0, fasta_line.find(' ') - 1);

    fasta_line.erase(0, fasta_line.find('\n'));
    fasta_line.erase(std::remove(fasta_line.begin(), fasta_line.end(), '\n'), fasta_line.end());
    std::vector<char> nucleotides{ fasta_line.begin(), fasta_line.end() };

    qual_line.erase(0, qual_line.find('\n'));
    std::istringstream int_iterator(qual_line);
    std::vector<int> reliabilities = { std::istream_iterator<int>(int_iterator),
                                       std::istream_iterator<int>() };

    //if (nucleotides.size() != reliabilities.size())
    // throw std::runtime_error("XD");

    sequences.emplace_back(sequence_id, nucleotides, reliabilities);
  }
  return sequences;
}

struct Vertex {
  Sequence& sequence;
  size_t position;
  std::string nucleotides;
};

struct Graph {
  Graph(std::vector<Sequence>& sequences, size_t length, size_t threshold) {
    for (auto& each : sequences)
      generate_vertexes(each, length, threshold);
    generate_edges();
  }

  void generate_vertexes(Sequence& sequence, size_t length, size_t threshold) {
    for (size_t i = 0; i < sequence.nucleotides.size(); ++i) {
      if (sequence.reliabilities[i] < threshold)
        continue;

      std::string sub_sequence{};
      for (size_t j = 0, left = length; j < left; ++j) {
        if (sequence.nucleotides.size() - i - j < left - j)
          return;

        if (sequence.reliabilities[i + j] < threshold)
          ++left;
        else
          sub_sequence += sequence.nucleotides[i + j];
      }
      vertexes.emplace_back(sequence, i, sub_sequence);
    }
  }

  void generate_edges() {
    for (size_t i = 0; i < vertexes.size(); ++i)
      for (size_t j = 0; j < vertexes.size(); ++j)
        if (i != j)
          if (vertexes[i].nucleotides == vertexes[j].nucleotides)
            if (&vertexes[i].sequence != &vertexes[j].sequence)
              edges[&vertexes[i]].push_back(&vertexes[j]);
  }

  std::vector<Vertex*> generate_clique() {
    Vertex* current_vertex = nullptr;
    for (const auto& [vertex, adjacent_vertexes] : edges)
      if (adjacent_vertexes.size() > edges[current_vertex].size())
        current_vertex = vertex;

    if (!current_vertex)
      return {};

    std::vector<Sequence*> sequences{ &current_vertex->sequence };
    std::vector<Vertex*> clique{ current_vertex };

    bool finished = false;
    while (!finished) {
      finished = true;
      for (const auto& adjacent_vertex : edges[clique.back()]) {
        if (std::find(clique.begin(), clique.end(), adjacent_vertex) != clique.end())
          continue;
        if (std::find(sequences.begin(), sequences.end(), &adjacent_vertex->sequence) != sequences.end())
          continue;

        clique.push_back(adjacent_vertex);
        sequences.push_back(&adjacent_vertex->sequence);
        finished = false;
        break;
      }
    }

    return clique;
  }

  std::vector<Vertex> vertexes{};
  std::map<Vertex*, std::vector<Vertex*>> edges{};
};

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Usage: akwb-zadanie-3.exe sample_name length threshold\n";
    return -1;
  }
  
  std::ifstream fasta_input(std::string(argv[1]) + ".fasta");
  std::ifstream qual_input(std::string(argv[1]) + ".qual");

  auto sequences = parse_sequences(fasta_input, qual_input);
  Graph graph(sequences, std::stoi(argv[2]), std::stoi(argv[3]));
  
  for (const auto& vertex : graph.generate_clique())
    std::cout << vertex->sequence.sequence_id << " "
      << vertex->position << " " << vertex->nucleotides << "\n";

  return 0;
}