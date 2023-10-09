General
-------

**Q: Are you currently employed by any company at the moment?**<br>

**Q: Were you hired by Morgan Stanley as a contractor or as a permanent employee?**<br>

**Q: Which project did you work on in C++ at Morgan Stanley, as I haven't come across any projects you mentioned that involve the use of C++?**<br>

**Q: What high-touch applications you were working on when you were in MS?**<br>

**Q: Have you performed any certification between client and the Flextrade? Do you know what do you need to do?**<br>
A:

C++
---

### Basic

**Q: Do you know if we can throw exception in constructor? or in destructor?**<br>
A: 
- Constructors: Yes. You should throw an exception from a constructor whenever you cannot properly initialize (construct) an object. There is no really satisfactory alternative to exiting a constructor by a throw. For more details, see here.

- Destructors: Not really. You can throw an exception in a destructor, but that exception must not leave the destructor; if a destructor exits by emitting an exception, all kinds of bad things are likely to happen (stack unwinding) because the basic rules of the standard library and the language itself will be violated. Don’t do it.

**Q: If you throw exceptions from constructors, how would you handle the resources?**<br>
A: If a constructor throws an exception, the object’s destructor is not run. If your object has already done something that needs to be undone (such as allocating some memory, opening a file, or locking a semaphore), this “stuff that needs to be undone” must be remembered by a data member inside the object. There are two ways to fix this:
- Have exceptions fully managed in the constructor (try catch within the constructor).
- Use a smart pointer. When a constructor is entered, all of its members have been constructed. And because when a constructor throws, and objects members have been constructed, they must be destroyed.<br>For example `std::unique_ptr`, it provides exception safety to classes and functions that handle objects with dynamic lifetime, by guaranteeing deletion on both normal exit and exit through exception

**Q: What is virtual inheritance? Explain how does it resolve diamond problem in multiple inheritance.**<br>
A: It is used to solve diamond problem in multiple inheritance. This prevents multiple inheritance with the same base class more than once by going thru DFS-like inheritance hierarchy.

```
class A { 
  public: 
  virtual ~A() { std::cout << "A"; } 
};
class B : virtual public A { 
  public: 
  ~B() { std::cout << "B"; } 
};
class C : virtual public A { 
  public: 
  ~C() { std::cout << "C"; } 
};
class D : public B, public C { 
 public: 
 ~D() { std::cout << "D"; } 
};

int main() {
  A* aPtr = new D();
  delete aPtr;
  return 0;
}
```
<ins>Output:</ins> DCBA

C++11
---

### Move semantics


**Q: What does `std::move` function do?**<br>
A:
`std::move` does not move anything but it unconditionally casts its argument to an rvalue. `std::move` is only a remove-reference-cast-to-rvalue operation, so basically all it does is a `static_cast` and it's intended only to ensure that the move constructor - if such exists on the left side - should be called with the argument from the right side.

Implementation of `std::move`:

```
template<typename _Tp>
constexpr typename std::remove_reference<_Tp>::type&&
move(_Tp&& __t) noexcept
{ return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }
```

**Q: What does `std::forward` function do? or what's the difference comparing `std::move`?**<br>
A: `std::forward` does not forward anything but it performs conditionally cast which is only if a particular condition is fulfilled. The condition when `T&&` with type deduction (template or auto).

This sounds a lot like `std::move`, which also casts to an rvalue. But `std::forward` will avoid casting to an rvalue if the argument is an lvalue reference. It’s implementation is conceptually something like:
```
if (is_lvalue_reference<T>::value)
  return t;
return std::move(t);
```


**Q: Do you know the differences between `push_back()` and `emplace_back()` in `std::vector`?**<br>
A: 

Interface:
```
template<class T, class Allocator<T>>
class vector {
public:
    …                                  // push_back is overloaded
    void push_back(const T& v);        // lref, copy lvalues
    void push_back(T&& v);             // rref, move rvalues

    template<class... Args>
    void emplace_back(Args&&... args); // uref, forwarding everything
```

**Q: Suppose there is a `std::string` object and a `std::vector` of string as local variables, Explain the difference between using `push_back()` and `emplace_back()` to add that object to the vector?**<br>
A: 

Codes:
```
struct MyClass {
    MyClass() {
        std::cout << "Create class" << std::endl;
    }
    ~MyClass() {
        std::cout << "Destroy class" << std::endl;
    }
    MyClass(const MyClass& my_class) {
        std::cout << "Copy Constructor Called" << std::endl;
    }
    MyClass (MyClass&& my_class) noexcept {
        std::cout << "Move Constructor Called" << std::endl;
    }
};

int main() {
    std::vector<MyClass> vector;
    vector.reserve(6);

    std::cout << "\n--- push_back lref ---" << std::endl;
    MyClass mc1;
    vector.push_back(mc1);

    std::cout << "\n--- push_back rref ---" << std::endl;
    MyClass mc2;
    vector.push_back(std::move(mc2));

    std::cout << "\n--- in-place push_back ---" << std::endl;
    vector.push_back(MyClass());

    std::cout << "\n--- emplace_back lref ---" << std::endl;
    MyClass mc3;
    vector.emplace_back(mc3);
    
    std::cout << "\n--- emplace_back rref ---" << std::endl;
    MyClass mc4;
    vector.emplace_back(std::move(mc4));

    std::cout << "\n--- in-place emplace_back ---" << std::endl;
    vector.emplace_back();
  
    std::cout << "\n--- Finish ---" << std::endl;
    return 0;
}
```

- push_back:

```
--- push_back lref ---
Create class
Copy Constructor Called

--- push_back rref ---
Create class
Move Constructor Called

--- in-place push_back ---
Create class
Move Constructor Called
Destroy class

```

- emplace_back

```
--- emplace_back lref ---
Create class
Copy Constructor Called

--- emplace_back rref ---
Create class
Move Constructor Called

--- in-place emplace_back ---
Create class
```

### Smart pointer

**Q: If you need to manage resources that are not allocated with new, such as file handles or sockets, is it still possible to use std::unique_ptr? If so, how would you do it, and what additional consideration(s) would you need to take into account?**<br>
A: Yes, it is still possible to use std::unique_ptr to manage resources that are not allocated with new, such as file handles or sockets. To do this, you would need to use a custom deleter that knows how to release the resource correctly.

```
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>

void close_socket(int socket_fd) {
    close(socket_fd);
}

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Create a unique_ptr that manages the socket
    std::unique_ptr<int, decltype(&close_socket)> socket_ptr(&socket_fd, &close_socket);

    // Use the socket_ptr to send and receive data over the socket

    return 0;
}
```

FIX Protocol
------------

**Q: What is the difference between ClOrdID and OrderID?**<br>
A: ClOrdId is a unique id assigned by buy-side while the later is assigned by sell-side. OrderID normally remains same for a message chain (e.g. on Cancel and mod orders) while ClOrdID changes with Cancel and Modification.

ClOrdID (tag 11) 
– is a unique id assigned by buy-side while the later is assigned by sell-side
– is meant to be used by the buy side to identify the order request
– The sell side returns ClOrdID in its response and this enables the response to be be linked to the originating request
– For a modification, the buy side sends a request containing the previous ClOrdID together with a new ClOrdID to identify this new request

OrderID (tag 37) 
– normally remains same for a message chain (e.g. on Cancel and mod orders) while ClOrdID changes with Cancel and Modification. 

**Q: Which tag and what value to set in FIX for high touch order?**<br>
A: Tag 21 (HandInst) and value = 3 for CARE order.

  ```
  1 = Automated execution order, private, no Broker
  2 = Automated execution order, public, Broker intervention OK
  3 = Manual order, best execution
  ```

**Q: What is a repeating group? How does it work in FIX protocol?**<br>
A: The "NoRoutingIDs" tag (Tag 215) in the FIX (Financial Information eXchange) protocol is used to indicate the number of repeating groups of routing information within a message. It specifies the count of repeating instances for the "RoutingID" (Tag 217) and "RoutingType" (Tag 216) tags.
