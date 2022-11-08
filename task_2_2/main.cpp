#include <iostream>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <map>

template <typename K>
using HashFunction = size_t(const K& key);

template <typename K>
size_t simpleHasher(const K& key) {
    return reinterpret_cast<unsigned long>(key);
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
     * @brief Поиск пары по заданному ключу
     * @param key ключ для поиска
     * @return адрес записи или NAN
     */
    int findEntry(const K& key) {
        return findNextMatchingEntry(
                key,
                [key](MyMapEntry* found){
                    return *(found->key) == key;
                },
                [key](MyMapEntry* found){
                    return found == nullptr || found->isEmpty();
                }
        );
    }

    /**
     * @brief Поиск пустого места для записи
     * @param key ключ записи
     * @return адрес пустого места или NAN
     */
    int findFreeHashForEntry(const K& key) {
        return findNextMatchingEntry(
                key,
                [key](MyMapEntry* found){
                    return found == nullptr || *(found->key) == key || found->isEmpty();
                }
        );
    }

    /**
     * @brief Поиск пары по заданному предикату
     * @param key ключ, с которого следует начать поиск
     * @param predicate предикат для найденного ключа
     * @param stopCondition условие остановки поиска
     * @return адрес записи или NAN
     */
    int findNextMatchingEntry(const K& key, const std::function<bool(MyMapEntry*)>& predicate, const std::function<bool(MyMapEntry*)>& stopCondition = nullptr) {
        int keyHash = computeKeyHash(key);

        if(!std::isnan(keyHash)) {
            for(int i = 0; i < capacity; i++) {
                if(stopCondition != nullptr && stopCondition(entries[keyHash])) {
                    break;
                }
                if(predicate(entries[keyHash])) {
                    return keyHash;
                }
                // Ограничиваем вместимостью
                keyHash = (keyHash + collisionShift) % capacity;
            }
        }

        return NAN;
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

        entries = new MyMapEntry*[capacity];
        clear();

        for(int i = 0; i < oldCapacity; i++) {
            MyMapEntry* entry = oldEntries[i];
            if(entry != nullptr && !entry->isEmpty()) {
                put(*(entry->key), *(entry->value));
            }
        }

        delete[] oldEntries;
    }
    #pragma clang diagnostic pop

    struct MyMapEntry {
        const K* key;
        V* value;

        MyMapEntry(const K* key, V* value): key(key), value(value) {}

        bool isEmpty() {
            return key == nullptr;
        }
    };

public:

    MyMap(int capacity = 16, double defaultLoadFactor = 0.75): size(0), capacity(capacity), defaultLoadFactor(defaultLoadFactor), entries(new MyMapEntry*[capacity]) {
        clear();
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
        int entryHash = findEntry(key);

        if(std::isnan(entryHash))
            return false;

        value = *(entries[entryHash]->value);
        return true;
    }

    /**
     * @brief Проверка на наличие ключа в массиве
     * @param key ключ
     * @return true если ключ есть, false если нет
     */
    bool containsKey(const K& key) {
        return !std::isnan(findEntry(key));
    }

    /**
     * @brief Помещение нового элемента в таблицу
     * @param key ключ
     * @param value значение
     * @return true если значение
     */
    bool put(const K& key, V& value) {
        int nextFreeIndex = findFreeHashForEntry(key);

        if(std::isnan(nextFreeIndex)) {
            // Вы что, задали load factor 1.0? Да чтоб вас...
            rehash();
            nextFreeIndex = findFreeHashForEntry(key);
            if(std::isnan(nextFreeIndex)) {
                // Так, а это уже страшно...
                throw std::runtime_error("Unable to find free hash after MyMap rehash :I");
            }
        }

        if(entries[nextFreeIndex] == nullptr || entries[nextFreeIndex]->isEmpty())
            size++;

        entries[nextFreeIndex] = new MyMapEntry(&key, &value);

        // load factor больше максимального - увеличиваем размер!
        if(getLoadFactor() > defaultLoadFactor)
            rehash();

        return true;
    }

    /**
     * @brief Удаление записи из таблицы по ключу
     * @param key ключ записи
     * @return удалённое значение или NULL если значение не найдено
     */
    bool remove(const K& key) {
        int entryHash = findEntry(key);

        if(std::isnan(entryHash))
            return false;

        delete entries[entryHash];
        entries[entryHash] = placeholder;
        size--;
        return true;
    }

    /**
     * @brief Полная очистка таблицы
     * @note Не изменяет размер таблицы
     */
    void clear() {
        for(int i = 0; i < capacity; i++) {
            //delete entries[i];
            entries[i] = nullptr;
        }
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
            }
            stopper++;
        }
        ss << "}";
        return ss.str();
    }

};

template <typename K>
size_t strHash(const K& key) {
    size_t h = 37;
    for (const char& c : key) {
        h = (h * 54059) ^ (c * 76963);
    }
    return h;
}

std::map<std::string, std::function<void(std::istringstream &)>> initCommands(
        MyMap<std::basic_string<char>, std::basic_string<char>, strHash>* myMap
) {
    return {
            {"put", [myMap](std::istringstream& args){
                std::string key, value;
                args >> key >> value;
                myMap->put(key, value);
                std::cout << "New entry in map: {" << key << ", " << value << "}" << std::endl;
            }},
            {"get", [myMap](std::istringstream& args){
                std::string key;
                std::string value = "none";
                args >> key;
                myMap->get(key, value);
                std::cout << "Value associated with key " << key << ": " << value;
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
    auto myMap = new MyMap<std::string, std::string, strHash>();

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
            if(cmdPos != commands.end())
                cmdPos->second(splitStream);
            else {
                std::cout << "Command `" << command << "` not found! Use `help` to see list of available commands" << std::endl;
            }
        }
    }

    delete myMap;
}