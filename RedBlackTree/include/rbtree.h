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

template <typename K, typename V>
using RBPtr = std::shared_ptr<details::Node<K, V>>;

TEMPLATE_NODE
auto get_father(const RBPtr<K, V> &node) -> RBPtr<K, V> {
  return node->f_node.lock();
}

TEMPLATE_NODE
auto get_uncle(const RBPtr<K, V> &node) -> RBPtr<K, V> {
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
auto get_grandfather(const RBPtr<K, V> &node) -> RBPtr<K, V> {
  auto father = get_father(node);
  if (father != nullptr)
    return get_father(father);
  return nullptr;
}

TEMPLATE_NODE
auto get_sibling(const RBPtr<K, V> &node) -> RBPtr<K, V> {
  auto father = get_father(node);
  if (father == nullptr)
    return nullptr;
  switch (node->attribute) {
  case Attribute::LEFT_CHILD:
    return father->r_node;
  case Attribute::RIGHT_CHILD:
    return father->l_node;
  }
  return nullptr;
}

TEMPLATE_NODE
auto get_close_nephew(const RBPtr<K, V> &node) -> RBPtr<K, V> {
  auto sibling = get_sibling(node);
  if (sibling == nullptr)
    return nullptr;
  switch (node->attribute) {
  case Attribute::LEFT_CHILD:
    return sibling->l_node;
  case Attribute::RIGHT_CHILD:
    return sibling->r_node;
  }
  return nullptr;
};

TEMPLATE_NODE
auto get_distant_nephew(const RBPtr<K, V> &node) -> RBPtr<K, V> {
  auto sibling = get_sibling(node);
  if (sibling == nullptr)
    return nullptr;
  switch (node->attribute) {
  case Attribute::LEFT_CHILD:
    return sibling->r_node;
  case Attribute::RIGHT_CHILD:
    return sibling->l_node;
  }
  return nullptr;
};
template <typename... Args>
auto equal_color(Color color, Args &&...args) -> bool {
  bool res = true;
  ((res &= (args->color == color)), ...);
  return res;
}

TEMPLATE_NODE
auto swap_color(RBPtr<K, V> &a, RBPtr<K, V> &b) {
  auto tmp = a->color;
  a->color = b->color;
  b->color = tmp;
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
  auto details_find(K key) -> details::RBPtr<K, V>;
  auto details_insert(K key, V value) -> details::RBPtr<K, V>;
  auto switch_insert(details::RBPtr<K, V> &node) -> void;
  auto switch_remove(details::RBPtr<K, V> &node, bool op = true) -> void;
  auto rotate_left(details::RBPtr<K, V> &node) -> void;
  auto rotate_right(details::RBPtr<K, V> &node) -> void;

private:
  details::RBPtr<K, V> root_node;
};

TEMPLATE_RBT_M rotate_left(details::RBPtr<K, V> &X)->void {
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
    Y->color = details::Color::BLACK;
    root_node = Y;
    {
      details::RBPtr<K, V> clear;
      root_node->f_node = clear;
    }
  }

  X->r_node = Y->l_node;
  if (X->r_node != nullptr) {
    X->r_node->attribute = details::Attribute::RIGHT_CHILD;
    X->r_node->f_node = X;
  }
  X->f_node = Y;
  Y->l_node = X;
  X->attribute = details::Attribute::LEFT_CHILD;
}

TEMPLATE_RBT_M rotate_right(details::RBPtr<K, V> &X)->void {
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
    Y->color = details::Color::BLACK;
    root_node = Y;
    {
      details::RBPtr<K, V> clear;
      root_node->f_node = clear;
    }
  }
  X->l_node = Y->r_node;
  if (X->l_node != nullptr) {
    X->l_node->attribute = details::Attribute::LEFT_CHILD;
    X->l_node->f_node = X;
  }
  X->f_node = Y;
  Y->r_node = X;
  X->attribute = details::Attribute::RIGHT_CHILD;
}

TEMPLATE_RBT_M
details_find(K key)->details::RBPtr<K, V> {
  if (root_node == nullptr)
    return nullptr;
  details::RBPtr<K, V> res = nullptr;
  for (res = root_node; res != nullptr;) {
    if (res->key > key) {
      if (res->l_node == nullptr)
        return res;
      res = res->l_node;
    } else if (res->key < key) {
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
details_insert(K key, V value)->details::RBPtr<K, V> {
  if (root_node == nullptr) {
    root_node = std::make_shared<details::Node<K, V>>(
        key, value, details::Attribute::ROOT, details::Color::BLACK, nullptr,
        nullptr, nullptr);
    return root_node;
  }

  details::RBPtr<K, V> res = details_find(key);
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

TEMPLATE_RBT_M switch_insert(details::RBPtr<K, V> &node)->void {
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

  if (case1) {
    node->color = details::Color::BLACK;
    return;
  } else if (case2) {
    return;
  } else if (case3) {
    father->color = details::Color::BLACK;
    uncle->color = details::Color::BLACK;
    grandfather->color = details::Color::RED;
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

TEMPLATE_RBT_M switch_remove(details::RBPtr<K, V> &node, bool op)->void {
  // Case 3 中，我们会递归向上来调整，但是这时我们以经不在需要删除点了，但是还要递归 switch_remove 这个函数，使用一个 op 来记录调用这次函数时，是否要执行 remove_node() 函数
  auto father = get_father(node);
  if (node->attribute == details::Attribute::ROOT and
      node->r_node == nullptr and node->r_node) {
    root_node = nullptr;
    return;
  }

  // 
  auto change_node = [&](details::RBPtr<K, V> &f, details::RBPtr<K, V> &c,
                         details::Attribute attr) -> details::RBPtr<K, V> {
    if (c == nullptr)
      return nullptr;
    c->f_node = f;
    c->attribute = attr;
    return c;
  };

  // 红色的叶子节点与只有一个节点的红色节点
  if (node->color == details::Color::RED and
      (node->r_node == nullptr or node->l_node == nullptr)) {
    switch (node->attribute) {
    case details::Attribute::LEFT_CHILD: {
      father->l_node = change_node(
          father, (node->r_node != nullptr ? node->r_node : node->l_node),
          details::Attribute::LEFT_CHILD);
      return;
    }
    case details::Attribute::RIGHT_CHILD: {
      father->r_node = change_node(
          father, (node->r_node != nullptr ? node->r_node : node->l_node),
          details::Attribute::RIGHT_CHILD);
      return;
    }
    }
  }

  // 有两个子节点, 会先尝试替换前驱和后继，在判断是否需要调整
  if (node->r_node != nullptr and node->l_node != nullptr) {
    auto prev_node = [&]() -> details::RBPtr<K, V> {
      for (auto i = node->l_node; i != nullptr; i = i->r_node) {
        if (i->r_node == nullptr)
          return i;
      }
      return nullptr;
    }();
    auto suf_node = [&]() -> details::RBPtr<K, V> {
      for (auto i = node->r_node; i != nullptr; i = i->l_node) {
        if (i->l_node == nullptr)
          return i;
      }
      return nullptr;
    }();

    auto swap_node = [&](details::RBPtr<K, V> &left,
                         details::RBPtr<K, V> &right) {
      left.swap(right);
      swap_color(left, right);
      if (left->r_node != nullptr)
        left->r_node->f_node = left;
      if (left->l_node != nullptr)
        left->l_node->f_node = left;
      if (right->r_node != nullptr)
        right->r_node->f_node = right;
      if (right->l_node != nullptr)
        right->l_node->f_node = right;
    };
    // 被删除的节点是红色
    if ((prev_node->color == details::Color::RED or
         suf_node->color == details::Color::RED) and
        node->color == details::Color::RED) {
      swap_node(node, (prev_node->color == details::Color::RED ? prev_node
                                                               : suf_node));
      switch_remove(node);
    }

    // 需要调整的情况
    swap_node(node, prev_node);
  }

  // 下面是调整的步骤
  auto sibling = get_sibling(node);
  auto close_nephew = get_close_nephew(node);
  auto distant_nephw = get_distant_nephew(node);
  father = get_father(node);

  bool all_exist = (father != nullptr and sibling != nullptr and
                    close_nephew != nullptr and distant_nephw != nullptr);

  auto remove_node = [&](details::RBPtr<K, V> &d_node) {
    assert(d_node->l_node == nullptr or d_node->r_node == nullptr);
    assert(d_node->f_node.lock() != nullptr);
    auto father = d_node->f_node.lock();
    switch (d_node->attribute) {
    case details::Attribute::LEFT_CHILD: {
      father->l_node = change_node(
          father, (d_node->r_node == nullptr ? d_node->l_node : d_node->r_node),
          details::Attribute::LEFT_CHILD);
      return;
    }
    case details::Attribute::RIGHT_CHILD: {
      father->r_node = change_node(
          father, (d_node->r_node == nullptr ? d_node->l_node : d_node->r_node),
          details::Attribute::RIGHT_CHILD);
      return;
    }
    }
  };
  // Case 1
  if (all_exist and
      equal_color(details::Color::BLACK, father, close_nephew, distant_nephw) and
      equal_color(details::Color::RED, sibling)) {
    swap_color(sibling, father);
    switch (node->attribute) {
    case details::Attribute::LEFT_CHILD: {
      rotate_left(father);
      break;
    }
    case details::Attribute::RIGHT_CHILD: {
      rotate_right(father);
      break;
    }
    }
    if (op)
      remove_node(node);
    return;
  }

  // Case 2
  if (all_exist and
      equal_color(details::Color::BLACK, sibling, close_nephew,
                  distant_nephw) and
      equal_color(details::Color::RED, father)) {
    swap_color(sibling, father);
    if (op)
      remove_node(node);
    return;
  }

  // Case 3 并不需要所有节点都存在，侄子节点如果是黑色的，可以是空节点。
  const bool case3_node_exist = (father != nullptr and sibling != nullptr);
  const bool case3_nephew =
      ((close_nephew == nullptr and
        distant_nephw == nullptr) or // 两个侄子节点都不存在, 也是为黑色
       (close_nephew != nullptr and distant_nephw == nullptr and
        close_nephew->color == details::Color::BLACK) or
       (close_nephew == nullptr and distant_nephw != nullptr and
        distant_nephw->color == details::Color::BLACK) or
       (all_exist and
        equal_color(details::Color::BLACK, close_nephew, distant_nephw)));
  ;
  if (case3_node_exist and
      equal_color(details::Color::BLACK, father, sibling) and case3_nephew) {
    sibling->color = details::Color::RED;
    if (op)
      remove_node(node);
    switch_remove(father, false);
    return;
  }

  // Case 4
  if (all_exist and
      equal_color(details::Color::BLACK, sibling, distant_nephw) and
      equal_color(details::Color::RED, close_nephew)) {
    switch (node->attribute) {
    case details::Attribute::LEFT_CHILD:
      rotate_right(sibling);
    case details::Attribute::RIGHT_CHILD:
      rotate_left(sibling);
    }
    swap_color(sibling, close_nephew);
  }

  // Case 5
  sibling = get_sibling(node);
  distant_nephw = get_distant_nephew(node);
  father = get_father(node);
  const bool case5_node_exist =
      (father != nullptr and sibling != nullptr and distant_nephw != nullptr);
  if (case5_node_exist and equal_color(details::Color::BLACK, sibling) and
      equal_color(details::Color::RED, distant_nephw)) {
    switch (node->attribute) {
    case details::Attribute::LEFT_CHILD: {
      rotate_left(father);
      break;
    }
    case details::Attribute::RIGHT_CHILD: {

      rotate_right(father);
      break;
    }
    }
    swap_color(sibling, father);
    distant_nephw->color = details::Color::BLACK;
  }
}

TEMPLATE_RBT_M remove(K key)->void {
  auto node = details_find(key);
  if (node == nullptr or node->key != key)
    return;
  switch_remove(node);
}

TEMPLATE_RBT_M modify(K key, V value)->void {
  auto res = details_find(key);
  if (res->key != key)
    throw;
  res->value = value;
}
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
  std::ofstream dot(pathname);
  dot << "graph rbtree {\n";
  auto dfs = [&dot](auto dfs, details::RBPtr<K, V> &node) -> void {
    auto l_node = node->l_node;
    auto r_node = node->r_node;
    dot << std::format("node_{} [color={}, "
                       "label=\"K={}, V={}\"];\n",
                       node->key,
                       (node->color == details::Color::RED ? "red" : "black"),
                       node->key, node->value);
    if (l_node != nullptr) {
      auto f_node = l_node->f_node.lock();
      dot << std::format("node_{}--node_{};\n", node->key, l_node->key);
      dfs(dfs, l_node);
    } else {
      dot << std::format(
          "l_nil_{} [color={}, label=\"Nil\"];\nnode_{}--l_nil_{};\n",
          node->key, "black", node->key, node->key, node->key);
    }
    if (r_node != nullptr) {
      auto f_node = r_node->f_node.lock();
      dot << std::format("node_{}--node_{};\n", node->key, r_node->key);
      dfs(dfs, r_node);
    } else {
      dot << std::format(
          "r_nil_{} [color={}, label=\"Nil\"];\nnode_{}--r_nil_{};\n",
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
