#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include "skiplist.hpp"
#include <map>

constexpr int N = 1e5;

//NOtE N = 1e5 about 0.2s/

auto main() -> int {
  std::cout << std::fixed << std::setprecision (7);
  std::clock_t s = std::clock();
  std::map<int, int> M;
  mzi::SkipList<int, int> L;

  std::clock_t s0 = std::clock();
  std::mt19937 rng(std::random_device{}());
  for (int i = 0; i < N; ++i) {
    int key = rng(), value = rng();
    M[key] = value;
    L[key] = value;
    // L.insert(key, value);
  }

  std::clock_t e0 = std::clock();
  std::cout << "Insert Time elapsed: " << (double)(e0 - s0) / CLOCKS_PER_SEC << std::endl;

  std::clock_t s1 = std::clock();
  auto iter = L.begin();
  for (auto [k, v] : M) {
    assert(k == (*iter).first);
    assert(v == (*iter).second);
    ++iter;
  }

  std::clock_t e1 = std::clock();
  std::cout << "For Time elapsed: " << (double)(e1 - s1) / CLOCKS_PER_SEC << std::endl;

  std::clock_t s2 = std::clock();
  for (int i = 0; i < N; ++i) {
    int key =rng();
    if (i & 1) {
      L.erase(key);
      M.erase(key);
    } else {
      bool r1 = L.contains(key);
      bool r2 = M.contains(key);
      assert(r1 == r2);
    }
  }
  std::clock_t e2 = std::clock();
  std::cout << "Erase And Find Time elapsed: " << (double)(e2 - s2) / CLOCKS_PER_SEC << std::endl;


  std::clock_t e = std::clock();
  std::cout << "Time elapsed: " << (double)(e - s) / CLOCKS_PER_SEC << std::endl;

}
