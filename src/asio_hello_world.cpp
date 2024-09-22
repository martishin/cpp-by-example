#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <iostream>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::ip::tcp;
using boost::asio::use_awaitable;

// Coroutine to handle client connections asynchronously
awaitable<void> handle_client(tcp::socket socket) {
    try {
        char data[1024];
        std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);

        // Simulate a long-running operation with a timer
        boost::asio::steady_timer timer(socket.get_executor());
        timer.expires_after(std::chrono::seconds(5));
        co_await timer.async_wait(use_awaitable);

        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        co_await async_write(socket, boost::asio::buffer(response), use_awaitable);

        socket.shutdown(tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Error in client handler: " << e.what() << std::endl;
    }
}

// Coroutine to accept client connections asynchronously
awaitable<void> accept_connections(tcp::acceptor& acceptor) {
    while (true) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(acceptor.get_executor(), handle_client(std::move(socket)), detached);
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        co_spawn(io_context, accept_connections(acceptor), detached);

        std::cout << "Server running on port 8080..." << std::endl;

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
