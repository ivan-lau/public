### Design a Shared Pointer

```cpp
#include <bits/stdc++.h>

template<typename T> class MySharedPtr;

namespace clsa {
    template<typename T>
    // using shared_ptr = MySharedPtr<T>;
    using shared_ptr = std::shared_ptr<T>;
}

namespace clsa {
    template <class T, class... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

class Person {
    std::string name;
    int age;

public:

    Person(const std::string& name, int age) : name(name), age(age) {
        std::cout << "Person created: " << name << std::endl;
    }

    ~Person() {
        std::cout << "Person destroyed: " << name << std::endl;
    }

    void display() {
        std::cout << "Name: " << name << ", Age: " << age << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << "Name: " << p.name << ", Age: " << p.age << std::endl;
        return os;
    }
};

template <typename T>
class MySharedPtr final {
    T* ptr;
    size_t* refCount;

public:
    MySharedPtr() : ptr(nullptr), refCount(new size_t(0)) {}

    MySharedPtr(T* ptr) : ptr(ptr), refCount(new size_t(1)) {}

    ~MySharedPtr() {
        if (refCount && --(*refCount) == 0) {
            delete ptr;
            delete refCount;
        }
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        if (refCount)
            ++(*refCount);
    }

    MySharedPtr& operator=(const MySharedPtr& other) {
        if (this != &other) {
            if (refCount && --(*refCount) == 0) {
                delete ptr;
                delete refCount;
            }

            ptr = other.ptr;
            refCount = other.refCount;

            if (refCount)
                ++(*refCount);
        }
        return *this;
    }

    void reset() {
        if (refCount && --(*refCount) == 0) {
            delete ptr;
            delete refCount;
        }
        ptr = nullptr;
        refCount = nullptr;
    }

    size_t use_count() const {
        return refCount == nullptr ? 0 : *refCount;
    }
};

int main() {
    clsa::shared_ptr<Person> person1(new Person("Keung To", 24));
    auto person3 = clsa::make_shared<Person>("Anson Lo", 28);
    person1->display(); 

    auto person2 = person1;

    std::cout << *person2;
    std::cout << *person3;

    std::cout << "Reference count for person1: " << person1.use_count() << std::endl;

    person2.reset();
    std::cout << "Reference count for person1 after reset: " << person1.use_count() << std::endl;

    return 0;
}
```