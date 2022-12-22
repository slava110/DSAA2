#pragma once

/**
 * Хэш-функция
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
template <typename K>
using HashFunction = size_t(const K& key);

template <typename K, typename V, HashFunction<K> HASHER>
class MyMap {
    struct MyMapEntry;

    MyMapEntry** entries;
    // Чтобы после выполнения delete на коллизиях не возникало "дыр"
    MyMapEntry* placeholder = new MyMapEntry(nullptr, nullptr);

    int size = 0;
    int capacity = 16;
    // Константа, на которую смещается адрес в случае коллизии
    const int collisionShift = 1;
    const double defaultLoadFactor = 0.75;

    /**
     * @brief Вычисление хэш-суммы ключа
     * @param key ключ
     * @return хэш-сумма ключа или NAN если ключ NULL
     */
    size_t computeKeyHash(const K& key) {
        return HASHER(key) % capacity;
    }

    /**
     * @brief Удваивает размер и заново хеширует все ключи
     */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
    void rehash() {
        MyMapEntry** oldEntries = entries;

        int oldCapacity = capacity;
        capacity *= 2;
        size = 0;

        entries = new MyMapEntry*[capacity];

        for(int i = 0; i < oldCapacity; i++) {
            MyMapEntry* entry = oldEntries[i];
            if(entry != nullptr && !entry->isEmpty()) {
                put(*(entry->key), *(entry->value));
                delete entry;
            }
        }

        delete[] oldEntries;
    }
#pragma clang diagnostic pop

    struct MyMapEntry {
        const K* key;
        const V* value;

        MyMapEntry(const K* key, const V* value): key(key), value(value) {}

        ~MyMapEntry() {
            delete key;
            delete value;
        }

        bool isEmpty() {
            return key == nullptr;
        }
    };

public:

    explicit MyMap(int capacity = 16, double defaultLoadFactor = 0.75): size(0), capacity(capacity), defaultLoadFactor(defaultLoadFactor), entries(new MyMapEntry*[capacity]) {
        for(int i = 0; i < capacity; i++) {
            entries[i] = nullptr;
        }
    }

    ~MyMap() {
        clear();
        delete[] entries;
    }

    /**
     * @brief Получение количества пар ключ-значение
     * @return Количество пар ключ-значение
     */
    int getSize() {
        return size;
    }

    /**
     * @brief Проверка, пуст ли массив
     * @return true если пуст, false в другом случае
     */
    bool isEmpty() {
        return size == 0;
    }

    /**
     * @brief Получение ёмкости хэш-таблицы
     * @return количество возможных записей в хэш-таблице
     */
    int getCapacity() {
        return capacity;
    }

    /**
     * @brief Получение значения load factor
     * @return значение load factor
     */
    double getLoadFactor() {
        return (double) size / capacity;
    }

    /**
     * @brief Получение значения из таблицы по ключу
     * @param key ключ
     * @return значение по ключу или NULL если значение не найдено
     */
    bool get(const K& key, V& value) {
        int keyHash = computeKeyHash(key);

        for(int i = 0; i < size; i++) {
            if(entries[keyHash] != nullptr) {
                if(!entries[keyHash]->isEmpty() && *(entries[keyHash]->key) == key) {
                    value = *(entries[keyHash]->value);
                    return true;
                }
            } else {
                return false;
            }
            // Ограничиваем вместимостью
            keyHash = (keyHash + collisionShift) % capacity;
        }

        return false;
    }

    /**
     * @brief Проверка на наличие ключа в массиве
     * @param key ключ
     * @return true если ключ есть, false если нет
     */
    bool containsKey(const K& key) {
        int keyHash = computeKeyHash(key);

        for(int i = 0; i < size; i++) {
            if(entries[keyHash] != nullptr) {
                if(!entries[keyHash]->isEmpty() && *(entries[keyHash]->key) == key) {
                    return true;
                }
            } else {
                return false;
            }
            // Ограничиваем вместимостью
            keyHash = (keyHash + collisionShift) % capacity;
        }

        return false;
    }

    /**
     * @brief Помещение нового элемента в таблицу
     * @param key ключ
     * @param value значение
     * @return true если значение
     */
    bool put(const K& key, const V& value) {
        int keyHash = computeKeyHash(key);

        for(int i = 0; i < capacity; i++) {
            if(entries[keyHash] == nullptr || entries[keyHash]->isEmpty() || *(entries[keyHash]->key) == key) {
                std::cout << "Adding entry (" << keyHash << ") [" << key << "] = " << value << ";" << std::endl;

                // MEMORY LEAK POSSIBLE HERE
                // But won't happen c:
                // Я ЗНАЛ, Я ЗНАЛ, ЧТО ЛОКАЛЬНЫЕ ПЕРЕМЕННЫЕ
                // С УКАЗАТЕЛЯМИ ПЛОХО СОЧЕТАЮТСЯ!
                K* keyP = new K;
                *keyP = key;
                V* valueP = new V;
                *valueP = value;

                entries[keyHash] = new MyMapEntry(keyP, valueP);

                size++;

                if(getLoadFactor() >= defaultLoadFactor) {
                    std::cout << "Rehashing!" << std::endl;
                    rehash();
                    std::cout << "Rehashing done!" << std::endl;
                }

                return true;
            }
            // Ограничиваем вместимостью
            keyHash = (keyHash + collisionShift) % capacity;
        }

        return false;
    }

    /**
     * @brief Удаление записи из таблицы по ключу
     * @param key ключ записи
     * @return удалённое значение или NULL если значение не найдено
     */
    bool remove(const K& key) {
        int keyHash = computeKeyHash(key);

        for(int i = 0; i < size; i++) {
            if(entries[keyHash] != nullptr) {
                if(!entries[keyHash]->isEmpty() && *entries[keyHash]->key == key) {
                    delete entries[keyHash];
                    entries[keyHash] = placeholder;
                    size--;
                    return true;
                }
            } else {
                return false;
            }
            // Ограничиваем вместимостью
            keyHash = (keyHash + collisionShift) % capacity;
        }
        return false;
    }

    /**
     * @brief Полная очистка таблицы
     * @note Не изменяет размер таблицы
     */
    void clear() {
        for(int i = 0; i < capacity; i++) {
            if(entries[i] != nullptr) {
                delete entries[i];
                entries[i] = nullptr;
            }
        }
        size = 0;
    }

    /**
     * @brief Представление таблицы в виде строки
     * @return строковое представление таблицы
     */
    std::string toString() {
        std::stringstream ss;
        ss << "MyMap {\n";
        int stopper = 0;
        for(int i = 0; i < capacity && stopper < size; i++) {
            if(entries[i] != nullptr && !entries[i]->isEmpty()) {
                ss << "    `" << *(entries[i]->key) << "` = `" << *(entries[i]->value) << "`\n";
                stopper++;
            }
        }
        ss << "}";
        return ss.str();
    }

};