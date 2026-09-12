//
// Created by evgen on 12.09.2026.
//

#ifndef JOIN_SERVER_DATABASE_H
#define JOIN_SERVER_DATABASE_H

#include <map>
#include <string>
#include <vector>

namespace join_server {

    /// Селектор таблицы. Распознавание буквы A/B — забота парсера;
    /// домен получает уже валидный enum и не занимается синтаксисом.
    enum class Table { A, B };

    /**
     * @brief Доменное ядро: две таблицы {id -> name} и set-операции над ними.
     *
     * Хранит две таблицы одинаковой структуры (A и B), где id — первичный ключ
     * (дубли в пределах таблицы запрещены). О сети и протоколе (OK/ERR/\n)
     * не знает ничего: возвращает голые данные, обрамление добавляет транспорт.
     */
    class Database {
    public:
        /**
         * @brief Вставляет строку в таблицу.
         * @param t    Целевая таблица (A или B).
         * @param id   Первичный ключ строки.
         * @param name Значение строки.
         * @return true при успехе; false, если id уже присутствует (дубль).
         */
        bool Insert(Table t, int id, const std::string& name);

        /**
         * @brief Полностью очищает таблицу.
         * @param t Целевая таблица (A или B).
         */
        void Truncate(Table t);

        /**
         * @brief Пересечение таблиц по общим id.
         * @return Строки формата "id,nameA,nameB" в порядке возрастания id
         *         (без завершающего \n и без OK).
         */
        std::vector<std::string> Intersection() const;

        /**
         * @brief Симметрическая разность: id, встречающиеся только в одной таблице.
         * @return Строки формата "id,nameA," (только в A) либо "id,,nameB"
         *         (только в B), в порядке возрастания id.
         */
        std::vector<std::string> SymmetricDifference() const;

    private:
        std::map<int, std::string> a_;  ///< Таблица A: id -> name, отсортирована по id.
        std::map<int, std::string> b_;  ///< Таблица B: id -> name, отсортирована по id.
    };

} // namespace join_server

#endif //JOIN_SERVER_DATABASE_H