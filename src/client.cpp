#include <array>
#include <atomic>
#include <mutex>
#include <iostream>
#include <csignal>
#include <thread>
#include <sstream>
#include <asio.hpp>
#include <cmath>
#include <type_traits>
#include "header_acessor.h"

asio::io_context io_content;
asio::ip::tcp::socket* socket_pointer = nullptr;

void signal_handler(int val){
    if(socket_pointer!=nullptr)
        socket_pointer->shutdown(asio::ip::tcp::socket::shutdown_both);
    io_content.stop();
}

int main(){
    std::signal(SIGINT,signal_handler);

    constexpr watchdog_message_layout message_layout;

    std::array<unsigned char,message_layout.watchdog_message_size> asio_memory_buffer;

    unsigned int port = 50010;
    asio::ip::tcp::endpoint endpoit(asio::ip::tcp::v4(), port);
    asio::ip::tcp::acceptor acceptor(io_content,endpoit);

    asio::error_code ec;
    asio::ip::tcp::socket client_socket = acceptor.accept();
    if (ec){
        std::cout << "failed to run, terminating....\n";
        std::cout << ec.message() << std::endl;
        return 1;
    };
    socket_pointer = &client_socket;
    watchdog_message message;
    auto shared_memory = SharedMemoryAccessor::create();

    if(shared_memory->get_shared_memory_address()==nullptr){
        std::cout << "failure to create shared memory\n";
        return 1;
    }
   while(!io_content.stopped()){
        asio::read(client_socket, asio::buffer(asio_memory_buffer), asio::transfer_exactly(asio_memory_buffer.size()), ec);
        if (ec) {
            std::printf("failed to read information\n terminating....\n");
            io_content.stop();
        }

        std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        message.client_receive_timestamp = currently.time_since_epoch().count();

        if(message.gps_reading){
            
        }

        if(message.grayscale_image_reading){

        }
        
        copy_from_watchdog_message_to_memory(asio_memory_buffer.data(),message);
        asio::write(client_socket,asio::buffer(asio_memory_buffer),asio::transfer_exactly(asio_memory_buffer.size()),ec);
        if(ec){
            std::printf("failed to send control action\n terminating....\n");
            io_content.stop();
        }
    }
}