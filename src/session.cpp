//
// Created by miguellr on 10/07/2021.
//

#include "../include/session.hpp"
#include "../include/base64.hpp"
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

unsigned int buffToInteger(const char * buffer)
{
    auto a = static_cast<unsigned int>(static_cast<unsigned char>(buffer[0]) << 24 |
                             static_cast<unsigned char>(buffer[1]) << 16 |
                             static_cast<unsigned char>(buffer[2]) << 8 |
                             static_cast<unsigned char>(buffer[3]));
    return a;
}

void session::do_read(const char *data, boost::system::error_code error_code, std::size_t length) {
    if (state == RECEIVE_DATA_SIZE) {
        to_receive = buffToInteger(data);

        std::cout << "Starting to receive " << to_receive << " bytes of data" << '\n';

        state = RECEIVE_DATA;

        write(READ_TO_RECEIVE_DATA, 1);
    } else {
        if (received < to_receive) {
            temp_data.push_back(data);

            received += length;

            //std::cout << "Received: " << received << " of " << to_receive << '\n';

            if (received >= to_receive) {
                std::cout << "Received: " << received << " of " << to_receive << '\n';

                to_receive = 0;
                received = 0;

                std::string out;

                for (const auto &item : temp_data) {
                    out.append(item);
                }

                this->read_callback(out.data(), error_code, length);

                temp_data.clear();

                state = RECEIVE_DATA_SIZE;

                write(READ_TO_RECEIVE_SIZE, 1);
            } else {
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
