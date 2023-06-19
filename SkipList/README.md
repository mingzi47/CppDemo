# SkipList

```cpp
#include "skiplist.hpp"
mzi::SkipList<int, int> L;
L.insert(100, 10);
L[200] = 10;
std::cout << L[200]; // 200;
L[200] = 100;
std::cout << L[200]; // 100;
L.erase(200);
L.contains(200);    // false
```
