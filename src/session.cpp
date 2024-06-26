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
        void (*read_callback)(std::vector<unsigned char> &, boost::system::error_code, std::size_t),
        void (*write_callback)(std::vector<unsigned char> &, boost::system::error_code, std::size_t)
) : _socket(std::move(socket)) {

    this->read_callback = read_callback;
    this->write_callback = write_callback;
}

void session::start() {
    read();

    _out.reserve(10*max_length);
}

session::message *session::buffToMessage(const char *buffer) {
    auto *buffered_message = (message *) buffer;
    auto *mn = static_cast<message *>(malloc(sizeof(message)));
    memcpy(mn, buffered_message, sizeof(*buffered_message));

    return mn;
}

void session::do_read(const char *data, boost::system::error_code error_code, std::size_t length) {
    std::string data_str(data, length);
    state = data_str == "EOF" ? state_eof : state_data;

    switch (state) {
        case state_data:
            write(RECEIVING_DATA, 1);

            temp_data.insert(temp_data.end(), buffToMessage(data));
            received += length;
            break;
        case state_eof:
            write(READ_TO_RECEIVE_DATA, 1);

            received = 0;

            std::sort(temp_data.begin(), temp_data.end(), [](message *a, message *b) {
                return a->id < b->id;
            });

            bool stop = false;

            for (auto packet : temp_data) {
                unsigned int size = packet->size;
                if (size >= max_length) {
                    delete packet;
                    error_code = boost::asio::error::make_error_code(boost::asio::error::invalid_argument);
                    break;
                };

                _out.insert(_out.end(), packet->data, packet->data + size);
                delete packet;
            }

            temp_data.clear();

            this->read_callback(_out, error_code, received);

            _out.clear();
            break;
    }
}

void session::do_write(const char *data, boost::system::error_code error_code, std::size_t length) {
    std::vector<unsigned char> to_write_data(data, data + length);

    this->write_callback(to_write_data, error_code, length);
    read();
}

void session::read() {
    auto self(shared_from_this());

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
}
