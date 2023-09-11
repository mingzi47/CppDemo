#pragma once
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>

#define TEMPLATE_NODE template <typename K, typename V>
#define TEMPLATE_RBT_M                                                         \
  template <typename K, typename V, typename Compare>                          \
  auto Rbtree<K, V, Compare>::
#define TEMPLATE_RBT_C                                                         \
  template <typename K, typename V, typename Compare> Rbtree<K, V, Compare>::

namespace rbt {
namespace details {
enum class Attribute : uint8_t {
  ROOT,
  LEFT_CHILD,
  RIGHT_CHILD,
};

enum class Color : uint8_t {
  BLACK,
  RED,
};

template <typename K, typename V> struct Node {
  K key;
  V value;
  Attribute attribute;
  Color color;
  std::weak_ptr<Node<K, V>> f_node;   // father
  std::shared_ptr<Node<K, V>> l_node; // left child
  std::shared_ptr<Node<K, V>> r_node; // right child

  Node() = default;
  Node(K key, V value, Attribute attribute, Color color,
       std::shared_ptr<Node<K, V>> f_node, std::shared_ptr<Node<K, V>> l_node,
       std::shared_ptr<Node<K, V>> r_node)
      : key(key), value(value), attribute(attribute), color(color),
        f_node(f_node), l_node(l_node), r_node(r_node){};
};

TEMPLATE_NODE
auto get_father(const std::shared_ptr<Node<K, V>> &node)
    -> std::shared_ptr<Node<K, V>> {
  return node->f_node.lock();
}

TEMPLATE_NODE
auto get_uncle(const std::shared_ptr<Node<K, V>> &node)
    -> std::shared_ptr<Node<K, V>> {
  auto father = get_father(node);
  if (father == nullptr)
    return nullptr;
  auto grandfather = get_father(father);
  if (grandfather == nullptr)
    return nullptr;

  if (father->attribute == Attribute::LEFT_CHILD) {
    return grandfather->r_node;
  }
  return grandfather->l_node;
}

TEMPLATE_NODE
auto get_grandfather(const std::shared_ptr<Node<K, V>> &node)
    -> std::shared_ptr<Node<K, V>> {
  auto father = get_father(node);
  if (father != nullptr)
    return get_father(father);
  return nullptr;
}

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

  // debug
  auto draw(std::filesystem::path pathname) -> void;

private:
  // op 1 insert 0 find
  auto details_find(K key) -> std::shared_ptr<details::Node<K, V>>;
  auto details_insert(K key, V value) -> std::shared_ptr<details::Node<K, V>>;
  auto switch_insert(std::shared_ptr<details::Node<K, V>> &node) -> void;
  auto rotate_left(std::shared_ptr<details::Node<K, V>> &node) -> void;
  auto rotate_right(std::shared_ptr<details::Node<K, V>> &node) -> void;

private:
  std::shared_ptr<details::Node<K, V>> root_node;
};

TEMPLATE_RBT_M rotate_left(std::shared_ptr<details::Node<K, V>> &X)->void {
  // clang-format off
  // X                  Y
  //  \                / \
  //   Y       =>     X   B
  //  / \              \
  // A   B              A
  // clang-format on
  auto Y = X->r_node;
  assert(Y != nullptr);
  if (Y == nullptr)
    return;
  auto f_X = get_father(X);

  if (f_X != nullptr) {
    switch (X->attribute) {
    case details::Attribute::LEFT_CHILD:
      f_X->l_node = Y;
      Y->attribute = details::Attribute::LEFT_CHILD;
      break;
    case details::Attribute::RIGHT_CHILD:
      f_X->r_node = Y;
      Y->attribute = details::Attribute::RIGHT_CHILD;
      break;
    default:
      throw;
    }
    Y->f_node = f_X;
  } else {
    Y->attribute = details::Attribute::ROOT;
    root_node = Y;
    {
      std::shared_ptr<details::Node<K, V>> clear;
      root_node->f_node = clear;
    }
  }

  X->r_node = Y->l_node;
  if (X->r_node != nullptr)
    X->r_node->attribute = details::Attribute::RIGHT_CHILD;
  X->f_node = Y;
  Y->l_node = X;
  X->attribute = details::Attribute::LEFT_CHILD;
}

TEMPLATE_RBT_M rotate_right(std::shared_ptr<details::Node<K, V>> &X)->void {
  // clang-format off
  //     X                 Y
  //    /                 / \
  //   Y         =>      A   X
  //  / \                   /
  // A   B                 B
  // clang-format on
  auto Y = X->l_node;
  assert(Y != nullptr);
  if (Y == nullptr)
    return;
  auto f_X = get_father(X);
  if (f_X != nullptr) {
    switch (X->attribute) {
    case details::Attribute::LEFT_CHILD:
      f_X->l_node = Y;
      break;
    case details::Attribute::RIGHT_CHILD:
      f_X->r_node = Y;
      break;
    default:
      throw;
    }
    Y->f_node = f_X;
  } else {
    Y->attribute = details::Attribute::ROOT;
    root_node = Y;
    {
      std::shared_ptr<details::Node<K, V>> clear;
      root_node->f_node = clear;
    }
  }
  X->l_node = Y->r_node;
  if (X->l_node != nullptr)
    X->l_node->attribute = details::Attribute::LEFT_CHILD;
  X->f_node = Y;
  Y->r_node = X;
  X->attribute = details::Attribute::RIGHT_CHILD;
}

TEMPLATE_RBT_M
details_find(K key)->std::shared_ptr<details::Node<K, V>> {
  if (root_node == nullptr)
    return nullptr;
  std::shared_ptr<details::Node<K, V>> res = nullptr;
  for (res = root_node; res != nullptr;) {
    if (res->key > key) {
      if (res->l_node == nullptr)
        return res;
      res = res->l_node;
    } else if (res->key < key){
      if (res->r_node == nullptr)
        return res;
      res = res->r_node;
    } else {
      return res;
    }
  }
  return nullptr;
}

TEMPLATE_RBT_M
details_insert(K key, V value)->std::shared_ptr<details::Node<K, V>> {
  // std::cout << std::format("insert key = {}, value = {}\n", key, value);
  if (root_node == nullptr) {
    root_node = std::make_shared<details::Node<K, V>>(
        key, value, details::Attribute::ROOT, details::Color::BLACK, nullptr,
        nullptr, nullptr);
    return root_node;
  }

  std::shared_ptr<details::Node<K, V>> res = details_find(key);
  assert(res != nullptr);
  if (res->key > key) {
    res->l_node = std::make_shared<details::Node<K, V>>(
        key, value, details::Attribute::LEFT_CHILD, details::Color::RED, res,
        nullptr, nullptr);
    return res->l_node;
  }
  res->r_node = std::make_shared<details::Node<K, V>>(
      key, value, details::Attribute::RIGHT_CHILD, details::Color::RED, res,
      nullptr, nullptr);
  return res->r_node;
}

TEMPLATE_RBT_C
Rbtree(const Rbtree &value) { root_node = value.root_node; }
TEMPLATE_RBT_M
operator=(const Rbtree &value)->Rbtree & { root_node = value.root_node; }
TEMPLATE_RBT_C
Rbtree(Rbtree &&value) { this->swap(value); }
TEMPLATE_RBT_M
operator=(Rbtree &&value)->Rbtree & { this->swap(value); }

TEMPLATE_RBT_M switch_insert(std::shared_ptr<details::Node<K, V>> &node)->void {
  auto uncle = get_uncle(node);
  auto father = get_father(node);
  auto grandfather = get_grandfather(node);
  const bool case1 = (father == nullptr);
  const bool case2 =
      (father != nullptr and father->color == details::Color::BLACK);

  const bool case3 =
      ((father != nullptr and father->color == details::Color::RED) and
       (uncle != nullptr and uncle->color == details::Color::RED));
  const bool tmp =
      ((uncle == nullptr or uncle->color == details::Color::BLACK) and
       (father != nullptr and father->color == details::Color::RED));
  const bool case4 = (tmp and node->attribute == father->attribute);
  const bool case5 = (tmp and node->attribute != father->attribute);
  // std::cout << std::format("key = {}, value = {}, case1 = {}, case2 = {}, "
  //                          "case3 = {}, case4 = {}, case5 = {}\n",
  //                          node->key, node->value, case1, case2, case3, case4,
  //                          case5);
  if (case1) {
    node->color = details::Color::BLACK;
    return;
  } else if (case2) {
    return;
  } else if (case3) {
    father->color = details::Color::BLACK;
    uncle->color = details::Color::BLACK;
    node = get_grandfather(node);
    node->color = details::Color::RED;
    switch_insert(grandfather);
  } else if (case4) {
    switch (node->attribute) {
    // clang-format off
    //        G(B)             N(B)
    //       /  \   right(G)  /  \
    //      N(R) U(B)  ===>  P(R) G(R)
    //     /                        \
    //    P(R)                       U(B)
    // clang-format on
    case details::Attribute::LEFT_CHILD:
      rotate_right(grandfather);
      break;
    // clang-format off
    //        G(B)             N(B)
    //       /  \    left(G)  /  \
    //      U(B) N(R)  ===>  G(R) P(R)
    //             \        /
    //              P(R)   U(B)
    // clang-format on
    case details::Attribute::RIGHT_CHILD:
      rotate_left(grandfather);
      break;
    }
    grandfather->color = details::Color::RED;
    father->color = details::Color::BLACK;
  } else if (case5) {
    switch (node->attribute) {
    // clang-format off
    //        G(B)              G(B)
    //       /  \    right(P)   /  \
    //      U(B) P(R)  ====>  U(B) N(R)
    //          /                    \
    //         N(R)                   P(R)
    // clang-format on
    case details::Attribute::LEFT_CHILD:
      rotate_right(father);
      break;
    // clang-format off
    //        G(B)                G(B)
    //       /   \    left(P)     /  \
    //      P(R)  U(B)  ====>   N(R) U(B)
    //       \                 /
    //        N(R)            P(R)
    // clang-format on
    case details::Attribute::RIGHT_CHILD:
      rotate_left(father);
      break;
    }
    // 最下面的点
    switch_insert(father);
  }
}

TEMPLATE_RBT_M insert(K key, V value)->void {
  auto node = details_insert(key, value);
  switch_insert(node);
}
TEMPLATE_RBT_M remove(K key)->void {}
TEMPLATE_RBT_M modify(K key, V value)->void {}
TEMPLATE_RBT_M contains(K key)->bool {
  auto res = details_find(key);
  if (res == nullptr)
    return false;
  return res->key == key;
}

TEMPLATE_RBT_M operator[](const K &key)->V & {
  auto res = details_find(key);
  if (res->key == key)
    return res->value;
  insert(key, V{});
  return details_find(key)->value;
}
TEMPLATE_RBT_M swap(Rbtree &value)->void {
  std::swap(root_node, value->root_node);
}

TEMPLATE_RBT_M draw(std::filesystem::path pathname)->void {
  std::fstream dot(pathname, std::ios::in | std::ios::out);
  dot << "digraph btree {\n";
  auto dfs = [&dot](auto dfs,
                    std::shared_ptr<details::Node<K, V>> &node) -> void {
    auto l_node = node->l_node;
    auto r_node = node->r_node;
    dot << std::format("node_{} [color={}, label=\"K={}, V={}\"];\n", node->key,
                       (node->color == details::Color::RED ? "red" : "black"),
                       node->key, node->value);
    if (l_node != nullptr) {
      dot << std::format(
          "node_{} [color={}, label=\"K={}, V={}\"];\nnode_{} -> node_{};\n",
          l_node->key, (l_node->color == details::Color::RED ? "red" : "black"),
          l_node->key, l_node->value, node->key, l_node->key);
      dfs(dfs, l_node);
    } else {
      dot << std::format(
          "l_nil_{} [color={}, label=\"Nil\"];\nnode_{} -> l_nil_{};\n",
          node->key, "black", node->key, node->key, node->key);
    }
    if (r_node != nullptr) {
      dot << std::format(
          "node_{} [color={}, label=\"K={}, V={}\"];\nnode_{} -> node_{};\n",
          r_node->key, (r_node->color == details::Color::RED ? "red" : "black"),
          r_node->key, r_node->value, node->key, r_node->key);
      dfs(dfs, r_node);
    } else {
      dot << std::format(
          "r_nil_{} [color={}, label=\"Nil\"];\nnode_{} -> r_nil_{};\n",
          node->key, "black", node->key, node->key, node->key);
    }
  };
  if (root_node != nullptr)
    dfs(dfs, root_node);
  dot << "}\n";
}

} // namespace rbt

#undef TEMPLATE_NODE
#undef TEMPLATE_RBT_C
#undef TEMPLATE_RBT_M
