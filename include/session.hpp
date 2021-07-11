//
// Created by miguellr on 10/07/2021.
//

#ifndef IMAGE_RECEIVER_BOOST_SESSION_HPP
#define IMAGE_RECEIVER_BOOST_SESSION_HPP

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {

public:
    explicit session(
            tcp::socket socket,
            void (*read_callback)(const char *, boost::system::error_code, std::size_t),
            void (*write_callback)(const char *, boost::system::error_code, std::size_t));

    void start();

private:
    void read();

    void write(const char * to_write, std::size_t length);

    void do_read(const char *, boost::system::error_code, std::size_t);

    void do_write(const char *, boost::system::error_code, std::size_t);

    void (*read_callback)(const char *, boost::system::error_code, std::size_t);

    void (*write_callback)(const char *, boost::system::error_code, std::size_t);

    tcp::socket _socket;

    enum {
        max_length = 1024 * 64
    };

    char _data[max_length]{};

    std::vector<const char *> temp_data;

    unsigned int to_receive = 0;
    unsigned int received = 0;
};

#endif //IMAGE_RECEIVER_BOOST_SESSION_HPP
