//
// Created by miguellr on 10/07/2021.
//

#ifndef IMAGE_RECEIVER_BOOST_TCP_SERVER_HPP
#define IMAGE_RECEIVER_BOOST_TCP_SERVER_HPP

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class server {
public:
    server(
      boost::asio::io_context &io_context,
      short port,
      void (*read_callback)(const char *, boost::system::error_code, std::size_t),
      void (*write_callback)(const char *, boost::system::error_code, std::size_t))
      : _acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
      
      accept();
      
      this->read_callback  = read_callback;
      this->write_callback = write_callback;
    }

private:
    void accept();
    
    tcp::acceptor _acceptor;
    
    void (*read_callback)(const char *, boost::system::error_code, std::size_t);
    
    void (*write_callback)(const char *, boost::system::error_code, std::size_t);
};

#endif //IMAGE_RECEIVER_BOOST_TCP_SERVER_HPP
