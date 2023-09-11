---
title: "基于 C++ 的红黑树"
date: 2023-09-13T23:45:14+08:00
categories:
  - Cpp
tags:
  - 红黑树
  - 数据结构
  - Cpp
index_img: /images/Cpp/基于Cpp的红黑树/0.png
---

## 性质

1. 节点是红色或者黑色
2. 所有叶子都是黑色的（叶子节点是 Nil）
3. 不存在两个相邻的节点都为红色 （红色节点的子节点和父节点都是黑色的）
4. 从根节点到任何叶子节点的路径上的黑色节点数量都是相等的 （每条路径上的红色节点是少于黑色节点的，保证了最长路径不会超过最短路径的两倍）
5. 根是黑色

## 插入

插入的节点最初是红色, 插入的步骤：

- 根据二叉搜索树的规则，把节点插入到对应的位置
- 插入节点后，开始调整树

### Case 1

插入节点为第一个节点，这个节点在插入后为跟节点，将这个节点染为黑色。

### Case 2

插入节点后的父节点为黑色, 此时不做修改仍然满足红黑树的性质。（新节点为红色，新插入的节点的孩子是叶子节点，它的相邻的节点都不是红色，红色节点不会影响性质 4）

（注：二叉搜索树新插入的节点是叶子节点）


### Case 3

插入节点后的父节点为红色, 叔父节点也为红色, 把它们变为黑色，同时将祖父节点变为红色，如果祖父节点是根节点，不满足性质 5，如果
祖父节点不是根节点，可能不满足性质 3, 这时将祖父节点作为先插入的一个节点重新去考虑。

![case3](./基于Cpp的红黑树/insert_case3.png)

```cpp
    father->color = details::Color::BLACK;
    uncle->color = details::Color::BLACK;
    node = get_grandfather(node);
    node->color = details::Color::RED;
    switch_insert(grandfather);

```

### Case 4

插入节点 N 后，节点 N 与 父节点 P 的方向相同 （例如：N 节点是 左孩子，P 节点是左孩子），而且节点 N 的叔父节点为黑色或者是一个 Nil 节点。在没有插入节点 N 时，树一定是满足所有性质的，我们可以旋转在染色的方法来使树合法。

- 如果 N 为左子节点则右旋转祖父节点 G，否则左旋转祖父节点 G，P - N 这条路径会少一个黑色节点。
- 将节点 N 染为黑色，节点 G 染为红色，此时每条路径的黑色节点与插入节点 N 前一样。

![case4](./基于Cpp的红黑树/insert_case4.png)

```cpp
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
```

### Case 5

插入节点 N 后，节点 N 与 父节点 P 的方向不同 （例如：N 节点是 左孩子，P 节点是右孩子），而且节点 N 的叔父节点为黑色或者是一个 Nil 节点，这种情况可以通过对节点 P 左旋转来将其转化为 Case 4 的情况。

![case5](./基于Cpp的红黑树/insert_case5.png)

```cpp
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
```

## 删除

## Debug

使用 dot 绘图，可以更清晰的观察小样例

```cpp
rbt.draw("test.dot");

```

```bash
dot -Tpng -o test.png test.dot
```

查看 test.png 即可

## 完整实现

- ![github/mingzi47/CppDemo/RedBlackTree](https://github.com/mingzi47/CppDemo/tree/main/RedBlackTree)

## 其他

**插入与查找的性能与 `std::map` 还有很大 2 ~ 3 倍的差距，查找的性能会更差些。**

- ![红黑树 - 维基百科](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)

- ![红黑树 - OI Wiki](https://oi-wiki.org/ds/rbtree/)
