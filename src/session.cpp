//
// Created by miguellr on 10/07/2021.
//

#include "../include/session.hpp"
#include <boost/bind/bind.hpp>
#include <iostream>
#include <string>

#define READ_TO_RECEIVE_DATA "."
#define RECEIVING_DATA ";"
#define READ_TO_RECEIVE_SIZE "/"

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
    if (to_receive == 0) {
        to_receive = int((u_char)data[0] << 24 |
                         (u_char)data[1] << 16 |
                         (u_char)data[2] << 8  |
                         (u_char)data[3]);

        std::cout << "Starting to receive " << to_receive << " bytes of data" << '\n';

        write(READ_TO_RECEIVE_DATA, 1);
    } else {
        if (received < to_receive) {
            if (received + length >= to_receive) {
                temp_data.push_back(data);

                std::string eujanaoseioquefazer;

                for (const auto &item : temp_data) {
                    eujanaoseioquefazer.append(item);
                }

                temp_data.clear();

                this->read_callback(eujanaoseioquefazer.data(), error_code, length);

                to_receive = 0;
                received = 0;

                write(READ_TO_RECEIVE_SIZE, 1);
            } else {
                received += length;

                std::cout << "Received: " << received << " of " << to_receive << '\n';

                write(RECEIVING_DATA, 1);
            }
        }
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

void session::write(const char * to_write, std::size_t length) {
    auto self(shared_from_this());

    boost::asio::async_write(_socket, boost::asio::buffer(to_write, length),
                             boost::bind(&session::do_write, self, _data, boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}
