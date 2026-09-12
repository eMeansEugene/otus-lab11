//
// Created by evgen on 05.09.2026.
//

#ifndef JOIN_SERVER_SERVER_H
#define JOIN_SERVER_SERVER_H

#include <boost/asio.hpp>

#include "database.h"

namespace join_server {

    /**
     * @brief Асинхронный TCP-сервер: принимает соединения и порождает сессии.
     *
     * На каждое входящее подключение создаёт Session (во владении самой сессии
     * через shared_ptr) и передаёт ей общую базу данных. Слушает на всех
     * интерфейсах (tcp::v4()).
     */
    class Server {
    public:
        /**
         * @brief Создаёт сервер, привязывает акцептор к порту и запускает приём.
         * @param io   Контекст ввода-вывода, на котором работает акцептор.
         * @param port TCP-порт для входящих соединений (все интерфейсы).
         * @param db   Общее доменное хранилище; должно жить дольше сервера
         *             (и всех порождённых им сессий).
         */
        Server(boost::asio::io_context& io, unsigned short port, Database& db);

    private:
        /// Заказывает приём очередного соединения; в обработчике создаёт сессию
        /// и перезапускает себя (цикл приёма продолжается и после ошибки accept).
        void DoAccept();

        boost::asio::ip::tcp::acceptor acceptor_;
        Database& db_;
    };

} // namespace join_server

#endif // JOIN_SERVER_SERVER_H