#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#define GEOM_DEBUG 1;

#include "src/resource_manager.h"
#include "src/util.h"

int main() {
  auto resource_mng = std::make_shared<geom_2d::ResourceManager>();

  sf::RenderWindow window(sf::VideoMode(1000, 800), "Menu",
                          sf::Style::Fullscreen);

  // Never use both setVerticalSyncEnabled and setFramerateLimit at the same
  // time!
  // window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(24);

  sf::CircleShape shape(10.f);
  shape.setFillColor(sf::Color::Cyan);

  sf::Font& font = resource_mng->GetFont();

  // Initialize the pause message
  sf::Text pauseMessage;
  pauseMessage.setFont(font);
  pauseMessage.setCharacterSize(40);
  pauseMessage.setPosition(170.f, 150.f);
  pauseMessage.setFillColor(sf::Color::Magenta);
  pauseMessage.setString(L"Играть с компьютером\nВыход");
  int count = 0;

  auto t1 = sf::milliseconds(15);

  sf::Sprite sprite;
  auto& texture = resource_mng->GetRunningSprite();
  texture.setSmooth(true);
  sprite.setTexture(texture);
  sprite.setScale(sf::Vector2f(5.f, 5.f)); 


  std::vector<sf::IntRect> anim_coords;
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 7; ++col) {
      anim_coords.emplace_back(col * 75, row * 86, 75, 86);
    }
  }
  int frame = 0;
  //sf::Clock clock;  // starts the clock
  while (window.isOpen()) {
    auto t0 = std::chrono::steady_clock::now();

    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::Escape:
              std::cout << "the escape key was pressed" << std::endl;
              std::cout << "control:" << event.key.control << std::endl;
              std::cout << "alt:" << event.key.alt << std::endl;
              std::cout << "shift:" << event.key.shift << std::endl;
              std::cout << "system:" << event.key.system << std::endl;
              window.close();
              break;
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            std::cout << "the left button was pressed" << std::endl;
            std::cout << "mouse x: " << event.mouseButton.x << std::endl;
            std::cout << "mouse y: " << event.mouseButton.y << std::endl;
          }
          break;
        case sf::Event::MouseMoved:
          sprite.setPosition(event.mouseMove.x, event.mouseMove.y);
          std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
          std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
          break;
      }
    }

    sprite.setTextureRect(anim_coords[++frame % 27]);

    window.clear(sf::Color::Black);
    window.draw(pauseMessage);
    window.draw(sprite);
    window.display();

    // geom_2d::debug("Elapsed {} ms\n", clock.restart().asMilliseconds());
  }

  return 0;
}
