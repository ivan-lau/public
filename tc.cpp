#include <boost/asio.hpp>
#include <iostream>
#include <fmt/core.h>
#include <boost/program_options.hpp>
#include <thread>
#include <atomic>
#include <csignal>
#include "order_generated.h"
#include "quote_generated.h"
#include "msg_type_generated.h"

using boost::asio::ip::tcp;
using namespace eqdwmm;

namespace po = boost::program_options;

void disable_term_key() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void reset_term_key() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag = (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

std::atomic<bool> client_running(true);
std::atomic<bool> print_orders(false);
std::atomic<bool> print_quotes(false);
std::atomic<bool> first_run(true);

void handle_sigint(int) {
    reset_term_key();
    print_orders = false;
    print_quotes = false;
}

void receiveOrders(tcp::socket& socket) {
    try {
        while (client_running) {
            if (print_orders) {

            uint32_t data_size = 0;
            boost::asio::read(socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
            std::vector<char> buffer(data_size);
            boost::asio::read(socket, boost::asio::buffer(buffer));

                auto order_response = flatbuffers::GetRoot<OrderResponse>(buffer.data());
                std::string header = fmt::format("| {:<12} | {:<15} | {:<10} | {:<10} |", "Order ID", "Instrument", "Price", "Quantity");
                std::string separator(header.length(), '-');
                std::string output = "\033[2J\033[H" + fmt::format("{}\n{}\n{}\n", separator, header, separator);

                for (const auto order : *order_response->orders()) {
                    output += fmt::format("| {:<12} | {:<15} | {:10.2f} | {:<10} |\n", order->id(), order->instrument()->c_str(), order->price(), order->quantity());
                }

                output += fmt::format("{}\n", separator);
                fmt::print("{}", output);
            }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (const std::exception& e) {
        std::cerr << "Disconnected from server\n\n";
        client_running = false;
        exit(EXIT_SUCCESS);
    }
}

void receiveQuotes(tcp::socket& socket) {
    try {
        while (client_running) {
            if (print_quotes) {
            uint32_t data_size = 0;
            boost::asio::read(socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
            std::vector<char> buffer(data_size);
            boost::asio::read(socket, boost::asio::buffer(buffer));

                auto quote_response = flatbuffers::GetRoot<QuoteResponse>(buffer.data());
                std::string header = fmt::format("| {:<5} | {:<15} | {:<10} | {:<10} | {:<10} | {:<10} |", "ID", "Instrument", "Bid #", "Bid", "Ask", "Ask #");
                std::string separator(header.length(), '-');
                std::string output = "\033[2J\033[H" + fmt::format("{}\n{}\n{}\n", separator, header, separator);

                for (const auto quote : *quote_response->quotes()) {
                    bool first = true;

                    for (const auto level : *quote->levels()) {
                        if (first) {
                            output += fmt::format("| {:<5} | {:<15} | {:<10} | {:<10.3f} | {:<10.3f} | {:<10} |\n",
                                                  quote->id(), quote->instrument()->c_str(), level->bid_volume(), level->bid(), level->ask(), level->ask_volume());
                            first = false;
                        } else {
                            output += fmt::format("| {:<5} | {:<15} | {:<10} | {:<10.3f} | {:<10.3f} | {:<10} |\n",
                                                  "", "", level->bid_volume(), level->bid(), level->ask(), level->ask_volume());
                        }
                    }

                    output += fmt::format("{}\n", separator);
                }

                fmt::print("{}", output);
            }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (const std::exception& e) {
        std::cerr << "Disconnected from server\n\n";
        client_running = false;
        exit(EXIT_SUCCESS);
    }
}

void sendCommands(tcp::socket& socket) {

    while (client_running) {
        if (!first_run && print_orders && print_quotes) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        first_run = false;

        std::string command;
        if (!print_orders && !print_quotes) {
            std::cout << "\nEnter command: ";
            std::getline(std::cin, command);

            if (std::cin.fail()) {
                std::cin.clear();
                std::cout << "\n";
                client_running = false;
                break;
            }

            if (command.empty())
                continue;

            if (command == "orders") {
                disable_term_key();

                flatbuffers::FlatBufferBuilder builder;
                auto order_request = CreateOrderRequest(builder);
                auto msg_type_holder = CreateMsgTypeHolder(builder, MsgType_OrderRequest, order_request.Union());
                builder.Finish(msg_type_holder);
                uint32_t data_size = builder.GetSize();

                try {
                    boost::asio::write(socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
                    boost::asio::write(socket, boost::asio::buffer(builder.GetBufferPointer(), builder.GetSize()));
                } catch (boost::system::system_error& e) {
                    std::cerr << "Write error: " << e.what() << std::endl;
                    break;
                }

                print_quotes = false;
                print_orders = true;
            } else if (command == "quotes") {
                disable_term_key();

                flatbuffers::FlatBufferBuilder builder;
                auto quote_request = CreateQuoteRequest(builder);
                auto msg_type_holder = CreateMsgTypeHolder(builder, MsgType_QuoteRequest, quote_request.Union());
                builder.Finish(msg_type_holder);
                uint32_t data_size = builder.GetSize();

                try {
                    boost::asio::write(socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
                    boost::asio::write(socket, boost::asio::buffer(builder.GetBufferPointer(), builder.GetSize()));
                } catch (boost::system::system_error& e) {
                    std::cerr << "Write error: " << e.what() << std::endl;
                    break;
                }

                print_orders = false;
                print_quotes = true;
            } else if (command == "exit") {
                client_running = false;
                break;
            } else if (std::cin.eof()) {
                std::cout << std::endl;
                client_running = false;
                break;
            } else {
                std::cout << "Unknown command: " << command << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, handle_sigint);

    std::atexit([]() {
        reset_term_key();
    });


    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("ip,i", po::value<std::string>()->default_value("localhost"), "set ip address")
            ("port,p", po::value<int>(), "set port number");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 1;
        }

        if (!vm.count("port")) {
            std::cout << "Port number was not set. Use -h or --help to see more options." << std::endl;
            return 1;
        }

        auto port = vm["port"].as<int>();
        auto ip = vm["ip"].as<std::string>();

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(ip, std::to_string(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        std::thread receive_order_thread(receiveOrders, std::ref(socket));
        std::thread receive_quote_thread(receiveQuotes, std::ref(socket));
        std::thread send_thread(sendCommands, std::ref(socket));

        // receive_order_thread.join();
        receive_quote_thread.join();
        send_thread.join();
    } catch (const boost::system::system_error& e) {
        if (std::string(e.what()).find("Connection refused") != std::string::npos) {
            std::cerr << "Error: Unable to connect to the server. Please check if the server is running and accepting connections." << std::endl;
        } else {
            std::cerr << "Boost system error: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return 0;
    }

    return 0;
}


#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>
#include <string>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include "order_generated.h"
#include "quote_generated.h"
#include "msg_type_generated.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis1(1, 100);
std::uniform_real_distribution<> dis2(1.0, 10.0);
std::uniform_int_distribution<> dis3(1000, 9000);
std::uniform_int_distribution<> dis4(1, 5);

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] = "0123456789";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };

    std::string str(length, '0');
    std::generate_n(str.begin(), length, randchar);
    return str;
}

using boost::asio::ip::tcp;
using namespace eqdwmm;
namespace po = boost::program_options;

std::atomic<bool> server_running(true);
std::vector<std::shared_ptr<tcp::socket>> sockets;

void sendOrders(std::shared_ptr<tcp::socket> socket) {
    flatbuffers::FlatBufferBuilder builder;
    try {
        while (server_running) {
            std::vector<flatbuffers::Offset<Order>> orders;
            for (int i = 0; i < static_cast<int>(dis2(gen) - 1); ++i) {
                int id = dis1(gen);
                std::string code = "BUR_" + random_string(4);
                float price = dis2(gen);
                int quantity = dis3(gen);
                orders.push_back(CreateOrder(builder, id, builder.CreateString(code), price, quantity));
            }
            
            auto orders_vector = builder.CreateVector(orders);
            auto order_response = CreateOrderResponse(builder, orders_vector);
            builder.Finish(order_response);

            uint32_t data_size = builder.GetSize();
            boost::asio::write(*socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
            boost::asio::write(*socket, boost::asio::buffer(builder.GetBufferPointer(), builder.GetSize()));

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Client disconnected" << std::endl;
    }
}


void sendQuotes(std::shared_ptr<tcp::socket> socket) {
    flatbuffers::FlatBufferBuilder builder;
    try {
        while (server_running) {

            std::vector<flatbuffers::Offset<Quote>> quotes;
            for (int i = 0; i < static_cast<int>(dis2(gen)) - 1; ++i) {
                std::vector<flatbuffers::Offset<Level>> levels;
                for (int j = 0; j < dis4(gen); ++j) {
                    levels.push_back(CreateLevel(builder, dis2(gen), dis2(gen), dis3(gen), dis3(gen)));
                }
                auto instrument = builder.CreateString("BUR_" + random_string(4));
                auto levels_vector = builder.CreateVector(levels);
                quotes.push_back(CreateQuote(builder, i, instrument, levels_vector));
            }

            auto quotes_vector = builder.CreateVector(quotes);
            auto quote_response = CreateQuoteResponse(builder, quotes_vector);
            builder.Finish(quote_response);

            uint32_t data_size = builder.GetSize();
            boost::asio::write(*socket, boost::asio::buffer(&data_size, sizeof(uint32_t)));
            boost::asio::write(*socket, boost::asio::buffer(builder.GetBufferPointer(), builder.GetSize()));

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Client disconnected" << std::endl;
    }
}

void handleClient(std::shared_ptr<tcp::socket> socket) {
    try {
        while (socket->is_open() && server_running) {
            uint32_t data_size = 0;
            boost::system::error_code read_error;
            boost::asio::read(*socket, boost::asio::buffer(&data_size, sizeof(uint32_t)), read_error);
            if (read_error) {
                if (read_error == boost::asio::error::eof) {
                    break;
                } else {
                    throw boost::system::system_error(read_error);
                }
            }
            std::vector<uint8_t> data(data_size);
            boost::asio::read(*socket, boost::asio::buffer(data.data(), data_size), read_error);
            if (read_error) {
                if (read_error == boost::asio::error::eof) {
                    break;
                } else {
                    throw boost::system::system_error(read_error);
                }
            }

            uint8_t* buffer = data.data();
            auto holder = eqdwmm::GetMsgTypeHolder(buffer);
            if (holder->msg_type_type() == MsgType_OrderRequest) {
                std::cout << "Received order request" << std::endl;
                // Process the order request
                //
                std::thread(sendOrders, socket).detach();
            } else if (holder->msg_type_type() == MsgType_QuoteRequest) {
                std::cout << "Received quote request" << std::endl;
                // Process the quote request
                //
                std::thread(sendQuotes, socket).detach();
            } else if (holder->msg_type_type() == MsgType_TradeRequest) {
                std::cout << "Received trade request" << std::endl;
                // Process the trade request
                //
            }
        }
    } catch (const std::exception& e) {
    }

    // cleanup code
    socket->close();
}

int main(int argc, char* argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("port,p", po::value<int>(), "set port number");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 1;
        }

        if (!vm.count("port")) {
            std::cout << "Port number was not set. Use -h or --help to see more options." << std::endl;
            return 1;
        }

        auto port = vm["port"].as<int>();
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        while (server_running) {
            std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(io_service);
            acceptor.accept(*socket);
            sockets.push_back(socket);

            std::thread clientThread(handleClient, socket);
            clientThread.detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}
