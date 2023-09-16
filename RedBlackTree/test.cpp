#include <assert.h>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <rbtree.h>

constexpr int N = 1e6;
#define CLAC_TIME(start)                                                       \
  std::chrono::duration_cast<std::chrono::milliseconds>(                       \
      std::chrono::steady_clock::now() - start)                                \
      .count()

auto test_map() -> void {
  std::map<int, int> map;
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i <= N; ++i) {
    map.insert({i, i});
  }
  std::cout << std::format("map insert : {} ms\n", CLAC_TIME(start));
  start = std::chrono::steady_clock::now();
  for (int i = 0; i <= N; ++i) {
    if (i & 1) {
      assert(map[i] == i);
    } else {
      map.erase(i);
    }
  }
  std::cout << std::format("map remove : {} ms\n", CLAC_TIME(start));
}

auto test_rbt() -> void {
  rbt::Rbtree<int, int> rbt;
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i <= N; ++i) {
    rbt.insert(i, i);
  }
  std::cout << std::format("rbt insert : {} ms\n", CLAC_TIME(start));
  start = std::chrono::steady_clock::now();
  for (int i = 0; i <= N; ++i) {
    if (i & 1) {
      assert(rbt[i] == i);
    } else {
      rbt.remove(i);
    }
  }
  std::cout << std::format("rbt remove : {} ms\n", CLAC_TIME(start));
}
auto main() -> int {
  test_map();
  test_rbt();
}
