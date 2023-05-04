
```cpp
#include "json.hpp"

// parse
eee::Json json = eee::parse(std::string json_str).value(); 

// Object key : String
json[key] = value
// Array index : size_t
json[index] = value 

json = value;

// generate
std::string str = eee::generate(json);
```

