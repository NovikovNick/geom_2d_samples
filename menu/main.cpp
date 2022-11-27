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
#include "src/util.h"

int main() {
  using namespace geom_2d;

  // 1. setup window
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  auto mode = sf::VideoMode(1100, 730);
  auto style = sf::Style::Fullscreen;
  sf::RenderWindow window(mode, L"Змейка v0.2", style, settings);
  // Never use both setVerticalSyncEnabled and setFramerateLimit at the same
  // time!
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(4);
  window.setMouseCursorVisible(false);

  // 2. init services
  auto resource_mng = std::make_shared<ResourceManager>();
  auto layout_srv = std::make_shared<LayoutService>(
      window.getSize().x, window.getSize().y, resource_mng);

  // 3. init other elements
  // cursor
  Cursor cursor(40, sf::Color::Red);
  sf::Sound click_sound(resource_mng->GetClickSoundBuffer());
  click_sound.setVolume(30.0f);

  // 5. event handling
  layout_srv->start_btn->onHover = [&click_sound](const Button& b) {
    click_sound.play();
  };
  layout_srv->start_btn->onClick = [&layout_srv, &click_sound]() {
    click_sound.play();
    layout_srv->onStart();
  };

  layout_srv->exit_btn->onHover = [&click_sound](const Button& b) {
    click_sound.play();
  };
  layout_srv->exit_btn->onClick = [&window]() { window.close(); };

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
          if (layout_srv->game_rules->active || layout_srv->game_win->active ||
              layout_srv->game_lose->active) {
            layout_srv->onRulesRead();

          } else {
            switch (event.key.code) {
              case sf::Keyboard::Escape:
                std::cout << "the escape key was pressed" << std::endl;
                std::cout << "control:" << event.key.control << std::endl;
                std::cout << "alt:" << event.key.alt << std::endl;
                std::cout << "shift:" << event.key.shift << std::endl;
                std::cout << "system:" << event.key.system << std::endl;
                window.close();
                break;
              case sf::Keyboard::Q:
                if (layout_srv->game_layout->active) layout_srv->onWin();
              case sf::Keyboard::W:
                if (layout_srv->game_layout->active) layout_srv->onLose();
                break;
            }
          }

          break;
        case sf::Event::MouseButtonPressed:

          if (layout_srv->game_rules->active || layout_srv->game_win->active ||
              layout_srv->game_lose->active) {
            layout_srv->onRulesRead();
          } else {
            if (event.mouseButton.button == sf::Mouse::Left) {
              auto [_, x, y] = event.mouseButton;
              for (const auto& btn : layout_srv->active_layout->buttons) {
                if (btn->hover) btn->click();
              }
              debug(".setPosition({}, {});\n", x, y);
            }
          }

          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor.setPosition(x, y);
          // layout_srv->tmp->setPosition(x, y);

          auto bounds = cursor.getBounds();
          for (const auto& btn : layout_srv->active_layout->buttons) {
            btn->setHover(btn->intersects(bounds));
          }
          break;
      }
    }

    window.clear(layout_srv->background_clr);
    window.draw(*layout_srv->active_layout);
    window.draw(cursor);
    window.display();
  }

  return 0;
}
