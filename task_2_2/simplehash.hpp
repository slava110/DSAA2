#pragma once

#include "hashmap.hpp"

/**
 * Простейшая реализация хэш-функции
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
template <typename K>
size_t simpleHasher(const K& key) {
    return std::hash<K>(key);
}

/**
 * Реализация хэш-функции для строки
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
template <typename K>
size_t strHash(const K& key) {
    size_t h = 37;
    for (const char& c : key) {
        h = (h * 54059) ^ (c * 76963);
    }
    return h;
}

/**
 * Реализация хэш-функции для числа
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
template <typename T>
size_t ulongHash(T key) {
    return key;
}