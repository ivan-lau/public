```cpp

#include <iostream>

class Integer {
private:
    int* value; // Pointer to dynamically allocated integer

public:
    // Constructor: Allocates memory for the integer
    Integer(int val) {
        value = new int(val); // Dynamically allocate memory
        std::cout << "Constructor: Allocated memory for " << *value << "\n";
    }

    // Destructor: Deallocates memory
    ~Integer() {
        if (value) {
            std::cout << "Destructor: Deallocated memory for " << *value << "\n";
            delete value; // Free the allocated memory
        }
    }

    // Copy Constructor: Deep copy
    Integer(const Integer& other) {
        value = new int(*other.value); // Allocate new memory and copy the value
        std::cout << "Copy Constructor: Copied value " << *value << "\n";
    }

    // Copy Assignment Operator: Deep copy
    Integer& operator=(const Integer& other) {
        if (this != &other) { // Self-assignment check
            delete value; // Free existing memory
            value = new int(*other.value); // Allocate new memory and copy the value
            std::cout << "Copy Assignment Operator: Copied value " << *value << "\n";
        }
        return *this;
    }

    // Move Constructor: Transfer ownership
    Integer(Integer&& other) noexcept {
        value = other.value; // Transfer ownership
        other.value = nullptr; // Ensure the moved-from object is in a valid state
        std::cout << "Move Constructor: Moved value " << *value << "\n";
    }

    // Move Assignment Operator: Transfer ownership
    Integer& operator=(Integer&& other) noexcept {
        if (this != &other) { // Self-assignment check
            delete value; // Free existing memory
            value = other.value; // Transfer ownership
            other.value = nullptr; // Ensure the moved-from object is in a valid state
            std::cout << "Move Assignment Operator: Moved value " << *value << "\n";
        }
        return *this;
    }

    // Overload << operator for printing
    friend std::ostream& operator<<(std::ostream& os, const Integer& obj) {
        if (obj.value) {
            os << *obj.value;
        } else {
            os << "nullptr";
        }
        return os;
    }
};

int main() {
    Integer a(10); // Constructor
    std::cout << "a: " << a << "\n";

    Integer b = a; // Copy Constructor
    std::cout << "b: " << b << "\n";

    Integer c(20); // Constructor
    c = a; // Copy Assignment Operator
    std::cout << "c: " << c << "\n";

    Integer d(std::move(c)); // Move Constructor
    std::cout << "d: " << d << "\n";
    std::cout << "c (after move): " << c << "\n"; // c is now in a valid but unspecified state

    Integer e(30); // Constructor
    e = std::move(d); // Move Assignment Operator
    std::cout << "e: " << e << "\n";
    std::cout << "d (after move): " << d << "\n"; // d is now in a valid but unspecified state

    return 0;
}

```
