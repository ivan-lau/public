
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
