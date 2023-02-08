#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#define GEOM_DEBUG 1;

#include "src/cursor.cc"
#include "src/ui/grid.cc"
#include "src/util.h"

int main() {
  using namespace geom_2d;

  // 1. setup window
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  auto mode = sf::VideoMode(1400, 800);
  auto style = sf::Style::Fullscreen;
  sf::RenderWindow window(mode, L"Grid Sample", style, settings);
  window.setVerticalSyncEnabled(true);
  window.setMouseCursorVisible(false);

  // 3. init other elements
  // cursor
  Cursor cursor(40, sf::Color::Red);

  // 6. main loop
  int frame = 0;
  while (window.isOpen()) {
    auto t0 = std::chrono::steady_clock::now();
    ++frame;
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::Escape:
              window.close();
              break;
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            debug(".setPosition({}, {});\n", x, y);
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor.setPosition(x, y);
          break;
      }
    }
    
    auto grid = std::make_shared<Grid>(8, 8, 64);

    window.clear(sf::Color::Black);
    window.draw(*grid);
    window.draw(cursor);
    window.display();
  }

  return 0;
}
