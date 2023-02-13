#include "render.h"

#include <SFML/Graphics.hpp>

#include "iostream"
#include "util.h"

namespace {

struct Coord {
  float x, y;
};
Coord pivot{400, 400};
float point_radius = 2.5f;

std::vector<sf::VertexArray> lines;
std::vector<sf::CircleShape> points;

}  // namespace

namespace math {

Render::Render() = default;

void Render::drawVector(const int x, const int y) {
  sf::VertexArray l(sf::LinesStrip, 2);
  l[0].position = sf::Vector2f(pivot.x, pivot.y);
  l[1].position = sf::Vector2f(pivot.x + x, pivot.y + y);
  lines.push_back(l);
  debug("Vector at [{:7.2f};{:7.2f}]\n", x - pivot.x, y - pivot.y);
}

void Render::drawPoint(const int x, const int y) {
  sf::CircleShape p(point_radius * 2);
  p.setFillColor(sf::Color::Green);
  p.setPosition({x - point_radius, y - point_radius});
  points.push_back(p);
  debug("Point at  [{:7.2f};{:7.2f}]\n", x - pivot.x, y - pivot.y);
}

void Render::render() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 16;
  auto mode = sf::VideoMode(static_cast<unsigned int>(pivot.x * 2),
                            static_cast<unsigned int>(pivot.y * 2));
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Math", style, settings);

  drawPoint(pivot.x, pivot.y);

  debug("======start rendering======\n");
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            drawPoint(x, y);
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          break;
      }
      if (event.type == sf::Event::Closed) window.close();
    }

    window.clear();
    for (const auto& line : lines) window.draw(line);
    for (const auto& point : points) window.draw(point);
    window.display();
  }
}

}  // namespace math
