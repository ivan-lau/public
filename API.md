```cpp
double round_to(double value, double precision = 1.0)
{
    double i = 1.0 / precision;
    return std::round(value * i) * precision;
}
```

To avoid the sharing of static variables across multiple instances of a shared library, you can employ techniques such as:

1. **Avoid or minimize the use of static variables**: If possible, consider using alternative approaches that do not rely heavily on static variables. Instead, design your library to accept inputs and return outputs explicitly through function parameters or data structures.

2. **Use thread-local storage**: If you want each instance of the shared library to have its own separate copy of a variable, you can use thread-local storage (TLS). Thread-local storage allows each thread or instance to have its own unique instance of a variable. C++11 introduced the `thread_local` keyword for defining thread-local variables. However, note that this approach is specific to individual threads and may not work as expected if multiple threads within the same instance of the library need access to independent copies of the variable.

3. **Dynamic allocation**: Instead of using static variables, you can dynamically allocate memory for the data that needs to be separate for each instance of the library. This way, each instance can have its own memory region to store and manipulate the data. However, keep in mind that dynamic allocation introduces additional responsibilities for memory management, such as proper deallocation when no longer needed.

4. **Isolate or encapsulate state**: If you need to have shared state across instances but also want to avoid interference between instances, you can encapsulate the shared data within a separate object or data structure. Each instance of the library can then work with its own instance of that object, ensuring isolation and preventing interference.

5. **Use separate processes**: If the requirement for separate static variable instances is critical and cannot be achieved within a shared library, you may consider using separate processes instead. Each process would load its own instance of the shared library, providing complete isolation between them.

It's important to carefully consider the design and requirements of your specific application to determine the most suitable approach for avoiding the sharing of static variables across instances of a shared library.

---

```cpp
#include <iostream>
#include <map>
#include <list>
#include <unordered_map>
#include <memory>

enum class Side {
    Buy,
    Sell
};

struct Order {
    int id;
    Side side;
    double price;
    int quantity;

    Order(int id, Side side, double price, int quantity)
        : id(id), side(side), price(price), quantity(quantity) {}
};

class MatchingEngine {
public:
    int insertOrder(const Order& order) {
        int orderId = generateOrderId();
        auto orderPtr = std::make_shared<Order>(order);
        orderIds[orderId] = orderPtr;
        
        if (order.side == Side::Buy) {
            buyOrders[order.price].push_back(orderId);
        } else {
            sellOrders[order.price].push_back(orderId);
        }
        
        matchOrders();
        
        return orderId;
    }
    
    void cancelOrder(int orderId) {
        auto it = orderIds.find(orderId);
        if (it != orderIds.end()) {
            double price = it->second->price;
            if (buyOrders.count(price) > 0) {
                removeOrderFromList(orderId, buyOrders[price]);
            } else if (sellOrders.count(price) > 0) {
                removeOrderFromList(orderId, sellOrders[price]);
            }
            orderIds.erase(it);
        }
    }

private:
    std::map<double, std::list<int>> buyOrders;
    std::map<double, std::list<int>> sellOrders;
    std::unordered_map<int, std::shared_ptr<Order>> orderIds;
    int lastOrderId = 0;

    int generateOrderId() {
        return ++lastOrderId;
    }

    template<typename T>
    void removeOrderFromList(int orderId, T& orderList) {
        orderList.remove(orderId);
    }

    void matchOrders() {
        for (auto buyIt = buyOrders.rbegin(); buyIt != buyOrders.rend(); ++buyIt) {
            auto buyPrice = buyIt->first;
            auto& buyList = buyIt->second;

            auto sellIt = sellOrders.lower_bound(buyPrice);
            while (sellIt != sellOrders.end() && sellIt->first <= buyPrice) {
                auto& sellList = sellIt->second;

                for (auto buyOrderIt = buyList.begin(); buyOrderIt != buyList.end(); ) {
                    for (auto sellOrderIt = sellList.begin(); sellOrderIt != sellList.end(); ) {
                        if (buyOrderIt->quantity <= sellOrderIt->quantity) {
                            matchOrder(*buyOrderIt, *sellOrderIt);
                            sellOrderIt = sellList.erase(sellOrderIt);

                            if (buyOrderIt->quantity == sellOrderIt->quantity) {
                                buyOrderIt = buyList.erase(buyOrderIt);
                                break;
                            }
                        } else {
                            matchOrder(*buyOrderIt, *sellOrderIt);
                            buyOrderIt->quantity -= sellOrderIt->quantity;
                            sellOrderIt = sellList.erase(sellOrderIt);
                        }
                    }

                    if (sellList.empty() || buyOrderIt->quantity == 0) {
                        break;
                    } else {
                        ++buyOrderIt;
                    }
                }

                if (sellList.empty()) {
                    sellIt = sellOrders.erase(sellIt);
                } else {
                    ++sellIt;
                }
            }

            if (buyList.empty()) {
                buyIt = decltype(buyIt){};
            }
        }
    }

    void matchOrder(const Order& buyOrder, const Order& sellOrder) {
        std::cout << "Matched: "
                  << "Buy Order ID=" << buyOrder.id << ", "
                  << "Sell Order ID=" << sellOrder.id << ", "
                  << "Price=" << buyOrder.price << ", "
                  << "Quantity=" << std::min(buyOrder.quantity, sellOrder.quantity) << std::endl;
    }
};
```

---

To allow the client to receive two kinds of data concurrently from the server, you can use asynchronous read operations in Boost.Asio. Here's an example that demonstrates how to receive two kinds of FlatBuffers data concurrently:

```cpp
#include <boost/asio.hpp>
#include "your_generated_schema1.h" // Include the generated header files for each schema
#include "your_generated_schema2.h"

class Client {
public:
    Client(boost::asio::io_context& ioContext, const std::string& serverIP, unsigned short serverPort)
        : socket_(ioContext)
    {
        boost::asio::ip::tcp::resolver resolver(ioContext);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(serverIP, std::to_string(serverPort));
        boost::asio::connect(socket_, endpoints);

        startReceiving();
    }

private:
    void startReceiving() {
        // Start receiving schema1 data
        receiveSchema1Data();

        // Start receiving schema2 data
        receiveSchema2Data();
    }

    void receiveSchema1Data() {
        // Create a buffer to hold the received data
        std::vector<uint8_t> buffer1(1024); // Adjust the buffer size as needed

        socket_.async_read_some(boost::asio::buffer(buffer1), [this, buffer1](const boost::system::error_code& error, std::size_t bytesRead) {
            if (!error) {
                // Process the received schema1 data
                flatbuffers::Verifier verifier(buffer1.data(), bytesRead);
                if (your_schema1_namespace::VerifyYourSchema1DataBuffer(verifier)) {
                    auto data1 = your_schema1_namespace::GetYourSchema1Data(buffer1.data());
                    // Process the data1 object
                }
            }

            // Continue receiving schema1 data
            receiveSchema1Data();
        });
    }

    void receiveSchema2Data() {
        // Create a buffer to hold the received data
        std::vector<uint8_t> buffer2(1024); // Adjust the buffer size as needed

        socket_.async_read_some(boost::asio::buffer(buffer2), [this, buffer2](const boost::system::error_code& error, std::size_t bytesRead) {
            if (!error) {
                // Process the received schema2 data
                flatbuffers::Verifier verifier(buffer2.data(), bytesRead);
                if (your_schema2_namespace::VerifyYourSchema2DataBuffer(verifier)) {
                    auto data2 = your_schema2_namespace::GetYourSchema2Data(buffer2.data());
                    // Process the data2 object
                }
            }

            // Continue receiving schema2 data
            receiveSchema2Data();
        });
    }

    boost::asio::ip::tcp::socket socket_;
};

int main() {
    boost::asio::io_context ioContext;
    Client client(ioContext, "127.0.0.1", 1234); // Specify the server IP and port
    ioContext.run();

    return 0;
}
```

In this example, the client uses two separate asynchronous read operations to receive data of schema1 and schema2 concurrently. Each read operation is associated with its own buffer, and when data is received, it is processed accordingly based on its schema.

Note that you'll need to adjust the buffer sizes and the processing logic according to your specific requirements. Additionally, you may want to implement error handling, handle multiple connections, and structure your code in a more organized manner.

---

