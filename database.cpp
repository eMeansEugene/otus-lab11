//
// Created by evgen on 12.09.2026.
//

#include "database.h"

#include <utility>

bool join_server::Database::Insert(Table t, int id, const std::string &name) {
    switch (t) {
        case Table::A: {
            const auto [fst, snd] = a_.insert(std::pair<int, std::string>(id, name));
            return snd;
        }
        case Table::B: {
            const auto [fst, snd] = b_.insert(std::pair<int, std::string>(id, name));
            return snd;
        }
        default:
            return false;
    }
}

void join_server::Database::Truncate(const Table t) {
    switch (t) {
        case Table::A: {
            a_.clear();
            break;
        }
        case Table::B: {
            b_.clear();
            break;
        }
        default: ;
    }
}

std::vector<std::string> join_server::Database::Intersection() const {
    std::vector<std::string> result;
    auto it_a = a_.begin();
    auto it_b = b_.begin();
    while (it_a != a_.end() && it_b != b_.end()) {
        if (it_a->first < it_b->first) {
            ++it_a;
        } else if (it_b->first < it_a->first) {
            ++it_b;
        } else {
            result.push_back(std::to_string(it_a->first) + ","
                             + it_a->second + "," + it_b->second);
            ++it_a;
            ++it_b;
        }
    }
    return result;
}


std::vector<std::string> join_server::Database::SymmetricDifference() const {
    std::vector<std::string> result;
    auto it_a = a_.begin();
    auto it_b = b_.begin();
    while (it_a != a_.end() && it_b != b_.end()) {
        if (it_a->first < it_b->first) {
            result.push_back(std::to_string(it_a->first) + "," + it_a->second + ",");
            ++it_a;
        } else if (it_b->first < it_a->first) {
            result.push_back(std::to_string(it_b->first) + ",," + it_b->second);
            ++it_b;
        } else {
            ++it_a;
            ++it_b;
        }
    }
    while (it_a != a_.end()) { result.push_back(std::to_string(it_a->first) + "," + it_a->second + ","); ++it_a; }
    while (it_b != b_.end()) { result.push_back(std::to_string(it_b->first) + ",," + it_b->second); ++it_b; }
    return result;
}
