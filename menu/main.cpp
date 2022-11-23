#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#define GEOM_DEBUG 1;

#include "src/cursor.cc"
#include "src/resource_manager.h"
#include "src/tile_map.cc"
#include "src/util.h"

// todo:
// 1. game screen => win/lose screen
// 2. Screen transition + multithreading
class Layout {

};

int main() {
  using namespace geom_2d;

  auto resource_mng = std::make_shared<ResourceManager>();
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  sf::RenderWindow window(sf::VideoMode(1000, 800), L"Змейка v0.2",
                          sf::Style::Fullscreen, settings);

  // Never use both setVerticalSyncEnabled and setFramerateLimit at the same
  // time!
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(24);
  window.setMouseCursorVisible(false);


  // menu
  auto key_clr = sf::Color(255, 165, 0);
  auto background_clr = sf::Color::Black;
  auto txt_clr = sf::Color::White;

  sf::Text game_title_lbl(L"Змейка", resource_mng->GetTitleFont(), 150);
  game_title_lbl.setPosition(602, 355);
  game_title_lbl.setFillColor(key_clr);

  sf::Text start_game_lbl(L"Начать игру", resource_mng->GetDefaultFont(), 50);
  start_game_lbl.setPosition(110, 400);
  start_game_lbl.setFillColor(txt_clr);

  sf::Text exit_lbl(L"Выход", resource_mng->GetDefaultFont(), 50);
  exit_lbl.setPosition(195, 532);
  exit_lbl.setFillColor(txt_clr);

  sf::Text version_lbl(L"v0.2.0", resource_mng->GetDefaultFont(), 25);
  version_lbl.setPosition(781, 526);
  version_lbl.setFillColor(txt_clr);

  sf::RectangleShape start_btn(sf::Vector2f(400.f, 105.f));
  start_btn.setFillColor(key_clr);
  start_btn.setOutlineColor(txt_clr);
  start_btn.setPosition(75, 380);

  sf::RectangleShape exit_btn(sf::Vector2f(400.f, 105.f));
  exit_btn.setFillColor(key_clr);
  exit_btn.setOutlineColor(txt_clr);
  exit_btn.setPosition(75, 516);

  std::vector<sf::RectangleShape> btns{start_btn, exit_btn};


  // cursor
  Cursor cursor(50, sf::Color::Red);
  sf::Sound cursor_sound(resource_mng->GetClickSoundBuffer());


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

  TileMap map(resource_mng->GetSnakeSpriteSheet(), sf::Vector2u(64, 64), level, 16, 16);
  map.setPosition(447, 24);


  bool is_menu_screen = true;

  int frame = 0;
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
            cursor_sound.play();
            is_menu_screen = !is_menu_screen;
            debug(".setPosition({}, {});\n", event.mouseButton.x,
                  event.mouseButton.y);
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor.setPosition(x, y);
          // map.setPosition(x, y);

          auto bounds = cursor.getBounds();

          for (auto& btn : btns)
            btn.setOutlineThickness(
                btn.getGlobalBounds().intersects(bounds) ? 5.f : 0);

          debug("{:6d} cursor position: {},{}\n", ++frame, x, y);
          break;
      }
    }

    window.clear(background_clr);

    if (is_menu_screen) {
      window.draw(btns[0]);
      window.draw(btns[1]);

      window.draw(game_title_lbl);
      window.draw(start_game_lbl);
      window.draw(version_lbl);
      window.draw(exit_lbl);
    } else {
      window.draw(map);
    }

    window.draw(cursor);

    window.display();
  }

  return 0;
}
