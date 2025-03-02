#include "header_creator.h"
#include <array>
#include <asio.hpp>
#include <atomic>
#include <cmath>
#include <csignal>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include <type_traits>

namespace hardtime{

    class Sync {
        asio::io_context* ctx = nullptr;
        SharedMemoryCreator* f_memory = nullptr;
        std::mutex mut;
        bool is_reading = false;
        bool is_writing = false;
      
      public:

      Sync(asio::io_context& in_ctx): ctx{& in_ctx}{

      }

        bool can_write() {
          std::lock_guard<std::mutex> g{mut};
          if (!is_reading) {
            is_writing = true;
          }
        }
      
        bool parse_external(const watchdog_message &msg) {
          std::lock_guard<std::mutex> g{mut};
          if (!is_reading) {
            is_writing = true;
          }
        }

        operator bool(){
          if(ctx)
            return !ctx->stopped();
        }

        SharedMemoryCreator* memory(){
          return f_memory;
        }
      };

      constexpr watchdog_message_layout message_layout;

template<size_t watchdog_port>
class Sensor {
  asio::io_context io_context;

  const Timing timing_info;
  std::array<unsigned char, message_layout.watchdog_message_size> asio_memory_buffer;
  asio::ip::tcp::endpoint endpoit;
  asio::ip::tcp::acceptor acceptor;
  std::unique_ptr<SharedMemoryCreator> shared_memory;
  asio::ip::tcp::socket client_socket;
  watchdog_message message;

public:

  Sensor()
      : endpoit(asio::ip::tcp::v4(), watchdog_port),
        acceptor{io_context, endpoit} {
    std::signal(SIGINT, [this](int val) { io_context.stop(); });
    shared_memory = SharedMemoryCreator::create();
  }

  bool init_watchdog_communication() {
    asio::error_code ec;
    client_socket = acceptor.accept();
    if (ec) {
      std::cout << "failed to run, terminating....\n";
      std::cout << ec.message() << std::endl;
      return false;
    };
    return true;
  }

  operator bool() { return !io_context.stopped(); }

  template<typename Lambda>
  void loop_tick(Lambda&& arg) {
    asio::error_code ec;
    asio::read(client_socket, asio::buffer(asio_memory_buffer),
               asio::transfer_exactly(asio_memory_buffer.size()), ec);
    if (ec) {
      std::printf("failed to send information\n terminating....\n");
      std::raise(SIGINT);
    }
    copy_from_memory_to_watchdog_message(asio_memory_buffer.data(), message);
    std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    message.sensors_receive_timestamp = currently.time_since_epoch().count();

    asio::high_resolution_timer t(io_context);
    t.expires_from_now(timing_info.sensors_tick_duration);

    arg(message);

    t.wait();

    currently = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    message.sensors_send_timestamp = currently.time_since_epoch().count();
    copy_from_watchdog_message_to_memory(asio_memory_buffer.data(), message);
    asio::write(client_socket, asio::buffer(asio_memory_buffer),
                asio::transfer_exactly(asio_memory_buffer.size()), ec);
    if (ec) {
      std::printf("failed to send information\n terminating....\n");
      std::raise(SIGINT);
    }
  }
};



}