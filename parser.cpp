//
// Created by evgen on 12.09.2026.
//

#include "parser.h"

#include <charconv>
#include <vector>

namespace join_server {
namespace {

/// Режет строку на токены по пробелам. Прогоны пробелов схлопываются,
/// ведущие/хвостовые игнорируются — так разбор устойчив к лишним пробелам.
std::vector<std::string_view> Tokenize(std::string_view line) {
    std::vector<std::string_view> tokens;
    std::size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && line[i] == ' ') ++i;   // пропустить пробелы
        const std::size_t start = i;
        while (i < line.size() && line[i] != ' ') ++i;   // добрать токен
        if (i > start) tokens.push_back(line.substr(start, i - start));
    }
    return tokens;
}

/// "A"/"B" -> Table. false, если токен — не буква таблицы.
bool ParseTable(std::string_view tok, Table& out) {
    if (tok == "A") { out = Table::A; return true; }
    if (tok == "B") { out = Table::B; return true; }
    return false;
}

/// Строгий разбор int: false при нечисле И при мусоре-хвосте ("12abc").
bool ParseInt(std::string_view tok, int& out) {
    const char* first = tok.data();
    const char* last  = tok.data() + tok.size();
    const auto [ptr, ec] = std::from_chars(first, last, out);
    return ec == std::errc() && ptr == last;   // без ошибки И токен сгрызен целиком
}

} // namespace

ParseResult Parse(std::string_view line) {
    const auto tokens = Tokenize(line);
    if (tokens.empty()) {
        return ParseError{"empty command"};
    }

    const std::string_view cmd = tokens[0];

    if (cmd == "INSERT") {
        if (tokens.size() != 4) {
            return ParseError{"INSERT expects: INSERT table id name"};
        }
        Command c;
        c.type = CommandType::Insert;
        if (!ParseTable(tokens[1], c.table)) {
            return ParseError{"unknown table " + std::string(tokens[1])};
        }
        if (!ParseInt(tokens[2], c.id)) {
            return ParseError{"invalid id " + std::string(tokens[2])};
        }
        c.name = std::string(tokens[3]);
        return c;
    }

    if (cmd == "TRUNCATE") {
        if (tokens.size() != 2) {
            return ParseError{"TRUNCATE expects: TRUNCATE table"};
        }
        Command c;
        c.type = CommandType::Truncate;
        if (!ParseTable(tokens[1], c.table)) {
            return ParseError{"unknown table " + std::string(tokens[1])};
        }
        return c;
    }

    if (cmd == "INTERSECTION") {
        if (tokens.size() != 1) {
            return ParseError{"INTERSECTION expects no arguments"};
        }
        return Command{CommandType::Intersection, {}, {}, {}};
    }

    if (cmd == "SYMMETRIC_DIFFERENCE") {
        if (tokens.size() != 1) {
            return ParseError{"SYMMETRIC_DIFFERENCE expects no arguments"};
        }
        return Command{CommandType::SymmetricDifference, {}, {}, {}};
    }

    return ParseError{"unknown command " + std::string(cmd)};
}

} // namespace join_server