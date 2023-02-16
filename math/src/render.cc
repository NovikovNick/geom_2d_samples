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

const static sf::Color BG_COLOR(35, 35, 35);
const static sf::Color FST_COLOR(255, 100, 0);
const static sf::Color SND_COLOR(255, 17, 17);
const static sf::Color TRD_COLOR(255, 255, 255);

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

  l[0].color = SND_COLOR;
  l[1].color = SND_COLOR;

  lines.push_back(l);
  vectors.push_back(to);
  debug("Vector from [{:7.2f};{:7.2f}] to [{:7.2f};{:7.2f}]\n", from.x(),
        from.y(), to.x(), to.y());
}

Eigen::Vector2f Render::drawPoint(const Eigen::Vector2f& world_coord) {
  sf::CircleShape p(point_radius);
  p.setFillColor(SND_COLOR);
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

  debug("======start rendering======\n");

  using namespace std::chrono;
  using namespace Eigen;

  Vector2f cursor{300, 0}, base{200, 300}, origin{0, 0};
  float radius = (cursor - origin).norm();
  float base_length = (base - origin).norm();

  drawPoint(origin);

  drawVector(base);                 // base
  drawVector(cursor);               // cursor
  drawVector(Vector2f{radius, 0});  // projection x
  drawVector(Vector2f{0, radius});  // projection y
  drawVector(origin);               // perpendicular_x
  drawVector(origin);               // perpendicular_y

  auto& base_line = lines[0];
  base_line[0].color = base_line[1].color = TRD_COLOR;

  auto& cursor_line = lines[1];
  auto& projection_x = lines[2];
  auto& projection_y = lines[3];
  auto& perpendicular_x = lines[4];
  auto& perpendicular_y = lines[5];

  auto& origin_point = points[0];

  Info info(font_, FST_COLOR);

  bool dragable = false;
  bool p1_selected = false, p2_selected = false, origin_selected = false;

  float sel_area = 10.f;
  sf::CircleShape selected(sel_area);
  selected.setFillColor(FST_COLOR);
  Vector2f area(sel_area, sel_area);

  GameObject cursor_arrow({{-1.f, 0.3f}, {0.f, 0.f}, {-1.f, -0.3f}});
  GameObject base_arrow({{-1.f, 0.3f}, {0.f, 0.f}, {-1.f, -0.3f}});

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  double dx = 0;
  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    t0 = t1;
    info.update(Info::INDEX::FPS, 1 / dx);
    info.update(Info::INDEX::DX, dx);

    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::A:
              p1_selected = true;
              break;
            case sf::Keyboard::D:
              p2_selected = true;
              break;
            case sf::Keyboard::W:
              origin_selected = true;
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              p1_selected = false;
              break;
            case sf::Keyboard::D:
              p2_selected = false;
              break;
            case sf::Keyboard::W:
              origin_selected = false;
              break;
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            dragable = true;
            // drawPoint(toWorldCoord({x, y}));
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left) dragable = false;
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          Vector2f p_offset{point_radius, point_radius};
          auto cur = toWorldCoord({x, y});

          bool intersect_x = cursor.x() <= cur.x() + sel_area &&
                             cursor.x() >= cur.x() - sel_area;
          bool intersect_y = cursor.y() <= cur.y() + sel_area &&
                             cursor.y() >= cur.y() - sel_area;
          bool base_x =
              base.x() <= cur.x() + sel_area && base.x() >= cur.x() - sel_area;
          bool base_y =
              base.y() <= cur.y() + sel_area && base.y() >= cur.y() - sel_area;
          bool origin_x = origin.x() <= cur.x() + sel_area &&
                          origin.x() >= cur.x() - sel_area;
          bool origin_y = origin.y() <= cur.y() + sel_area &&
                          origin.y() >= cur.y() - sel_area;

          if (dragable) {
            if (p1_selected) selected.setPosition(toScreenCoord(cursor - area));
            if (p2_selected) selected.setPosition(toScreenCoord(base - area));
            if (origin_selected)
              selected.setPosition(toScreenCoord(origin - area));
          } else if (intersect_x && intersect_y) {
            selected.setPosition(toScreenCoord(cursor - area));
            p1_selected = true;
            p2_selected = origin_selected = false;
          } else if (base_x && base_y) {
            selected.setPosition(toScreenCoord(base - area));
            p2_selected = true;
            p1_selected = origin_selected = false;
          } else if (origin_x && origin_y) {
            selected.setPosition(toScreenCoord(origin - area));
            origin_selected = true;
            p1_selected = p2_selected = false;
          } else {
            p2_selected = p1_selected = origin_selected = false;
          }

          if (dragable && (p1_selected || p2_selected || origin_selected)) {
            if (p1_selected) cursor = cur;
            if (p2_selected) base = cur;
            if (origin_selected) origin = cur;

            cursor -= origin;
            base -= origin;
            radius = cursor.norm();
            base_length = base.norm();

            cursor.normalize();
            base.normalize();

            auto dot = cursor.dot(base);
            auto cross = cursor.cross(base);
            float angle = std::acos(dot);

            info.update(Info::INDEX::DOT, dot);
            info.update(Info::INDEX::CROSS, cross);
            info.update(Info::INDEX::ANGLE, angle * 180 / 3.14);
            info.update(Info::INDEX::COS, std::cos(angle));
            info.update(Info::INDEX::SIN, std::sin(angle));
            info.update(Info::INDEX::ATAN,
                        std::atan2(cursor.y(), cursor.x()) * 180 / 3.14);

            auto xP = std::cos(angle) * radius;
            auto yP = (cross >= 0 ? -1 : 1) * std::sin(angle) * radius;
            auto d2 = Vector2f{-base.y(), base.x()};

            projection_x[0].position = toScreenCoord(origin);
            projection_x[1].position = toScreenCoord(base * xP + origin);

            projection_y[0].position = toScreenCoord(origin);
            projection_y[1].position = toScreenCoord(d2 * yP + origin);

            cursor_line[0].position = toScreenCoord(origin);
            cursor_line[1].position = toScreenCoord(cursor * radius + origin);

            base_line[0].position = toScreenCoord(origin);
            base_line[1].position = toScreenCoord(base * base_length + origin);

            perpendicular_x[0] = toScreenCoord(d2 * yP + origin);
            perpendicular_x[1] = toScreenCoord(cursor * radius + origin);

            perpendicular_y[0] = toScreenCoord(base * xP + origin);
            perpendicular_y[1] = toScreenCoord(cursor * radius + origin);

            cursor = cursor * radius + origin;
            base = base * base_length + origin;

            origin_point.setPosition(toScreenCoord(origin - p_offset));

            cursor_arrow.position = cursor;
            cursor_arrow.rotation = (cursor - origin).normalized();

            base_arrow.position = base;
            base_arrow.rotation = (base - origin).normalized();

          }
          break;
      }
      if (event.type == sf::Event::Closed) window.close();
    }

    sf::CircleShape shape(base_length);
    shape.setPointCount(75);
    shape.setPosition(
        toScreenCoord(origin - Vector2f{base_length, base_length}));
    shape.setFillColor(BG_COLOR);
    shape.setOutlineThickness(1);
    shape.setOutlineColor(TRD_COLOR);

    window.clear(BG_COLOR);
    if (dragable) window.draw(shape);
    if (p1_selected || p2_selected || origin_selected) window.draw(selected);
    for (const auto& line : lines) window.draw(line);

    sf::VertexArray arrow_coords(sf::Triangles, cursor_arrow.size());
    for (int i = 0; i < cursor_arrow.size(); ++i) {
      arrow_coords[i].position = toScreenCoord(cursor_arrow[i]);
    }
    window.draw(arrow_coords);

    sf::VertexArray base_arrow_coords(sf::Triangles, base_arrow.size());
    for (int i = 0; i < base_arrow.size(); ++i) {
      base_arrow_coords[i].position = toScreenCoord(base_arrow[i]);
    }
    window.draw(base_arrow_coords);

    for (const auto& point : points) window.draw(point);
    window.draw(info);

    window.display();
  }
}
}  // namespace math
