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
            void (*read_callback)(std::vector<unsigned char>&, boost::system::error_code, std::size_t),
            void (*write_callback)(std::vector<unsigned char>&, boost::system::error_code, std::size_t));

    void start();

    enum {
        max_length = 1024 * 64,
        state_eof  = 1,
        state_data = 0
    };

    struct {
        unsigned int id;
        unsigned int size;
        char data[max_length];
    } typedef message;

private:
    void read();

    void write(const char *to_write, std::size_t length);

    void do_read(const char *, boost::system::error_code, std::size_t);

    void do_write(const char *, boost::system::error_code, std::size_t);

    void (*read_callback)(std::vector<unsigned char>&, boost::system::error_code, std::size_t);

    void (*write_callback)(std::vector<unsigned char>&, boost::system::error_code, std::size_t);

    static message *buffToMessage(const char *buffer);

    tcp::socket _socket;

    char _data[max_length]{};

    std::vector<message *> temp_data;
    std::vector<unsigned char> _out;

    unsigned int state    = 0;
    unsigned int received = 0;
};

#endif //IMAGE_RECEIVER_BOOST_SESSION_HPP
