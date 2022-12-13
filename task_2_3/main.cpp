// Вариант 3 (18)
#include <iostream>
#include <string>
#include <thread> // For Sleep function
#include <chrono>
#include "./../common/colors.hpp"

/**
 * @brief Находит самое длинное слово в предложении с одинаковыми буквами на концах
 * @note Если есть 2 самых длинных слова - выбирает первое.
 * @param text
 * @return индекс слова или -1 если слово не найдено
 */
int findLongestWordWithSameLettersOnEnds(const std::string& text) {
    int maxPos = -1;
    int maxLength = 0;

    int wordStart = 0;
    for(int i = 1; i < text.length() + 1; i++) { //TODO end of string fix //TODO make better TODOs idk what's this about
        if(text.length() < i || text[i] == ' ') {
            if(text[i - 1] == text[wordStart]) {
                int wordLength = i - 1 - wordStart;
                if(wordLength > maxLength) {
                    maxPos = wordStart;
                    maxLength = wordLength;
                }
            }
            wordStart = i++;
        }
    }

    return maxPos;
}


//region fancyLongestWord

void printLongestWordFormatted(
        const std::string& text,
        int maxStart,
        int maxLength,
        int curPos = -1,
        int wordStart = -1,
        int match = -1,
        bool nextLine = false
) {
    int lastSymbol = 0;

    if(maxLength > 0) {
        if(maxStart > 0)
            std::cout << dye::grey(text.substr(lastSymbol, maxStart));

        std::cout << dye::on_green(text.substr(maxStart, maxLength + 1));
        lastSymbol = maxStart + maxLength + 1;
    }

    // Вывести символ, с которым сравниваем
    if(wordStart != -1) {
        if(wordStart > 0)
            std::cout << dye::grey(text.substr(lastSymbol, wordStart - lastSymbol));

        std::cout << (match == 0
            ? dye::on_red(text[wordStart])
            : (match == 1
                ? dye::on_green(text[wordStart])
                : dye::on_yellow(text[wordStart])
            )
        );
        lastSymbol = wordStart + 1;
    }

    // Вывести символ, который сравниваем
    if(curPos != -1 && curPos < text.length()) {
        if(curPos > 0)
            std::cout << dye::grey(text.substr(lastSymbol, curPos - lastSymbol));
        std::cout << (
            (wordStart != -1)
                ? (match == 0
                    ? dye::on_red(text[curPos])
                    : (match == 1
                        ? dye::on_green(text[curPos])
                        : dye::on_yellow(text[curPos])
                    )
                )
                : dye::on_grey(text[curPos]
            )
        );
        lastSymbol = curPos + 1;
    }

    if(curPos != text.length())
        std::cout << text.substr(lastSymbol);
    else
        std::cout << dye::grey(text.substr(lastSymbol));

    std::cout << "     ";
    if(nextLine)
        std::cout << std::endl;
    else
        std::cout << "\r";
}

/**
 * @brief Находит самое длинное слово в предложении с одинаковыми буквами на концах
 * @note Если есть 2 самых длинных слова - выбирает первое.
 * @param text текст, в котором нужно найти это слово
 * @return индекс слова или -1 если слово не найдено
 */
int findLongestWordWithSameLettersOnEndsButFancy(const std::string& text, int animationDelay = 1000) {
    std::string preprocessedText = text + ' ';

    int maxPos = -1;
    int maxLength = 0;

    int wordStart = 0;

    int comparisonsSpace = 0;
    int comparisonsSymbol = 0;
    int comparisonsMax = 0;

    // Изначальный текст
    printLongestWordFormatted(preprocessedText, maxPos, maxLength);
    std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));


    for(int i = 1; i < preprocessedText.length(); i++) {
        // Итерирование
        printLongestWordFormatted(preprocessedText, maxPos, maxLength, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

        comparisonsSpace++;

        if(preprocessedText[i] == ' ') {
            // Сравнение
            printLongestWordFormatted(preprocessedText, maxPos, maxLength, i - 1, wordStart);
            std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

            comparisonsSymbol++;

            if(preprocessedText[i - 1] == preprocessedText[wordStart]) {
                // Сравнение успешно
                printLongestWordFormatted(preprocessedText, maxPos, maxLength, i - 1, wordStart, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

                comparisonsMax++;

                int wordLength = i - 1 - wordStart;
                if(wordLength > maxLength) {
                    maxPos = wordStart;
                    maxLength = wordLength;
                }
            } else {
                // Сравнение неуспешно
                printLongestWordFormatted(preprocessedText, maxPos, maxLength, i - 1, wordStart, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));
            }
            wordStart = ++i;
        }
    }

    // Финальная версия
    printLongestWordFormatted(preprocessedText, maxPos, maxLength, preprocessedText.length(), -1, -1, true);
    std::cout << "Text length: " << text.length() << std::endl;
    std::cout << "Space comparisons: " << comparisonsSpace << std::endl;
    std::cout << "Surroundings comparisons: " << comparisonsSymbol << std::endl;
    std::cout << "Maximum value comparisons: " << comparisonsMax << std::endl;
    std::cout << "Total comparisons: " << (comparisonsSpace + comparisonsSymbol + comparisonsMax) << std::endl;

    return maxPos;
}

//endregion

/**
 * Префикс-функция (KMP)
 * @param pattern шаблон для поиска
 * @param lps результат
 */
void generateLPSForKMP(const std::string& pattern, int* lps) {
    lps[0] = 0;

    int i = 1;
    int j = 0;

    while(i < pattern.length()) {
        if (pattern[i] == pattern[j]) {
            lps[i] = ++j;
            i++;
        } else {
            if (j == 0) {
                lps[i] = 0;
                i++;
            } else {
                j = lps[j - 1];
            }
        }
    }
}

/**
 * @brief Поиск последнего вхождения образца в текст. Реализация алгоритма Кнута-Мориса-Пратта
 * @return индекс последнего вхождения шаблона в строку
 */
int findPatternLastKMP(const std::string& text, const std::string& pattern) {
    int res = -1;

    // Префикс-функция
    int* lps = new int[pattern.length()];
    generateLPSForKMP(pattern, lps);

    int i = 0;
    int matchLength = 0;

    // Итерация пока есть место, чтобы уместить шаблон
    while ( (text.length() - i) >= (pattern.length() - matchLength) ) {
        if (pattern[matchLength] == text[i]) {
            matchLength++;
            i++;
        }

        if (matchLength == pattern.length()) {
            res = i - matchLength;
            matchLength = lps[matchLength - 1];
        } else if (i < text.length() && pattern[matchLength] != text[i]) {
            if (matchLength != 0) {
                matchLength = lps[matchLength - 1];
            } else {
                i = i + 1;
            }
        }
    }

    return res;
}

/*void printLSPGenerationFormatted(
    const std::string& pattern,
    int curPos = -1,
    int comparisonPos = -1,
    int match = -1,
    bool nextLine = false
) {
    int lastSymbol = 0;

    // Вывести символ, с которым сравниваем
    if(comparisonPos != -1) {
        if(comparisonPos > 0)
            std::cout << dye::grey(pattern.substr(lastSymbol, comparisonPos - lastSymbol));

        std::cout << (match == 0
                      ? dye::on_red(pattern[comparisonPos])
                      : (match == 1
                         ? dye::on_green(pattern[comparisonPos])
                         : dye::on_yellow(pattern[comparisonPos])
                      )
        );
        lastSymbol = comparisonPos + 1;
    }

    // Вывести символ, который сравниваем
    if(curPos != -1 && curPos < pattern.length()) {
        if(curPos > 0)
            std::cout << dye::grey(pattern.substr(lastSymbol, curPos - lastSymbol));
        std::cout << (
                (comparisonPos != -1)
                ? (match == 0
                   ? dye::on_red(pattern[curPos])
                   : (match == 1
                      ? dye::on_green(pattern[curPos])
                      : dye::on_purple(pattern[curPos])
                   )
                )
                : dye::on_grey(pattern[curPos]
                )
        );
        lastSymbol = curPos + 1;
    }

    if(curPos != pattern.length())
        std::cout << pattern.substr(lastSymbol);
    else
        std::cout << dye::grey(pattern.substr(lastSymbol));

    std::cout << "          ";
    if(nextLine)
        std::cout << std::endl;
    else
        std::cout << "\r";
}

void generateLPSForKMPFancy(const std::string& pattern, int* lps, int animationDelay) {
    std::cout << "Generating LSP..." << std::endl;
    std::cout << dye::on_purple("Purple") << " - iterator, " << dye::on_yellow("Yellow");
    std::cout << " - comparison symbol, " << dye::on_green("Green") << " - comparison success";
    std::cout << std::endl << std::endl;

    lps[0] = 0;

    int i = 1;
    int j = 0;

    printLSPGenerationFormatted(pattern);
    std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

    while(i < pattern.length()) {
        printLSPGenerationFormatted(pattern, i, j);
        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

        if (pattern[i] == pattern[j]) {
            printLSPGenerationFormatted(pattern, i, j, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

            lps[i] = ++j;
            i++;
        } else {
            printLSPGenerationFormatted(pattern, i, j, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

            if (j == 0) {
                lps[i] = 0;
                i++;
            } else {
                j = lps[j - 1];
            }
        }
    }

    printLSPGenerationFormatted(pattern, pattern.length(), -1, -1, true);

    std::cout << "LPS: ";
    for(int k = 0; k < pattern.length(); k++) {
        std::cout << lps[k];
    }
    std::cout << std::endl;
}

void printPatternLastKMPFormatted(
    const std::string& text,
    const std::string& pattern,
    int curPos = -1,
    int matchLength = 0,
    int match = -1,
    bool nextLine = false
) {
    int lastSymbol = 0;

    // Вывести символ, с которым сравниваем
    if(comparisonPos != -1) {
        if(comparisonPos > 0)
            std::cout << dye::grey(pattern.substr(lastSymbol, comparisonPos - lastSymbol));

        std::cout << (match == 0
                      ? dye::on_red(pattern[comparisonPos])
                      : (match == 1
                         ? dye::on_green(pattern[comparisonPos])
                         : dye::on_yellow(pattern[comparisonPos])
                      )
        );
        lastSymbol = comparisonPos + 1;
    }

    // Вывести символ, который сравниваем
    if(curPos != -1 && curPos < pattern.length()) {
        if(curPos > 0)
            std::cout << dye::grey(pattern.substr(lastSymbol, curPos - lastSymbol));
        std::cout << (
                (comparisonPos != -1)
                ? (match == 0
                   ? dye::on_red(pattern[curPos])
                   : (match == 1
                      ? dye::on_green(pattern[curPos])
                      : dye::on_purple(pattern[curPos])
                   )
                )
                : dye::on_grey(pattern[curPos]
                )
        );
        lastSymbol = curPos + 1;
    }

    if(curPos != pattern.length())
        std::cout << pattern.substr(lastSymbol);
    else
        std::cout << dye::grey(pattern.substr(lastSymbol));

    std::cout << "          ";
    if(nextLine)
        std::cout << std::endl;
    else
        std::cout << "\r";
}

int findPatternLastKMPFancy(const std::string& text, const std::string& pattern, int animationDelay = 1000) {
    int res = -1;

    // Префикс-функция
    int* lps = new int[pattern.length()];
    generateLPSForKMPFancy(pattern, lps, animationDelay);

    int i = 0;
    int matchLength = 0;

    // Итерация пока есть место, чтобы уместить шаблон
    while ( (text.length() - i) >= (pattern.length() - matchLength) ) {

        printPatternLastKMPFormatted(text, pattern, i, matchLength);
        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

        if (pattern[matchLength] == text[i]) {
            matchLength++;
            i++;
        }

        printPatternLastKMPFormatted(text, pattern, i, matchLength);
        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

        if (matchLength == pattern.length()) {
            printPatternLastKMPFormatted(text, pattern, i, matchLength, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

            res = i - matchLength;
            matchLength = lps[matchLength - 1];
        } else if (i < text.length() && pattern[matchLength] != text[i]) {
            printPatternLastKMPFormatted(text, pattern, i, matchLength, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));

            if (matchLength != 0) {
                matchLength = lps[matchLength - 1];
            } else {
                i = i + 1;
            }
        }
    }

    printPatternLastKMPFormatted(text, pattern, -1, 0, -1, false);

    return res;
}*/

int main() {
    findLongestWordWithSameLettersOnEndsButFancy(
            "Is text yaaay wow yey",
            350
    );

    /*int res = findPatternLastKMP(
        "abcxabcdabxaabcdabcabcdabcdabcy",
        "abcdabca"
    );*/

    //std::cout << "Pos: " << res << std::endl;
    system("pause");
}