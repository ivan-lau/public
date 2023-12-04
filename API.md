### Design a Hash Map


You are required to implement a clsa::hash_map class, which represents a basic hash map with the following functionality:

- Create a function `insert`: Inserts a key-value pairs into the hash map.
- Create a function `at`: Retrieves the value linked to a specified key, and raise an exception if no value is associated with the key.
- Create a function `erase`: Removes the element (if one exists) with the key equivalent to key.

Please disregard the implementation of iterators, and feel free to utilize any STL sequence containers except for associative containers such as std::map, std::set, std::unordered_map, and std::unordered_set. It is important to note that the use of any pre-existing hash table libraries or data structures is not permitted.

```cpp
#include <bits/stdc++.h>

namespace clsa {
    template<typename K, typename V>
    using hash_map = std::unordered_map<K, V>;
}

enum class Number {
    ONE = 1,
    TWO,
    THREE,
    MILLION = 1000000
};

int main() {
    std::unordered_map<Number, std::string> uo_map;

    uo_map.insert({Number::ONE, "One"});
    uo_map.insert({Number::MILLION, "Million"});
    std::cout << uo_map.at(Number::ONE) << '\n';
    std::cout << uo_map.at(Number::MILLION) << '\n';
    
    try {
        std::cout << uo_map.at(Number::TWO) << std::endl;
    } catch (...) {
        std::puts("key [2] not found");
    }

    uo_map.erase(Number::MILLION);
    try {
        std::cout << uo_map.at(Number::MILLION) << std::endl;
    } catch (...) {
        std::puts("key [1000000] not found");
    }

    return 0;
}
```