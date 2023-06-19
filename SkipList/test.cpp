#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include "skiplist.hpp"
#include <map>

constexpr int N = 1e5;
auto main() -> int {
  std::map<int, int> M;
  mzi::SkipList<int, int> L;

  std::clock_t s = std::clock();
  std::mt19937 rng(std::random_device{}());
  for (int i = 0; i < N; ++i) {
    int key = rng(), value = rng();
    M[key] = value;
    L[key] = value;
  }
  for (int i = 0; i < N; ++i) {
    int key =rng();
    if (i & 1) {
      L.erase(key);
      M.erase(key);
    } else {
      int r1 = L.find(key) ? L[key] : 0;
      int r2 = M.count(key) ? M[key] : 0;
      assert(r1 == r2);
    }
  }
  std::clock_t e = std::clock();
  std::cout << std::fixed << std::setprecision (7);
  // about 0.2s/
  std::cout << "Time elapsed: " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;
}
