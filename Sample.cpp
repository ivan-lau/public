
#include <iostream>
#include <string>

// This is the subscriber class
class Subscriber {
public:
    virtual void update(const std::string& message) = 0;
};

// This is a concrete implementation of the Subscriber class
class ConcreteSubscriber : public Subscriber {
public:
    void update(const std::string& message) override {
        std::cout << "ConcreteSubscriber received message: " << message << std::endl;
    }
};

// This is the publisher class, which has a reference to a subscriber
class Publisher {
public:
    Publisher(Subscriber& subscriber) : subscriber_(subscriber) {}

    void notify(const std::string& message) {
        subscriber_.update(message);
    }

private:
    Subscriber& subscriber_;
};

int main() {
    ConcreteSubscriber subscriber;
    Publisher publisher(subscriber);

    publisher.notify("Hello, world!");

    return 0;
}

// #include <iostream>

// struct Data {};

// struct ExternalStream {
//     virtual void onCallback() = 0;
// };

// struct MyExternalStream : public ExternalStream {
//     void onCallback() override {
//         // simulate callback
//         std::cout << "Received callback from external stream\n";
//     }
// };

// template <typename Derived>
// struct IHandler {
//     void callback() {
//         static_cast<Derived*>(this)->callback();
//     }
// };

// template <typename Derived>
// struct StreamWrapper : public ExternalStream {
//     template <typename Handler>
//     StreamWrapper(Handler& handler) : m_handler(handler) {}

//     StreamWrapper& operator=(StreamWrapper&& other) {
//         m_handler = other.m_handler;
//         other.m_handler = nullptr;
//         return *this;
//     }

//     void onCallback() override final {
//         m_handler.callback();
//     }

//     IHandler<Derived>& m_handler;
// };

// struct MyClass : public IHandler<MyClass> {
//     void callback() {
//         // handle callback ...
//         std::cout << "Received callback from stream wrapper\n";
//     }

//     MyClass() : m_streamWrapper(*this) {}

//     StreamWrapper<MyClass> m_streamWrapper;
// };

// int main() {
//     // create an instance of MyClass
//     MyClass myClass;

//     // create an instance of MyExternalStream
//     MyExternalStream myExternalStream;

//     // register MyClass with the stream wrapper
//     myClass.m_streamWrapper = StreamWrapper<MyClass>(myClass);

//     // simulate a callback from the external stream
//     myExternalStream.onCallback();

//     return 0;
// }


#include <iostream>

struct Data {};

struct ExternalStream {
    virtual void onCallback() = 0;
};

struct MyExternalStream : public ExternalStream {
    void onCallback() override {
        // simulate callback
        std::cout << "Received callback from external stream\n";
    }
};

template <typename Derived>
struct StreamWrapper {
    struct IHandler {
        void callback() {
            static_cast<Derived*>(this)->callback();
        }
    };

    StreamWrapper(IHandler& handler) : m_handler(handler) {}

    void onCallback() {
        m_handler.callback();
    }

    IHandler& m_handler;
};

struct MyClass : public StreamWrapper<MyClass>::IHandler {
    void callback() {
        // handle callback ...
        std::cout << "Received callback from stream wrapper\n";
    }

    MyClass() : m_streamWrapper(*this) {}

    StreamWrapper<MyClass> m_streamWrapper;
};

int main() {
    MyClass myObj;
    myObj.m_streamWrapper.onCallback(); // simulate a callback

    return 0;
}

//If you want to avoid the virtual function call and resolve the function call at compile-time, you can use a template argument instead of a virtual function. Here's an example implementation:


template <typename T, void (T::*callback)()>
struct StreamWrapper {
    StreamWrapper(T& obj) : m_obj(obj) {}

    void onCallback() {
        (m_obj.*callback)();
    }

    T& m_obj;
};

struct MyClass {
    void callback() {
        // handle callback ...
        std::cout << "Received callback from stream wrapper\n";
    }

    MyClass() : m_streamWrapper(*this) {}

    StreamWrapper<MyClass, &MyClass::callback> m_streamWrapper;
};
/*
In this implementation, the StreamWrapper class takes two template parameters: a class type T and a pointer to a member function of T that takes no arguments and returns void. The StreamWrapper constructor takes a reference to a T object, and the onCallback function calls the member function pointed to by the callback template parameter on the T object.

The MyClass class now defines a callback member function that matches the signature of the callback template parameter in StreamWrapper. Its constructor initializes a StreamWrapper object that subscribes to the MyClass object by passing a pointer to the callback member function as the callback template parameter.
*/
With this implementation, the function call in the StreamWrapper class is resolved at compile-time using the template parameter, avoiding the virtual function call and the overhead associated with it.
