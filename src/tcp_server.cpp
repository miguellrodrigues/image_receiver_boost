//
// Created by miguellr on 10/07/2021.
//

#include "../include/tcp_server.hpp"
#include "../include/session.hpp"

void tcp_server::accept() {
  _acceptor.async_accept(
    [this](boost::system::error_code error_code, tcp::socket socket) {
        if (!error_code) {
          std::make_shared<session>(std::move(socket), this->read_callback, this->write_callback)->start();
        }
        
        accept();
    });
}
