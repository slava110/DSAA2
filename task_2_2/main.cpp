// Разработайте приложение, которое использует хеш-таблицу (пары «ключ – хеш») для
// организации прямого доступа к элементам динамического множества полезных данных.

// Множество реализуйте на массиве, структура элементов (перечень полей) которого приведена в
// индивидуальном варианте (п.3).

// Приложение должно содержать класс с базовыми операциями: вставки, удаления, поис-
// ка по ключу, вывода. Включите в класс массив полезных данных и хеш-таблицу. Хеш-функцию
// подберите самостоятельно, используя правила выбора функции.

// Реализуйте расширение размера таблицы и рехеширование, когда это требуется, в соот-
// ветствии с типом разрешения коллизий.

// Предусмотрите автоматическое заполнение таблицы 5-7 записями.

// Реализуйте текстовый командный интерфейс пользователя для возможности вызова ме-
// тодов в любой произвольной последовательности, сопроводите вывод достаточными для
// понимания происходящего сторонним пользователем подсказками.

// Проведите полное тестирование программы (все базовые операции, изменение размера и
// рехеширование), тест-примеры определите самостоятельно. Результаты тестирования включите
// в отчет по выполненной работе.

#include <iostream>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <map>

/**
 * Хэш-функция
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
template <typename K>
using HashFunction = size_t(const K& key);

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

template <typename K, typename V, HashFunction<K> HASHER = simpleHasher>
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
                // Тут выкидывается Memory Access Violation
                // Ссылка удаляется вместе с контекстом функции и не сохраняется в указатель?

                ss << "    `" << *(entries[i]->key) << "` = `" << *(entries[i]->value) << "`\n";
                stopper++;
            }
        }
        ss << "}";
        return ss.str();
    }

};

class Product {
    std::string name;
    unsigned long price;
};

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
 * Реализация хэш-функции для строки
 * @tparam K тип ключа хэш-таблицы
 * @param key ключ хэш-таблицы
 * @return хэш ключа
 */
size_t ulongHash(const unsigned long key) {
    return key;
}

std::map<std::string, std::function<void(std::istringstream&)>> initCommands(
        MyMap<std::basic_string<char>, std::basic_string<char>, strHash>* myMap
) {
    return {
            {"put", [myMap](std::istringstream& args){
                std::string key, value;
                args >> key >> std::ws >> value;
                if(myMap->put(key, value)) {
                    std::cout << "New entry in map: {" << key << ", " << value << "}" << std::endl;
                } else {
                    std::cout << "Unable to put new entry!" << std::endl;
                }
            }},
            {"get", [myMap](std::istringstream& args){
                std::string key;
                std::string value = "none";
                args >> key;
                bool res = myMap->get(key, value);
                std::cout << "[" << res << "] Value associated with key " << key << ": " << value << std::endl;
            }},
            {"contains", [myMap](std::istringstream& args){
                std::string key;
                args >> key;
                if(myMap->containsKey(key)) {
                    std::cout << "Key " << key << " found" << std::endl;
                } else {
                    std::cout << "Key " << key << " not found" << std::endl;
                }
            }},
            {"remove", [myMap](std::istringstream& args){
                std::string key;
                args >> key;
                if(myMap->remove(key)) {
                    std::cout << "Key " << key << " removed" << std::endl;
                } else {
                    std::cout << "Key " << key << " not found" << std::endl;
                }
            }},
            {"display", [myMap](std::istringstream& args){
                std::cout << myMap->toString() << std::endl;
            }},{"size", [myMap](std::istringstream& args){
                std::cout << "Size: " << myMap->getSize() << std::endl;
            }},
            {"clear", [myMap](std::istringstream& args){
                myMap->clear();
                std::cout << "Map cleared" << std::endl;
            }},
            {"help", [](std::istringstream& args){
                std::cout << "Available commands:" << std::endl;
                std::cout << "- put <key> <value> - put value into map and associate it with key" << std::endl;
                std::cout << "- get <key> - get value associated with this key from map" << std::endl;
                std::cout << "- contains <key> - check if map contains value associated with this key" << std::endl;
                std::cout << "- remove <key> - remove value associated with this key from map" << std::endl;
                std::cout << "- display - see stored values in map" << std::endl;
                std::cout << "- clear - clear map" << std::endl;
            }}
    };
}

int main() {
    auto myMap = new MyMap<unsigned long, std::string, ulongHash>();

    myMap->put("", "");

    std::map<std::string, std::function<void(std::istringstream&)>> commands = initCommands(myMap);

    std::cout << "Map interface by slava_110" << std::endl;
    std::cout << "Enter `help` to see list of commands" << std::endl;

    std::string line;

    while(getline(std::cin, line)) {
        std::cout << ">";
        std::istringstream splitStream(line);
        std::string command;
        if(splitStream >> command >> std::ws) {
            if(command == "quit")
                break;

            auto cmdPos = commands.find(command);
            if(cmdPos != commands.end()) {
                cmdPos->second(splitStream);
            } else {
                std::cout << "Command `" << command << "` not found! Use `help` to see list of available commands" << std::endl;
            }
        }
    }

    delete myMap;
}