#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#define GEOM_DEBUG 1;

#include "src/cursor.cc"
#include "src/layout_service.h"
#include "src/resource_manager.h"
#include "src/tile_map.cc"
#include "src/util.h"

// todo:
// 1. game screen => win/lose screen
// 3. Screen transition + multithreading

int main() {
  using namespace geom_2d;

  // 1. setup window
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  auto mode = sf::VideoMode(1200, 800);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, L"Змейка v0.2", style, settings);
  // Never use both setVerticalSyncEnabled and setFramerateLimit at the same
  // time!
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(24);
  window.setMouseCursorVisible(false);

  // 2. init services
  auto resource_mng = std::make_shared<ResourceManager>();
  auto layout_srv = std::make_shared<LayoutService>(
      window.getSize().x, window.getSize().y, resource_mng);

  // 3. init other elements
  // cursor
  Cursor cursor(50, sf::Color::Red);
  sf::Sound click_sound(resource_mng->GetClickSoundBuffer());
  click_sound.setVolume(30.0f);

  // tile map
  std::vector<SNAKE_INDEX> level(16 * 16, SNAKE_INDEX::APPLE);
  for (int i = 0; i < 16; ++i) {
    level[i] = SNAKE_INDEX::BODY_HOR;
    level[i * 16] = SNAKE_INDEX::BODY_VER;
    level[i + 15 * 16] = SNAKE_INDEX::BODY_HOR;
    level[i * 16 + 15] = SNAKE_INDEX::BODY_VER;

    level[0] = SNAKE_INDEX::TURN_TL;
    level[15] = SNAKE_INDEX::TURN_TR;
    level[15 * 16] = SNAKE_INDEX::TURN_BL;
    level[16 * 16 - 1] = SNAKE_INDEX::TURN_BR;
  }

  TileMap map(resource_mng->GetSnakeSpriteSheet(), sf::Vector2u(64, 64), level,
              16, 16);
  map.setPosition(447, 24);

  bool is_menu_screen = true;

  layout_srv->start_btn->onHover = [&click_sound](const Button& b) {
    click_sound.play();
  };
  layout_srv->start_btn->onClick = [&is_menu_screen, &click_sound]() {
    click_sound.play();
    is_menu_screen = !is_menu_screen;
  };

  layout_srv->exit_btn->onHover = [&click_sound](const Button& b) {
    click_sound.play();
  };
  layout_srv->exit_btn->onClick = [&window]() { window.close(); };

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
            if (layout_srv->start_btn->hover) layout_srv->start_btn->click();
            if (layout_srv->exit_btn->hover) layout_srv->exit_btn->click();

            debug(".setPosition({}, {});\n", event.mouseButton.x,
                  event.mouseButton.y);
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor.setPosition(x, y);

          auto bounds = cursor.getBounds();

          if (is_menu_screen) {
            layout_srv->start_btn->setHover(
                layout_srv->start_btn->intersects(bounds));
            layout_srv->exit_btn->setHover(
                layout_srv->exit_btn->intersects(bounds));
          }

          // debug("{:6d} cursor position: {},{}\n\n", frame, x, y);
          break;
      }
    }

    window.clear(layout_srv->background_clr);

    if (is_menu_screen) {
      window.draw(*layout_srv->menu);
    } else {
      window.draw(map);
    }

    window.draw(cursor);

    window.display();
  }

  return 0;
}
