//
// Created by miguellr on 10/07/2021.
//

#include "../include/session.hpp"
#include "../include/base64.hpp"
#include <boost/bind/bind.hpp>
#include <iostream>

session::session(
        tcp::socket socket,
        void (*read_callback)(const char *, boost::system::error_code, std::size_t),
        void (*write_callback)(const char *, boost::system::error_code, std::size_t)
) : _socket(std::move(socket)) {

    this->read_callback = read_callback;
    this->write_callback = write_callback;
}

void session::start() {
    read();
}

void session::do_read(const char *data, boost::system::error_code error_code, std::size_t length) {
    std::cout << length << '\n';

    this->read_callback(data, error_code, length);

    write(0);
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

void session::write(std::size_t length) {
    auto self(shared_from_this());

    boost::asio::async_write(_socket, boost::asio::buffer("", length),
                             boost::bind(&session::do_write, self, _data, boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}
