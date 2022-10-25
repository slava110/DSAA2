#include <Windows.h>
#include <functional>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <iostream>
#include <bitset>

/**
 * @brief Вывод числа в двоичной и в десятичной формах
 * @tparam T тип числа
 * @param prefix префикс вывода
 * @param value число
 */
template <typename T>
void fancyNumberOutput(const std::string& prefix, int value) {
    const int xSize = sizeof(T) * 8;
    std::cout << prefix << " bin: " << std::bitset<xSize>(value) << std::endl;
    std::cout << prefix << " dec: " << (unsigned int) value << std::endl;
}

void subtask_1_a() {
    unsigned char x = 255;   // 8-разрядное двоичное число 11111111
    unsigned char maska = 1; // 1 = 00000001 – 8-разрядная маска
    x = x & (~(maska << 4)); // Результат (x=239)
    fancyNumberOutput<unsigned char>("X", x);
}

void subtask_1_b() {
    unsigned char maska = 1;        // 1 = 00000001 – 8-разрядная маска
    unsigned char x = (maska << 6); // Результат (x=64)
    fancyNumberOutput<unsigned char>("X", x);
}

void subtask_1_c() {
    unsigned int x = 25;
    const int n = sizeof(int) * 8; //=32 - количество разрядов в числе типа int
    unsigned maska = (1 << (n - 1)); //1 в старшем бите 32-разрядной сетки
    std::cout << "Начальный вид маски: " << std::bitset<n> (maska) << std::endl;
    std::cout << "Результат: ";
    for (int i = 1; i <= n; i++) { //32 раза - по количеству разрядов:
        std::cout << ((x & maska) >> (n - i));
        maska = maska >> 1; //смещение 1 в маске на разряд вправо
    }
    std::cout << std::endl;
}

/**
 * @brief Сортирока введённых чисел при помощи битового массива
 * @note Вводить числа следует через пробел. Нажатие Enter завершает ввод
 * @tparam SIZE размер битового массива
 * @param arrContains функция для проверки на наличие числа в битовом массиве
 * @param arrAppender функция для добавления числа в битовый массив
 * @param arrBitsetGetter функция для получения bitset репрезентации битового массива
 * @author Святослав Попов
 */
template <std::size_t SIZE>
void binarySortTask(
        const std::function<bool(unsigned int)>& arrContains,
        const std::function<void(unsigned int)>& arrAppender,
        const std::function<std::bitset<SIZE>()>& arrBitsetGetter
) {
    for(int i = 0; i < SIZE && std::cin.peek() != '\n'; i++) { // Ввод пока не закончится место в массиве и пока не встретится Enter
        unsigned int num = 0;
        std::cin >> num;
        if(num >= SIZE || num < 0) {
            std::cout << "Обнаружено число, выходящее за рамки [0.." << SIZE << "): " << num;
            std::cout << ". Оно будет проигнорировано" << std::endl;
            continue;
        }
        arrAppender(num);
    }

    std::cout << "Битовый массив: " << arrBitsetGetter() << std::endl; // Целая лямбда для красивого отображения в консоли

    std::cout << "Отсортированные числа: ";
    for(int i = 0; i < SIZE; i++) {
        if(arrContains(i)) {
            std::cout << i;
            std::cout << " "; //Trailing space, но не принципиально
        }
    }
    std::cout << std::endl;
}

/**
 * @brief Сортирока введённых чисел при помощи битового массива
 * @tparam T тип битового массива (например: `unsigned char`)
 * @author Святослав Попов
 */
template <typename T>
void binarySortTask() {
    T binaryArray = 0;

    binarySortTask<sizeof(T) * 8>(
            [&binaryArray](unsigned int i){
                return binaryArray & (((T) 1) << i); // Проверяем, есть ли в битовом массиве число
            },
            [&binaryArray](unsigned int num){
                binaryArray = binaryArray | (((T) 1) << num); // Добавляем в битовый массив число, меняя нужный бит на 1
            },
            [&binaryArray] {
                return std::bitset<sizeof(T) * 8>(binaryArray); // bitset для красивого вывода в консоли
            }
    );
}

void subtask_2_a() {
    binarySortTask<unsigned char>();
}

void subtask_2_b() {
    binarySortTask<unsigned long long>();
}

void subtask_2_c() {
    // Массив чисел. По сути один большой битовый массив в памяти. ББМ. Так и назовём
    unsigned char binaryArray[8];

    // В C++ массивы по умолчанию не заполнены 0, надо заполнять :c
    memset(binaryArray, 0, sizeof(binaryArray));

    binarySortTask<64>(
            [&binaryArray](unsigned int i){
                return binaryArray[i / 8] & (1 << (i % 8)); // Проверяем, есть ли в ББМ число
            },
            [&binaryArray](unsigned int num){
                binaryArray[num / 8] = binaryArray[num / 8] | (1 << (num % 8)); // Добавляем число в ББМ
            },
            [&binaryArray] {
                // Очень сложная функция, которая в один большой bitset отправляет числа из ББМ
                // Можно было использовать побитовые операции, но на StackOverflow сказали, что лучше не надо
                // Там умные люди, я им доверяю
                std::bitset<64> bs;
                int pos = 0;
                for(unsigned char subArr : binaryArray) {
                    for(int b = 0; b < 8; ++b) {
                        bs.set(pos++, subArr >> b & 1);
                    }
                }
                return bs;
            }
    );
}

/**
 * @brief Генерация файла numbers.txt, заполненного не более чем 10^7 положит. числами, каждое в диапазоне
 * [1000000...9999999] и уникально
 * @author Святослав Попов
 */
void subtask_3_generate() {
    std::vector<unsigned int> v(9000000);

    // Хитрый способ получения уникальных чисел в массиве
    for (unsigned int i = 0; i < 9000000; ++i)
        v[i] = i + 1000000;

    // Уникальные случайные числа после каждого запуска
    std::random_device rd;
    std::mt19937 randGenerator(rd());

    // Перемешиваем массив
    std::shuffle(v.begin(), v.end(), randGenerator);

    std::ofstream generated("numbers.txt");

    // Вывод массива в файл
    copy(v.begin(), v.end(), std::ostream_iterator<int>(generated, " "));

    generated.close();

    std::cout << "Файл сгенерирован" << std::endl;
}

/**
 * @brief Получение количество занятых вектором байт в памяти
 * @tparam T тип вектора
 * @param vec вектор
 * @return количество байт, занятых вектором в памяти
 * @author Святослав Попов
 */
template<typename T>
size_t getVectorUsedMemory(const std::vector<T>& vec) {
    size_t size_of_vector_struct = sizeof(std::vector<T>); // Размер структуры данных
    size_t size_of_single_element = sizeof(T); // Размер элемента
    return size_of_vector_struct + size_of_single_element * vec.size(); // Магия математики
}

void subtask_3() {
    auto subtaskBeginTime = std::chrono::system_clock::now(); // Засекаем время

    std::ifstream numbersFile("numbers.txt");

    if(!numbersFile) {
        std::cout << "Файл с числами не найден!" << std::endl; // На всякий случай
        return;
    }

    std::vector<unsigned char> numbers(1250000); // 10000000 / 8, ибо каждый элемент хранит 8 чисел

    std::fill(numbers.begin(), numbers.end(), 0); // Заполняем 0, чтобы работали побитовые операции

    // Чтение из файла в битовый массив
    unsigned int num;
    while(numbersFile >> num) {
        numbers[num / 8] = numbers[num / 8] | (1 << (num % 8));
    }

    numbersFile.close();

    std::ofstream sortedNumbersFile("sorted_numbers.txt");

    // Вывод отсортированного массива в файл
    for(unsigned int i = 1000000; i < 10000000; i++) {
        if(numbers[i / 8] & (1 << (i % 8))) {
            sortedNumbersFile << i << " ";
        }
    }

    sortedNumbersFile.close();

    auto subtaskEndTime = std::chrono::system_clock::now();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>((subtaskEndTime - subtaskBeginTime)).count();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(subtaskEndTime - subtaskBeginTime).count();

    std::cout << "Время выполнения: " << milliseconds << "мс (" << seconds << "с)" << std::endl;
    std::cout << "Размер битового массива: " << getVectorUsedMemory(numbers) << "Б" << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    subtask_3();
}