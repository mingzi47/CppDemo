#include <assert.h>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <rbtree.h>

constexpr int N = 1e5;
#define CLAC_TIME(start)                                                       \
  std::chrono::duration_cast<std::chrono::milliseconds>(                       \
      std::chrono::steady_clock::now() - start)                                \
      .count()

auto main() -> int {
  auto start = std::chrono::steady_clock::now();
  std::map<int, int> map;
  rbt::Rbtree<int, int> rbt;
  std::cout << "\n TEST START \n";

  std::cout << "\n TEST Insert \n";
  for (int i = 0; i <= N; ++i) {
    // map.insert({i, i});
    rbt.insert(i, i);
  }


  std::cout << "Insert : " << CLAC_TIME(start) << " ms \n";

  auto start2 = std::chrono::steady_clock::now();

  std::cout << "\n TEST Find and Remove \n";
  for (int i = 0; i <= N; ++i) {
    if (i & 1) {
      // assert(map[i] == rbt[i]);
      // assert(map[i] == i);
      assert(rbt[i] == i);
    } else {
      // map.erase(i);
      // rbt.remove(i);
    }
  }
  rbt.draw("test.dot");

  std::cout << "Find and Remove : " << CLAC_TIME(start2) << " ms \n";

  std::cout << "Total : " << CLAC_TIME(start) << " ms \n";
  std::cout << "\n TEST END \n";
}
