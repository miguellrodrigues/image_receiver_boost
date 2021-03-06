//
// Created by miguellr on 10/07/2021.
//

#include "../include/session.hpp"

#include <boost/bind/bind.hpp>
#include <algorithm>
#include <string>

#define READ_TO_RECEIVE_DATA "."
#define RECEIVING_DATA "/"

session::session(
  tcp::socket socket,
  void (*read_callback)(const char *, boost::system::error_code, std::size_t),
  void (*write_callback)(const char *, boost::system::error_code, std::size_t)
) : _socket(std::move(socket)) {
  
  this->read_callback  = read_callback;
  this->write_callback = write_callback;
}

void session::start() {
  read();
}

session::message *session::buffToMessage(const char *buffer) {
  auto *buffered_message = (message *) buffer;
  
  auto *mn = static_cast<message *>(malloc(sizeof(message)));
  
  memcpy(mn, buffered_message, sizeof(*buffered_message));
  
  return mn;
}

void session::do_read(const char *data, boost::system::error_code error_code, std::size_t length) {
  if (std::string(data) == "EOF") {
//    std::cout << "Downloaded " << received / 1024 << " Kilo Bytes" << '\n';
    
    received = 0;
    
    std::sort(temp_data.begin(), temp_data.end(), [](message *a, message *b) {
        return a->id < b->id;
    });
    
    std::string out;
    
    for (const auto &packet : temp_data) {
      out.append(std::string(packet->data));
      delete packet;
    }
    
    this->read_callback(out.data(), error_code, length);
    
    out.clear();
    temp_data.clear();
    
    write(READ_TO_RECEIVE_DATA, 1);
  } else {
    temp_data.push_back(buffToMessage(data));
    
    received += length;
    
    //std::cout << "Received: " << length << '\n';
    
    write(RECEIVING_DATA, 1);
  }
}

void session::do_write(const char *data, boost::system::error_code error_code, std::size_t length) {
  this->write_callback(data, error_code, length);
  read();
}

void session::read() {
  auto self(shared_from_this());
  
  /*boost::asio::async_read(_socket, boost::asio::buffer(_data, max_length),
                           boost::bind(&session::do_read, self, _data, boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));*/
  
  _socket.async_receive(boost::asio::buffer(_data, max_length),
                        boost::bind(&session::do_read, self, _data,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
}

void session::write(const char *to_write, std::size_t length) {
  auto self(shared_from_this());
  
  
  _socket.async_write_some(boost::asio::buffer(to_write, length),
                           boost::bind(&session::do_write, self, to_write, boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
  
  /*boost::asio::async_write(_socket, boost::asio::buffer(to_write, length),
                           boost::bind(&session::do_write, self, _data, boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));*/
}
