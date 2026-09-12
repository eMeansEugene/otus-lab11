//
// Created by evgen on 05.09.2026.
//

#include <memory>

#include "server.h"
#include "session.h"

namespace ip = boost::asio::ip;
namespace join_server {
    Server::Server(boost::asio::io_context &io, const unsigned short port, Database &db) : acceptor_(
            io, ip::tcp::endpoint(ip::tcp::v4(), port)), db_(db) {
        DoAccept();
    }

    void Server::DoAccept() {
        acceptor_.async_accept(
            [this](const boost::system::error_code &ec, ip::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), db_)->Start();
                }
                DoAccept();
            });
    }
}