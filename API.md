```cpp
struct Apple {};
struct Orange {};
struct Banana {};

//  ... 10-20 more structs like that

//////// You are not allowed to change anything above this line

class FruitPlate
{
  // ... implement that class using code from the `main` function as a set of requirements ...
  //
};

//////// You are not allowed to change anything below this line
#include <iostream>

int main() {

  FruitPlate plate;

  plate.Add(Apple{});
  plate.Add(Banana{});
  plate.Add(Orange{});
  plate.Add(Apple{});

  // I expecrt this code to print "apple banana orange apple" here
  std::cout << plate << std::endl;

  return 0;
}
```
