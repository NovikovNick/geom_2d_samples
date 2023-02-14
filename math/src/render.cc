#include "render.h"

#include <SFML/Graphics.hpp>

#include "iostream"
#include "util.h"

namespace {

Eigen::Translation2f world_t{-400, -400};
float point_radius = 5.f;

std::vector<Eigen::Vector2f> vectors;
std::vector<sf::VertexArray> lines;
std::vector<sf::CircleShape> points;

Eigen::Vector2f toWorldCoord(const Eigen::Vector2f& screen_coord) {
  return world_t * screen_coord;
}

sf::Vector2f toScreenCoord(const Eigen::Vector2f& world_coord) {
  Eigen::Vector2f t = world_t.inverse() * world_coord;
  return sf::Vector2f(t.x(), t.y());
}
}  // namespace

namespace math {

Render::Render() = default;

void Render::drawVector(const Eigen::Vector2f& world_coord) {
  drawVector({0, 0}, world_coord);
}

void Render::drawVector(const Eigen::Vector2f& from,
                        const Eigen::Vector2f& to) {
  sf::VertexArray l(sf::LinesStrip, 2);
  l[0].position = toScreenCoord(from);
  l[1].position = toScreenCoord(to);

  l[0].color = sf::Color::Green;
  l[1].color = sf::Color::White;

  lines.push_back(l);
  vectors.push_back(to);
  debug("Vector from [{:7.2f};{:7.2f}] to [{:7.2f};{:7.2f}]\n", from.x(),
        from.y(), to.x(), to.y());
}

Eigen::Vector2f Render::drawPoint(const Eigen::Vector2f& world_coord) {
  sf::CircleShape p(point_radius);
  p.setFillColor(sf::Color::Green);
  p.setPosition(toScreenCoord(world_coord) -
                sf::Vector2f(point_radius, point_radius));
  points.push_back(p);

  debug("Point at  [{:7.2f};{:7.2f}]\n", world_coord.x(), world_coord.y());
  return world_coord;
}

void Render::render() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 16;
  auto mode = sf::VideoMode(800, 800);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Math", style, settings);

  // auto prev = drawPoint({0, 0});
  debug("======start rendering======\n");

  auto t0 = std::chrono::steady_clock::now();
  float rotation = 0;
   
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
            // prev = drawPoint(toWorldCoord({x, y}));
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          /*vectors[1] = toWorldCoord({x, y});
          lines[1][1] = toScreenCoord(vectors[1]);*/
          break;
      }
      if (event.type == sf::Event::Closed) window.close();
    }

    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - t0);
    rotation = delta.count() / 1000.f;
    

    using namespace Eigen;

    std::vector<Eigen::Vector2f> rect{{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};
    sf::VertexArray r(sf::Quads, 4);
    for (int i = 0; i < rect.size(); ++i) {

      Transform<float, 2, Affine> t;
      t = Matrix3f::Identity();
      //t *= world_t.inverse();
      t *= Translation2f(1.f, 0.f);
      t *= Scaling(100.f, 100.f);
      t *= Rotation2Df(rotation);

      r[i].position = toScreenCoord(t * rect[i]);
    }

    window.clear();
    for (const auto& line : lines) window.draw(line);
    for (const auto& point : points) window.draw(point);

    window.draw(r);

    window.display();
  }
}

}  // namespace math
