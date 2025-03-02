#include <asio.hpp>
#include <ctime>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>
#include <array>
#include <csignal>
#include "header_acessor.h"

asio::io_context io_context;
constexpr watchdog_message_layout message_layout;

void signal_handler(int signal){
    io_context.stop();
};
constexpr Timing timing;
constexpr auto maximum_delay_in_milliseconds = timing.tick_duration;
struct Client;

void safety_shutdown(Client* control_law);

struct Client{
  bool data_sent = false;
  std::array<unsigned char,message_layout.watchdog_message_size> allocated_memory_buffer;
  asio::io_context& context;
  asio::ip::tcp::socket sensor_socket_;
  asio::ip::tcp::socket client_socket_;
  asio::high_resolution_timer timer;
  watchdog_message message;

  explicit Client(asio::io_context& in_context,
                    asio::ip::tcp::socket&& sensor_socket,
                    asio::ip::tcp::socket&& client_socket
                                                      ) : context{in_context} ,
                                                         sensor_socket_{std::move(sensor_socket)},
                                                        client_socket_{ std::move(client_socket) },
                                                         timer{in_context}{}

  Client(const Client & copyclient) = delete;

  Client(Client && client) = delete;

  ~Client(){
    timer.cancel();
    safety_shutdown(this);
  }
};

void safety_shutdown(Client* control_law){
   std::cout << "terminating with safety stop because something went wrong (shutting down controller)" << std::endl;
   std::cout.flush();
}

void write_control(Client& ref);
void request_sensors_acquistion(Client& ref);
void read_sensors_acknowledgment(Client& ref);
void warn_client_of_readings(Client& ref);
void read_client_acknowledgment(Client& ref);

void write_control(Client& client){
  client.timer.expires_from_now(maximum_delay_in_milliseconds);
  client.timer.async_wait([&](asio::error_code ec) { // handles what happens when timer ends
    if(client.data_sent){
      client.data_sent = false;
      write_control(client);
      return ;
    }
    else{
      client.context.stop();
      return ;
    } 
  });
  //----------------------- here we should do our control law --------------------

  //------------------------------------------------------------------------------
  std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now()
  );
  std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
  client.message.watchdog_command_timestamp = millis_since_utc_epoch.count();
  request_sensors_acquistion(client);
}

void request_sensors_acquistion(Client& client) {
  std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now()
  );
  std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
  client.message.watchdog_sensor_reqst_timestamp = millis_since_utc_epoch.count();
  copy_from_watchdog_message_to_memory(client.allocated_memory_buffer.data(),client.message);
  asio::async_write( client.sensor_socket_,asio::buffer(client.allocated_memory_buffer),asio::transfer_exactly(client.allocated_memory_buffer.size()),
    [ &client](asio::error_code ec, size_t /*length*/) {
        if (ec) {
          std::cout << "failure detected in request_sensors_acquistion\n";
          return ;
        }
        read_sensors_acknowledgment(client);
  });
};

void read_sensors_acknowledgment(Client& client){
  asio::async_read( client.sensor_socket_, asio::buffer(client.allocated_memory_buffer),asio::transfer_exactly(client.allocated_memory_buffer.size()), 
    [ &client](asio::error_code ec, size_t /*length*/) {
      if (ec) {
        std::cout << "failure detected in read_sensors_acknowledgment\n";
        return ;
      } 
      copy_from_memory_to_watchdog_message(client.allocated_memory_buffer.data(),client.message);
      std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now()
      );
      std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
      client.message.watchdog_sensor_receive_timestamp = millis_since_utc_epoch.count();
      warn_client_of_readings(client);
  });
}
void warn_client_of_readings(Client& client){
  std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now()
  );
  std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
  client.message.watchdog_client_reqst_timestamp = millis_since_utc_epoch.count();
  copy_from_watchdog_message_to_memory(client.allocated_memory_buffer.data(),client.message);
  asio::async_write( client.client_socket_,asio::buffer(client.allocated_memory_buffer),asio::transfer_exactly(client.allocated_memory_buffer.size()),
    [ &client](asio::error_code ec, size_t /*length*/) {
        if (ec) {
          std::cout << "failure detected in warn_client_of_readings\n";
          return ;
        }
        read_client_acknowledgment(client);
  });
}

void read_client_acknowledgment(Client& client) {
  //std::cout << "read_client_acknowledgment\n";
  asio::async_read( client.client_socket_, asio::buffer(client.allocated_memory_buffer),asio::transfer_exactly(client.allocated_memory_buffer.size()), 
    [ &client](asio::error_code ec, size_t /*length*/) {
      if (ec) {
        std::cout << "failure detected in read_client_acknowledgment\n";
        return ;
      } 
      copy_from_memory_to_watchdog_message(client.allocated_memory_buffer.data(),client.message);
      std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now()
      );
      std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
      client.message.watchdog_client_receive_timestamp= millis_since_utc_epoch.count();
      client.data_sent = true;
  });
}

int main(int argc, char* argv[])
{
  std::signal(SIGINT, signal_handler);

  asio::ip::tcp::socket sensor_socket(io_context);
  asio::ip::tcp::resolver sensor_resolver(io_context);
  asio::connect(sensor_socket, sensor_resolver.resolve("127.0.0.1","50000"));

  asio::ip::tcp::socket client_socket(io_context);
  asio::ip::tcp::resolver client_resolver(io_context);
  asio::connect(client_socket, client_resolver.resolve("127.0.0.1","50010"));

  Client watchgod{io_context,std::move(sensor_socket),std::move(client_socket) };
  
  write_control(watchgod);

  io_context.run();
  return 0;
};