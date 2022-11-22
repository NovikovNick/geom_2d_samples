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
// 1. snake board tile map
// 2. menu screen => game screen => win/lose screen
// 3. Screen transition + multithreading

int main() {
  auto resource_mng = std::make_shared<geom_2d::ResourceManager>();
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  sf::RenderWindow window(sf::VideoMode(1000, 800), "Menu",
                          sf::Style::Fullscreen, settings);

  // Never use both setVerticalSyncEnabled and setFramerateLimit at the same
  // time!
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(60);
  window.setMouseCursorVisible(false);

  // Initialize the pause message
  sf::Text msg;
  msg.setFont(resource_mng->GetFont());
  msg.setCharacterSize(40);
  msg.setString(L"Привет! ");
  msg.setPosition(575, 56);

  auto sprite_pos = sf::Vector2f(600, 120);

  sf::Sprite sprite;
  auto& texture = resource_mng->GetRunningSprite();
  texture.setSmooth(true);
  sprite.setTexture(texture);
  sprite.setPosition(sprite_pos);

  std::vector<sf::IntRect> anim_coords;
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 7; ++col) {
      anim_coords.emplace_back(col * 75, row * 86, 75, 86);
    }
  }

  sf::RectangleShape shape(sf::Vector2f(75.f, 86.f));
  shape.setFillColor(sf::Color(100, 100, 100));
  shape.setOutlineThickness(1.f);
  shape.setOutlineColor(sf::Color::Magenta);
  shape.setPosition(sprite_pos);

  // cursor
  geom_2d::Cursor cursor(30);
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile("resources/click.wav")) return -1;
  sf::Sound sound;
  sound.setBuffer(buffer);


  // tile map
  // define the level with an array of tile indices
  const std::vector<int> level{
      0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
      1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
      3, 3, 3, 3, 0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0, 0, 1,
      1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 1, 0, 3, 0, 2, 2,
      0, 0, 1, 1, 1, 1, 2, 0, 2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1,
      1, 1, 0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
  };

  // create the tilemap from the level definition
  geom_2d::TileMap map;
  if (!map.load("resources/graphics-vertex-array-tilemap-tileset.png",
                sf::Vector2u(32, 32), level, 16, 8))
    return -1;

  int fps = 1;
  int frame = 0;
  // sf::Clock clock;  // starts the clock
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
            sound.play();
            std::cout << "the left button was pressed" << std::endl;
            std::cout << "mouse x: " << event.mouseButton.x << std::endl;
            std::cout << "mouse y: " << event.mouseButton.y << std::endl;
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor.setPosition(x, y);
          msg.setFillColor(sf::Color(x % 255, y % 255, frame % 255));

          auto bounds = cursor.getBounds();
          shape.setFillColor(shape.getGlobalBounds().intersects(bounds)
                                 ? sf::Color::Green
                                 : sf::Color::Black);

          geom_2d::debug("{:6d} Bounds are: {},{}, {} - {}\n", frame,
                         bounds.left, bounds.top, bounds.width, bounds.height);
          break;
      }
    }

    // animation
    sprite.setTextureRect(anim_coords[++frame % 27]);

    window.clear(sf::Color::Black);
    window.draw(msg);
    window.draw(shape);
    window.draw(sprite);
    window.draw(map);
    window.draw(cursor);

    window.display();

    // geom_2d::debug("Elapsed {} ms\n", clock.restart().asMilliseconds());
  }

  return 0;
}
