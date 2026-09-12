//
// Created by evgen on 05.09.2026.
//

#include "session.h"

#include <iostream>

#include "parser.h"


namespace join_server {
    Session::Session(boost::asio::ip::tcp::socket socket, Database &db) : socket_(std::move(socket)), db_(db) {
    }

    void Session::Start() {
        DoRead();
    }

    void Session::DoRead() {
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            [self = shared_from_this()](const boost::system::error_code &ec, const size_t n) {
                self->OnRead(ec, n);
            });
    }

    void Session::OnRead(const boost::system::error_code &ec, const std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) {
            if (buffer_[i] == '\n') {
                HandleLine(pending_line_);
                pending_line_.clear();
            } else {
                pending_line_ += buffer_[i];
            }
        }
        if (!ec) {
            DoRead();
        } else if (ec == boost::asio::error::eof) {
            Finalize();
        } else {
            std::cout << "Error: " << ec.message() << "\n"; // сессия не продолжает читать → умрёт
        }
    }

    void Session::HandleLine(const std::string &line) {
        const auto res = Parse(line);
        if (const auto error = std::get_if<ParseError>(&res)) {
            Reply("ERR " + error->message + "\n");
        } else {
            std::vector<std::string> result;
            auto c = std::get<Command>(res);
            switch (c.type) {
                case CommandType::Insert: {
                    if (db_.Insert(c.table, c.id, c.name)) {
                        Reply("OK\n");
                    } else {
                        Reply("ERR duplicate " + std::to_string(c.id) + "\n");
                    }
                    break;
                }
                case CommandType::Truncate: {
                    db_.Truncate(c.table);
                    Reply("OK\n");
                    break;
                }

                case CommandType::Intersection: {
                    result = db_.Intersection();
                    for (const auto &item: result) {
                        Reply(item + "\n");
                    }
                    Reply("OK\n");
                    break;
                }
                case CommandType::SymmetricDifference: {
                    result = db_.SymmetricDifference();
                    for (const auto &item: result) {
                        Reply(item + "\n");
                    }
                    Reply("OK\n");
                    break;
                }
            }
        }
    }

    void Session::Reply(const std::string &msg) {
        const bool is_empty = queue_.empty();
        queue_.emplace_back(msg);
        if (is_empty) {
            DoWrite();
        }
    }

    void Session::DoWrite() {
        boost::asio::async_write(
            socket_, boost::asio::buffer(queue_.front()),
            [self = shared_from_this()](const boost::system::error_code &ec, const std::size_t n) {
                self->OnWrite(ec, n);
            });
    }

    void Session::OnWrite(boost::system::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "write error: " << ec.message() << "\n";
            return;
        }
        queue_.pop_front();
        if (!queue_.empty()) DoWrite();
    }

    void Session::Finalize() {
        if (!pending_line_.empty()) {
            HandleLine(pending_line_);
            pending_line_.clear();
        }
    }
}
