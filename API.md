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
            double buyPrice = buyIt->first;
            std::list<int>& buyList = buyIt->second;

            auto sellIt = sellOrders.find(buyPrice);
            if (sellIt == sellOrders.end()) {
                ++buyIt;
                continue;
            }
    
            std::list<int>& sellList = sellIt->second;

            auto buyOrderIt = buyList.begin();
            while (buyOrderIt != buyList.end()) {
                auto sellOrderIt = sellList.begin();
                while (sellOrderIt != sellList.end()) {
                    int buyOrderId = *buyOrderIt;
                    int sellOrderId = *sellOrderIt;

                    const Order& buyOrder = *orderIds[buyOrderId];
                    const Order& sellOrder = *orderIds[sellOrderId];

                    int matchedVolume = std::min(buyOrder.quantity, sellOrder.quantity);
                    if (matchedVolume > 0) {
                        // Match the orders...
                        matchOrder(buyOrder, sellOrder);
    
                        buyOrder.quantity -= matchedVolume;
                        sellOrder.quantity -= matchedVolume;
    
                        // Remove the matched orders from the lists if the volume is fully matched...
                        if (buyOrder.quantity == 0) {
                            buyOrderIt = buyList.erase(buyOrderIt);
                        }
                        if (sellOrder.quantity == 0) {
                            sellOrderIt = sellList.erase(sellOrderIt);
                        }
                    } else {
                        // No volume match, move to the next sell order...
                        ++sellOrderIt;
                    }
                }

                if (sellOrderIt == sellList.end()) {
                    ++buyOrderIt;
                }
            }

            if (buyList.empty()) {
                buyIt = std::map<double, std::list<int>>::reverse_iterator(buyOrders.erase(std::next(buyIt).base()));
            } else {
                ++buyIt;
            }
    
            if (sellList.empty()) {
                sellOrders.erase(sellIt);
            }
        }
    }

void matchOrder(const Order& buyOrder, const Order& sellOrder) {
    int buyOrderId = -1;
    int sellOrderId = -1;

    // Find the buy order ID
    for (const auto& entry : orderIds) {
        if (entry.second == &buyOrder) {
            buyOrderId = entry.first;
            break;
        }
    }

    // Find the sell order ID
    for (const auto& entry : orderIds) {
        if (entry.second == &sellOrder) {
            sellOrderId = entry.first;
            break;
        }
    }

    if (buyOrderId != -1 && sellOrderId != -1) {
        std::cout << "Matched: "
                  << "Buy Order ID=" << buyOrderId << ", "
                  << "Sell Order ID=" << sellOrderId << ", "
                  << "Price=" << buyOrder.price << ", "
                  << "Quantity=" << std::min(buyOrder.quantity, sellOrder.quantity) << std::endl;
    }
}

void printOrderBook() {
    std::cout << "Buy Orders:" << std::endl;
    for (auto it = buyOrders.rbegin(); it != buyOrders.rend(); ++it) {
        double buyPrice = it->first;
        const std::list<int>& buyList = it->second;

        std::cout << "Price: " << buyPrice << std::endl;
        for (const auto& orderId : buyList) {
            const Order& order = *orderIds[orderId];
            std::cout << "Order ID: " << order.id << ", Quantity: " << order.quantity << std::endl;
        }
    }

    std::cout << "Sell Orders:" << std::endl;
    for (const auto& entry : sellOrders) {
        double sellPrice = entry.first;
        const std::list<int>& sellList = entry.second;

        std::cout << "Price: " << sellPrice << std::endl;
        for (const auto& orderId : sellList) {
            const Order& order = *orderIds[orderId];
            std::cout << "Order ID: " << order.id << ", Quantity: " << order.quantity << std::endl;
        }
    }
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

FROM centos:7

RUN yum -y install gcc gcc-c++ glibc-static python python-devel libicu libicu-devel zlib zlib-devel bzip2 bzip2-devel wget which make openssl-devel
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz
RUN tar xzf gcc-8.2.0.tar.gz

WORKDIR /gcc-8.2.0

RUN wget --no-check-certificate https://ftp.gnu.org/gnu/gmp/gmp-6.1.0.tar.bz2
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/mpfr/mpfr-3.1.4.tar.bz2
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz
RUN wget --no-check-certificate https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2

RUN ./contrib/download_prerequisites

RUN ./configure --prefix=/usr/local/gcc-8.2.0 --enable-bootstrap --enable-checking=release --enable-languages=c,c++ --disable-multilib --enable-threads=posix
RUN make -j$(nproc)
RUN make install

ENV PATH="/usr/local/gcc-8.2.0/bin:${PATH}"
ENV LD_LIBRARY_PATH="/usr/local/gcc-8.2.0/lib64:${LD_LIBRARY_PATH}"

WORKDIR /usr/lib64
RUN rm -f libstdc++.so.6
RUN ln -s /usr/local/gcc-8.2.0/lib64/libstdc++.so.6.0.25 libstdc++.so.6

RUN yum remove -y gcc

WORKDIR /

RUN wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4.tar.gz
RUN tar xzf cmake-3.26.4.tar.gz

WORKDIR /cmake-3.26.4

RUN ./bootstrap
RUN make -j$(nproc)
RUN make install

WORKDIR /

---






