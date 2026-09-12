//
// Created by evgen on 03.09.2026.
//

#ifndef JOIN_SERVER_SESSION_H
#define JOIN_SERVER_SESSION_H

#include <array>
#include <deque>
#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "database.h"

namespace join_server {

/**
 * @brief Обслуживает одно клиентское TCP-соединение (асинхронный «читатель-писатель»).
 *
 * Читает байты из сокета через async_read_some, склеивает их в строки по '\n'
 * (недописанный хвост переживает границы пакетов в pending_line_). Каждую готовую
 * строку разбирает парсером, исполняет на общем Database и отправляет клиенту
 * ответ протокола (OK / строки результата / ERR). Исходящие ответы проходят через
 * очередь: в любой момент в полёте не более одной async_write на сокет.
 *
 * Время жизни управляется через enable_shared_from_this: пока есть незавершённая
 * асинхронная операция (чтение или запись), копия shared_ptr в её обработчике
 * удерживает объект живым. После закрытия соединения сессия уничтожается сама.
 * Создавать только через std::make_shared.
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    /**
     * @brief Создаёт сессию поверх принятого сокета.
     * @param socket Подключённый сокет клиента (передаётся во владение).
     * @param db     Общее доменное хранилище; должно жить дольше сессии.
     */
    Session(boost::asio::ip::tcp::socket socket, Database& db);

    /**
     * @brief Запускает обслуживание: инициирует первое асинхронное чтение.
     */
    void Start();

private:
    /// Заказывает очередное асинхронное чтение; обработчик удерживает
    /// сессию живой через shared_from_this.
    void DoRead();

    /// Обработчик чтения. Сначала разбирает полученные n байт (даже если
    /// одновременно пришёл eof — команды перед закрытием не теряются), затем
    /// по коду ошибки продолжает чтение, финализирует или логирует ошибку.
    void OnRead(const boost::system::error_code& ec, std::size_t n);

    /// Обрабатывает одну готовую строку: Parse → Database → постановка ответа.
    void HandleLine(const std::string& line);

    /// Ставит готовый ответ (уже с \n) в очередь исходящих; если запись
    /// не идёт — запускает DoWrite. Единственная точка входа для отправки.
    void Reply(const std::string& msg);

    /// Пишет голову очереди в сокет. Ровно одна async_write в полёте.
    void DoWrite();

    /// Завершение записи: pop головы; если очередь не пуста — следующая DoWrite.
    void OnWrite(boost::system::error_code ec, std::size_t n);

    /// Досылает недописанный хвост pending_line_ при закрытии соединения
    /// (последняя команда без завершающего '\n' не теряется).
    void Finalize();

    boost::asio::ip::tcp::socket socket_;
    Database& db_;
    std::array<char, 1024> buffer_{};
    std::string pending_line_;
    std::deque<std::string> queue_;  ///< Очередь исходящих; непустота = запись в полёте.
};

} // namespace join_server

#endif // JOIN_SERVER_SESSION_H