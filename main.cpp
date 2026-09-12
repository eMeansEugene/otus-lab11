//
// Created by evgen on 05.09.2026.
//

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "server.h"


namespace {

/**
 * @brief Разбирает строку в TCP-порт (1..65535).
 * @param arg Строковое представление порта из argv.
 * @param out Результат при успешном разборе.
 * @return true, если строка — корректный номер порта.
 */
bool ParsePort(const std::string& arg, std::uint16_t& out) {
    try {
        std::size_t pos = 0;
        const int value = std::stoi(arg, &pos);
        if (pos != arg.size() || value < 1 || value > 65535) {
            return false;
        }
        out = static_cast<std::uint16_t>(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief Разбирает строку в размер статического блока (> 0).
 * @param arg Строковое представление bulk_size из argv.
 * @param out Результат при успешном разборе.
 * @return true, если строка — положительное целое.
 */

} // namespace

int main(const int argc, char* argv[]) {
    using namespace join_server;


    if (argc != 2) {
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "bulk_server")
                  << " <port> \n";
        return 1;
    }

    std::uint16_t port = 0;
    if (!ParsePort(argv[1], port)) {
        std::cerr << "Invalid port: " << argv[1] << " (expected 1..65535)\n";
        return 1;
    }

    try {
        boost::asio::io_context io;
        Database db;
        Server server(io, port, db);
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}