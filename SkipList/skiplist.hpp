#include <memory>

namespace mzi {
// \frac{1}{4}
constexpr int P = 4;
constexpr int S = 0xFFFF;
constexpr int PS = P / S;
constexpr int MAX_LEV = 32;

template <typename K, typename V, typename Comp = std::less<K>()>
struct SkipList {
  using SNode = struct {};

private:
public:
  SkipList();
  auto random_level() -> int;
  auto find(K key) -> V;
  auto insert(K key, V value) -> bool;
  auto erase(K key) -> bool;
  auto begin() -> V;
  auto end() -> V;
  auto operator[](K key) -> V&;
};

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::random_level() -> int {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::find(K key) -> V {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::insert(K key, V value) -> bool {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::erase(K key) -> bool {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::begin() -> V {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::end() -> V {}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::operator[](K key) -> V& {}
} // namespace mzi
