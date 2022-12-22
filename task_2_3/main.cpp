// Вариант 3 (18)
#include <iostream>
#include <string>

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

int main() {



    //std::cout << "Pos: " << res << std::endl;
    system("pause");
}