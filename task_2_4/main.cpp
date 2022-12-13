// Вариант 18
// Симметричное вещественное АВЛ-дерево с балансировкой
// Симметричный обход, найти сумму значений листьев, найти среднее арифм. всех узлов

//Провести полное тестирование программы на дереве размером n=10
//элементов, сформированном вводом с клавиатуры. Тест-примеры определить
//самостоятельно. Результаты тестирования в виде скриншотов экранов
//включить в отчет по выполненной работе.

#include <iostream>
#include <functional>

class MyAVLTree {

    /**
     * Элемент дерева
     */
    struct Node {
        Node* childLeft;
        Node* childRight;
        int height = 0;
        double data;

        Node(Node* child1, Node* child2, double data): childLeft(child1), childRight(child2), data(data) {}

        ~Node() {
            delete childLeft;
            delete childRight;
        }

        double getData() const {
            return data;
        }

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "misc-no-recursion"

        /**
         * Паттерн Visitor. Посещение всех элементов дерева симметричным обходом
         * @param visitorFunc функция-посетитель
         * @return true если стоит продолжить посещение, false если стоит прекратить
         */
        bool visit(const std::function<bool(double)>& visitorFunc) const {
            if(childLeft != nullptr && !childLeft->visit(visitorFunc)) {
                return false;
            }

            if(!visitorFunc(data))
                return false;

            if(childRight != nullptr && !childRight->visit(visitorFunc)) {
                return false;
            }

            return true;
        }
        #pragma clang diagnostic pop
    };

    Node* root;

public:

    MyAVLTree() {
        root = nullptr;
    }

    explicit MyAVLTree(double rootValue) {
        root = new Node(nullptr, nullptr, rootValue);
    }

    ~MyAVLTree() {
        // Despacito, play Fallen Kingdom
        delete root;
    }

    /**
     * Вставить новый элемент в AVL дерево
     * @param data элемент
     */
    void insert(double data) {
        insertInto(root, data);
    }

    /**
     * Паттерн Visitor. Посещает все элементы дерева симметричным обходом пока функция возвращает true
     * @param visitorFunc функция-посетитель. Должна возвращать true если стоит продолжать обход
     */
    void visit(const std::function<bool(double)>& visitorFunc) {
        if(root != nullptr) {
            root->visit(visitorFunc);
        }
    }

    /**
     * Получение суммы элементов
     * @return сумма всех элементов дерева
     */
    double sum() {
        double sum = 0;
        visit([&sum](double data){
            sum += data;
            return true;
        });
        return sum;
    }

    /**
     * Получение количество элементов
     * @return количество элементов дерева
     */
    int size() {
        int amount = 0;
        visit([&amount](double data){
            amount += 1;
            return true;
        });
        return amount;
    }

    /**
     * Получение среднего значения из элементов дерева
     * @return среднее значение
     */
    double average() {
        double sum = 0;
        int amount = 0;
        visit([&sum, &amount](double data){
            sum += data;
            amount += 1;
            return true;
        });
        return sum / amount;
    }

    //region utils

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "misc-no-recursion"

    /**
     * Функция вставки элемента в узел дерева
     * @param node узел дерева
     * @param data элемент
     * @return узел, в который был вставлен элемент
     */
    static Node* insertInto(Node* node, double data) {
        if(node == nullptr) {
            node = new Node(nullptr, nullptr, data);
        } else if(data < node->data) {
            node->childLeft = insertInto(node->childLeft, data);
            balance(node);
        } else { // data > node->data
            node->childRight = insertInto(node->childRight, data);
            balance(node);
        }

        return node;
    }
    #pragma clang diagnostic pop

    /**
     * Безопасное получение высоты дерева с корнем node
     * @param node корень дерева
     * @return высота дерева
     */
    static unsigned char heightSafe(Node* node) {
        return node ? node->height : 0;
    }

    /**
     * Фактор балансировки дерева
     * @param node корень дерева
     * @return фактор балансировки
     */
    static int bfactor(Node* node) {
        return heightSafe(node->childRight) - heightSafe(node->childLeft);
    }

    /**
     * Обновление высоты дерева относительно деревьев-детей
     * @param node корень дерева
     */
    static void fixHeight(Node* node) {
        unsigned char leftHeight = heightSafe(node->childLeft);
        unsigned char rightHeight = heightSafe(node->childRight);
        node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }

    /**
     * Одинарный поворот дерева влево
     * @param node корень дерева
     * @return повёрнутое дерево (на всю голову)
     */
    static Node* rotateLeft(Node* node) {
        Node* t = node->childRight;
        node->childRight = t->childLeft;
        t->childLeft = node;
        fixHeight(node);
        fixHeight(t);
        return node;
    }

    /**
     * Одинарный поворот дерева вправо
     * @param node корень дерева
     * @return повёрнутое дерево (на всю голову)
     */
    static Node* rotateRight(Node* node) {
        Node* t = node->childLeft;
        node->childLeft = t->childRight;
        t->childRight = node;
        fixHeight(node);
        fixHeight(t);
        return node;
    }

    /**
     * Балансировка дерева с корнем node
     * @param node корень дерева
     * @return корень дерева
     */
    static Node* balance(Node* node) {
        fixHeight(node);
        int bFact = bfactor(node);
        if(bFact > 1) {
            if(bfactor(node->childRight) < 0)
                node->childRight = rotateRight(node->childRight);
            return rotateLeft(node);
        } else if(bFact < 1) {
            if(bfactor(node->childLeft) > 0)
                node->childLeft = rotateLeft(node->childLeft);
            return rotateRight(node);
        }
        return node;
    }

    //endregion
};

int main() {

}