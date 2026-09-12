//
// Created by evgen on 12.09.2026.
//

#ifndef JOIN_SERVER_PARSER_H
#define JOIN_SERVER_PARSER_H

#include <string>
#include <string_view>
#include <variant>

#include "database.h"  // Table

namespace join_server {

    /// Тип распознанной команды. Определяет, какие поля Command осмысленны.
    enum class CommandType { Insert, Truncate, Intersection, SymmetricDifference };

    /**
     * @brief Разобранная команда.
     *
     * Тегированная структура: по type видно, какие поля заполнены —
     * Insert использует все; Truncate — только table; INTERSECTION и
     * SYMMETRIC_DIFFERENCE — только type.
     */
    struct Command {
        CommandType type;
        Table       table{};  ///< Осмысленно для Insert, Truncate.
        int         id{};     ///< Осмысленно для Insert.
        std::string name;     ///< Осмысленно для Insert.
    };

    /**
     * @brief Синтаксическая ошибка разбора команды.
     *
     * message — голый текст без "ERR " и без \n; обрамление протокола
     * добавляет транспортный слой (Session).
     */
    struct ParseError {
        std::string message;
    };

    /// Результат разбора: либо готовая команда, либо ошибка синтаксиса.
    using ParseResult = std::variant<Command, ParseError>;

    /**
     * @brief Разбирает одну строку команды (уже без завершающего \n).
     *
     * Проверяется только синтаксис: имя команды, число аргументов, буква
     * таблицы (A/B), числовой id. Домен не вызывается. Дубль id выявляется
     * не здесь, а при исполнении в Database::Insert.
     *
     * @param line Строка команды без завершающего перевода строки.
     * @return Command при успешном разборе; ParseError с текстом при
     *         синтаксической ошибке.
     */
    ParseResult Parse(std::string_view line);

} // namespace join_server

#endif //JOIN_SERVER_PARSER_H