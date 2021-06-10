#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <numeric>
#include <optional>


std::optional<std::vector<int>> find(int size,
  std::multiset<int> instance, const std::vector<int>& map) {
  if (instance.contains(size)) {
    instance.erase(instance.find(size));
    if (instance.empty())
      return map;

    for (auto i = map.begin(); i != map.end(); ++i) {
      auto sum = std::accumulate(i, map.end(), 0);
      if (!instance.contains(sum))
        return {};
      instance.erase(instance.find(sum));
    }

    for (const auto& each : instance) {
      std::vector<int> new_map = map;
      new_map.push_back(each);
      if (auto result = find(size - *map.rbegin(), instance, new_map))
        return result;
    }
  }

  return {};
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: akwb-zadanie-4.exe file_name\n";
    return -1;
  }

  std::ifstream input_file(argv[1]);
  std::multiset<int> instance{ std::istream_iterator<int>(input_file),
                               std::istream_iterator<int>() };

  auto k = (-3 + sqrt(8 * instance.size() + 1)) / 2;
  if (k - floor(k) != 0) {
    std::cout << "Bledy na wejsciu\n";
    return -1;
  }

  std::cout << "Przewidywana dlugosc mapy: " << k + 1 << "\n";

  auto x = *instance.rbegin() - *(++instance.rbegin());
  if (auto result = find(*instance.rbegin(), instance, { x })) {
    std::cout << "mapa = (";
    for (const auto& each : result.value())
      std::cout << each << ", ";
    std::cout << ")\n";
  }
  else
    std::cout << "Brak rozwiazan\n";

  return 0;
}