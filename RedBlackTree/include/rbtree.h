#pragma once
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>

namespace rbt {
namespace details {
enum class Color : uint8_t {
  black,
  red,
};

template <typename K, typename V> struct Node {
  K key;
  V value;
  Color color;
  std::weak_ptr<Node<K, V>> f_node;   // father
  std::shared_ptr<Node<K, V>> l_node; // left child
  std::shared_ptr<Node<K, V>> r_node; // right child
};

template <typename K, typename V>
auto get_father(std::shared_ptr<Node<K, V>> node)
    -> std::shared_ptr<Node<K, V>>{};
template <typename K, typename V>
auto get_grandfather(std::shared_ptr<Node<K, V>> node)
    -> std::shared_ptr<Node<K, V>>{};
template <typename K, typename V>
auto rotate_left(std::shared_ptr<Node<K, V>> node) -> void {}
template <typename K, typename V>
auto rotate_right(std::shared_ptr<Node<K, V>> node) -> void {}
} // namespace details

template <typename K, typename V, typename Compare = std::less<K>()>
class Rbtree {
public:
  Rbtree() = default;
  Rbtree(const Rbtree &value);
  auto operator=(const Rbtree &value) -> Rbtree &;
  Rbtree(Rbtree &&value);
  auto operator=(Rbtree &&value) -> Rbtree &;

  auto insert(K key, V value) -> void;
  auto remove(K key) -> void;
  auto modify(K key, V value) -> void;
  auto contains(K key) -> bool;
  auto operator[](const K &key) -> V &;
  auto swap(Rbtree &value) -> void;

private:
  std::shared_ptr<details::Node<K, V>> root_node;
};

template <typename K, typename V, typename Compare>
Rbtree<K, V, Compare>::Rbtree(const Rbtree &value) {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::operator=(const Rbtree &value) -> Rbtree & {}
template <typename K, typename V, typename Compare>
Rbtree<K, V, Compare>::Rbtree(Rbtree &&value) {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::operator=(Rbtree &&value) -> Rbtree & {}

template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::insert(K key, V value) -> void {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::remove(K key) -> void {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::modify(K key, V value) -> void {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::contains(K key) -> bool {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::operator[](const K &key) -> V & {}
template <typename K, typename V, typename Compare>
auto Rbtree<K, V, Compare>::swap(Rbtree &value) -> void {}
} // namespace rbt
