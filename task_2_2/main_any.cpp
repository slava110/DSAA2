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
#include <sstream>
#include <map>

#include "hashmap.hpp"
#include "simplehash.hpp"

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
            if(cmdPos != commands.end()) {
                cmdPos->second(splitStream);
            } else {
                std::cout << "Command `" << command << "` not found! Use `help` to see list of available commands" << std::endl;
            }
        }
    }

    delete myMap;
}