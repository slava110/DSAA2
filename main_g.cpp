#include <SFML/Graphics.hpp>
#include <thread>

void mainRender(sf::RenderWindow* window) {
    window->setActive(true);
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);

    while(window->isOpen()) {
        window->clear();

        sf::CircleShape shape(50.f);
        shape.setFillColor(sf::Color(100, 250, 50));
        shape.setPosition(250, 400);

        window->draw(shape);

        window->display();
    }
}

int main() {
    auto window = sf::RenderWindow{ { 860u, 640u }, "Data Structures and Algorithms" };

    /*auto image = sf::Image{};
    if (!image.loadFromFile("icon.png")) {
        return 2; // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
    }*/

    //window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

    window.setActive(false);

    std::thread renderingThread(mainRender, &window);

    while (window.isOpen()) {
        for (auto event = sf::Event{}; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}