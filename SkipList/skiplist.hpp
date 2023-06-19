#include <array>
#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <vector>

namespace mzi {
// \frac{1}{4}
constexpr int P = 4;
constexpr int S = 0xFFFF;
constexpr int PS = S / P;
constexpr int MAX_LEV = 32;

template <typename K, typename V, typename Comp = std::less<K>>
struct SkipList {

  struct SkipListNode {
    K key;
    V value;
    std::vector<std::shared_ptr<SkipListNode>> forward;
    SkipListNode(K key, V value, int level) : key(key), value(value) {
      forward.reserve(level + 1);
    }
  };

  using Nptr = std::shared_ptr<SkipListNode>;

  struct Iter {
    Nptr value;
    // ++iter
    auto operator++() -> Iter { return *this = Iter{value->forward[0]}; }
    // iter++
    auto operator++(int) -> Iter {
      auto res = Iter{value};
      value = value->forward[0];
      return res;
    }
    auto operator*() -> std::pair<K, V> { return {value->key, value->value}; }
  };

private:
  Nptr head, tail;
  int cur_level;
  Comp cmp;
  std::array<Nptr, MAX_LEV + 1> update{};
  std::mt19937 rng{std::random_device{}()};

public:
  SkipList();
  auto random_level() -> int;
  auto find(K key) -> std::pair<Nptr, bool>;
  auto insert(K key, V value) -> bool;
  auto erase(K key) -> bool;
  auto begin() -> Iter;
  auto end() -> Iter;
  auto operator[](K key) -> V &;
  auto contains(K key) -> bool;
};

template <typename K, typename V, typename Comp>
SkipList<K, V, Comp>::SkipList()
    : head(std::make_shared<SkipListNode>(K{}, V{}, MAX_LEV)),
      tail(std::make_shared<SkipListNode>(K{}, V{}, MAX_LEV)), cur_level(0) {
  for (int i = 0; i <= MAX_LEV; ++i) {
    head->forward[i] = tail;
  }
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::random_level() -> int {
  int level = 0;
  while ((rng() & S) < PS)
    ++level;
  return level > MAX_LEV ? MAX_LEV : level;
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::find(K key) -> std::pair<Nptr, bool> {
  Nptr tmp = head;
  for (int i = cur_level; i >= 0; --i) {
    while (tmp->forward[i] != tail && cmp(tmp->forward[i]->key, key))
      tmp = tmp->forward[i];
    update[i] = tmp;
  }
  if (tmp->forward[0] == tail || tmp->forward[0]->key != key) {
    return {tmp, false};
  }
  return {tmp, true};
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::insert(K key, V value) -> bool {
  auto [tmp, status] = find(key);
  operator[](key) = value;
  return true;
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::erase(K key) -> bool {
  auto [tmp, status] = find(key);
  if (!status) {
    return false;
  }
  for (int i = 0; i <= cur_level; ++i) {
    if (update[i]->forward[i] != tmp->forward[i])
      break;
    update[i]->forward[i] = tmp->forward[i]->forward[i];
  }
  return true;
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::begin() -> Iter {
  return Iter{head->forward[0]};
}
template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::end() -> Iter {
  return tail;
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::operator[](K key) -> V & {
  auto [tmp, status] = find(key);
  if (status) {
    return tmp->forward[0]->value;
  }
  auto node_level = [&](int res) {
    if (res > cur_level) {
      res = ++cur_level;
      update[res] = head;
    }
    return res;
  }(random_level());
  Nptr new_node = std::make_shared<SkipListNode>(key, V{}, node_level);
  for (int i = node_level; i >= 0; --i) {
    new_node->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = new_node;
  }
  return new_node->value;
}

template <typename K, typename V, typename Comp>
auto SkipList<K, V, Comp>::contains(K key) -> bool {
  auto [tmp, status] = find(key);
  return status;
}
} // namespace mzi
