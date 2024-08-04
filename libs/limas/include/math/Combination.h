#pragma once
#include "system/Singleton.h"

namespace limas {
namespace math {

class Combination {
 public:
  friend class Singleton<Combination>;

  std::vector<std::vector<int>> getCombinations(const std::vector<int>& input,
                                                int r) {
    std::vector<std::vector<int>> combinations;
    generateCombinations(input, r,
                         [&combinations](const std::vector<int>& combination) {
                           combinations.push_back(combination);
                         });
    return combinations;
  }

 private:
  Combination() {}

  void generateCombinationsUtil(
      const std::vector<int>& input, int r, int index, int data_index,
      std::vector<int>& data,
      const std::function<void(const std::vector<int>&)>& callback) {
    if (data_index == r) {
      callback(data);
      return;
    }

    if (index >= input.size()) {
      return;
    }

    data[data_index] = input[index];
    generateCombinationsUtil(input, r, index + 1, data_index + 1, data,
                             callback);
    generateCombinationsUtil(input, r, index + 1, data_index, data, callback);
  }

  void generateCombinations(
      const std::vector<int>& input, int r,
      const std::function<void(const std::vector<int>&)>& callback) {
    std::vector<int> data(r);
    generateCombinationsUtil(input, r, 0, 0, data, callback);
  }
};

inline std::vector<std::vector<int>> getCombinations(
    const std::vector<int>& input, int r) {
  return Singleton<Random>::getInstance().getCombinations(input, r);
}

inline std::vector<std::vector<int>> getPermutations(std::vector<int> input) {
  std::vector<std::vector<int>> permutations;
  do {
    permutations.push_back(input);
  } while (std::next_permutation(input.begin(), input.end()));
  return permutations;
}

}  // namespace math
}  // namespace limas
