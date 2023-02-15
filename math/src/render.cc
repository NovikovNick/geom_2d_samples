#include "render.h"

#include <bitset>

#include "game_object.cc"
#include "game_object.h"
#include "info.cc"
#include "info.h"
#include "iostream"
#include "util.h"

namespace {
const static uint8_t INPUT_LEFT = 0;
const static uint8_t INPUT_RIGHT = 1;
const static uint8_t INPUT_UP = 2;
const static uint8_t INPUT_DOWN = 3;

Eigen::Vector2f world_offset(450, 450);
Eigen::Translation2f world_t(world_offset);
float point_radius = 5.f;
float velocity = 400.f;

std::vector<Eigen::Vector2f> vectors;
std::vector<sf::VertexArray> lines;
std::vector<sf::CircleShape> points;

Eigen::Vector2f toWorldCoord(const Eigen::Vector2f& screen_coord) {
  return world_t.inverse() * screen_coord;
}

sf::Vector2f toScreenCoord(const Eigen::Vector2f& world_coord) {
  Eigen::Vector2f t = world_t * world_coord;
  return sf::Vector2f(t.x(), t.y());
}
}  // namespace

namespace math {

Render::Render() {
  if (!font_.loadFromFile("resources/default_fnt.otf"))
    debug("Unable to load default_fnt.otf!\n");
};

void Render::drawVector(const Eigen::Vector2f& world_coord) {
  drawVector({0, 0}, world_coord);
}

void Render::drawVector(const Eigen::Vector2f& from,
                        const Eigen::Vector2f& to) {
  sf::VertexArray l(sf::LinesStrip, 2);
  l[0].position = toScreenCoord(from);
  l[1].position = toScreenCoord(to);

  l[0].color = sf::Color(255, 17, 17);
  l[1].color = sf::Color(255, 17, 17);

  lines.push_back(l);
  vectors.push_back(to);
  debug("Vector from [{:7.2f};{:7.2f}] to [{:7.2f};{:7.2f}]\n", from.x(),
        from.y(), to.x(), to.y());
}

Eigen::Vector2f Render::drawPoint(const Eigen::Vector2f& world_coord) {
  sf::CircleShape p(point_radius);
  p.setFillColor(sf::Color(255, 17, 17));
  p.setPosition(toScreenCoord(world_coord) -
                sf::Vector2f(point_radius, point_radius));
  points.push_back(p);

  debug("Point at  [{:7.2f};{:7.2f}]\n", world_coord.x(), world_coord.y());
  return world_coord;
}

void Render::render() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(900, 900);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Math", style, settings);

  // reduce the framerate to minimize laptop overheat :(
  window.setFramerateLimit(60);

  // auto prev = drawPoint({0, 0});
  debug("======start rendering======\n");

  using namespace std::chrono;
  using namespace Eigen;

  Vector2f cursor = Vector2f::Identity();

  // std::vector<Vector2f> rect;
  GameObject rect_obj({{1, -1}, {1, 1}, {-1, 1}, {-1, -1}});

  std::bitset<4> input;

  Vector2f vect{300, 0};
  drawVector(vect);
  auto& line = lines[0][1];

  float dot = 0;

  Info info(font_);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  double dx = 0;
  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    t0 = t1;
    info.update(Info::INDEX::FPS, std::format("FPS: {:.0f}\n", 1 / dx));
    info.update(Info::INDEX::DX, std::format("delta(sec): {:.6f}\n", dx));

    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            drawPoint(toWorldCoord({x, y}));
          }
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input.set(INPUT_LEFT);
              break;
            case sf::Keyboard::D:
              input.set(INPUT_RIGHT);
              break;
            case sf::Keyboard::W:
              input.set(INPUT_UP);
              break;
            case sf::Keyboard::S:
              input.set(INPUT_DOWN);
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input.set(INPUT_LEFT, false);
              break;
            case sf::Keyboard::D:
              input.set(INPUT_RIGHT, false);
              break;
            case sf::Keyboard::W:
              input.set(INPUT_UP, false);
              break;
            case sf::Keyboard::S:
              input.set(INPUT_DOWN, false);
              break;
          }
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          cursor = toWorldCoord({x, y}).normalized();

          dot = cursor.dot(vect.normalized());
          float angle = std::acos(dot);
          /*dot_product_label.setString(std::format("dot={:7.2f}\n", dot));
          angle_label.setString(
              std::format("degree={:7.2f}\n", angle * 180 / 3.14));*/
          vect = Rotation2Df(std::acos(dot)) * vect;
          line.position = toScreenCoord(vect);
          break;
      }
      if (event.type == sf::Event::Closed) window.close();
    }

    if (input[INPUT_LEFT]) rect_obj.position += Vector2f(-dx * velocity, 0);
    if (input[INPUT_RIGHT]) rect_obj.position += Vector2f(dx * velocity, 0);
    if (input[INPUT_UP]) rect_obj.position += Vector2f(0, -dx * velocity);
    if (input[INPUT_DOWN]) rect_obj.position += Vector2f(0, dx * velocity);

    sf::VertexArray rectangle(sf::Quads, 4);
    for (int i = 0; i < rect_obj.size(); ++i) {
      rectangle[i].position = toScreenCoord(rect_obj[i]);
    }

    window.clear();
    window.draw(rectangle);
    for (const auto& line : lines) window.draw(line);
    for (const auto& point : points) window.draw(point);
    window.draw(info);

    window.display();
  }
}

}  // namespace math
