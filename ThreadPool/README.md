
```cpp
#include "ThreadPool.hpp"

ctp::ThreadPool pool(3);

auto ret = pool.submit([]{return 100;});

std::cout << ret.get() << "\n";
```
