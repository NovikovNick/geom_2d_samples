#include "render.h"

#include <bitset>
#include <cmath>
#include <iostream>

#include "game_object.cc"
#include "game_object.h"
#include "info.cc"
#include "info.h"
#include "util.h"

namespace {

const static float kPi = std::acos(-1.0);
const static float kDegree = 180 / kPi;

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

void Render::drawVector(const Eigen::Vector2f& world_coord,
                        const sf::Color color) {
  drawVector({0, 0}, world_coord, color);
}

void Render::drawVector(const Eigen::Vector2f& from, const Eigen::Vector2f& to,
                        const sf::Color color) {
  sf::VertexArray l(sf::LinesStrip, 2);
  l[0].position = toScreenCoord(from);
  l[1].position = toScreenCoord(to);

  l[0].color = color;
  l[1].color = color;

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
  window.setFramerateLimit(24);

  debug("======start rendering======\n");

  using namespace std::chrono;
  using namespace Eigen;

  Vector2f lhs{300, 0}, rhs{200, 300}, origin{0, 0};
  float lhs_length = (lhs - origin).norm();
  float rhs_length = (rhs - origin).norm();

  drawPoint(origin);

  drawVector(rhs, TRD_COLOR);     // base
  drawVector(lhs, SND_COLOR);     // cursor
  drawVector(origin, SND_COLOR);  // projection x
  drawVector(origin, SND_COLOR);  // projection y
  drawVector(origin, TRD_COLOR);  // rhs_perpendicular
  drawVector(origin, TRD_COLOR);  // rhs_inverted

  auto& rhs_line = lines[0];
  auto& lhs_line = lines[1];
  auto& projection_cos = lines[2];
  auto& projection_sin = lines[3];
  auto& rhs_perpendicular = lines[4];
  auto& rhs_inverted = lines[5];

  auto& origin_point = points[0];

  Info info(font_, FST_COLOR);

  sf::Text lhs_label("LHS", font_, 18);
  lhs_label.setFillColor(SND_COLOR);

  sf::Text rhs_label("RHS", font_, 18);
  rhs_label.setFillColor(TRD_COLOR);

  sf::Text rhs_unit_circle_1_quarter_label("  I\n dot +\ncross +", font_, 14);
  rhs_unit_circle_1_quarter_label.setFillColor(TRD_COLOR);

  sf::Text rhs_unit_circle_2_quarter_label("  II\n dot -\ncross +", font_, 14);
  rhs_unit_circle_2_quarter_label.setFillColor(TRD_COLOR);

  sf::Text rhs_unit_circle_3_quarter_label("  III\n dot -\ncross -", font_, 14);
  rhs_unit_circle_3_quarter_label.setFillColor(TRD_COLOR);

  sf::Text rhs_unit_circle_4_quarter_label("  IV\n dot +\ncross -", font_, 14);
  rhs_unit_circle_4_quarter_label.setFillColor(TRD_COLOR);

  bool dragable = false;
  bool lhs_selected = false, rhs_selected = false, origin_selected = false;

  float sel_area = 10.f;
  sf::CircleShape selected(sel_area);
  selected.setFillColor(FST_COLOR);
  Vector2f area(sel_area, sel_area);

  GameObject lhs_arrow({{-1.f, 0.3f}, {0.f, 0.f}, {-1.f, -0.3f}});
  GameObject rhs_arrow({{-1.f, 0.3f}, {0.f, 0.f}, {-1.f, -0.3f}});

  sf::VertexArray angle_arc(sf::LinesStrip, 11);
  for (int i = 0; i < 11; ++i) angle_arc[i].color = FST_COLOR;

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
              lhs_selected = true;
              break;
            case sf::Keyboard::D:
              rhs_selected = true;
              break;
            case sf::Keyboard::W:
              origin_selected = true;
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              lhs_selected = false;
              break;
            case sf::Keyboard::D:
              rhs_selected = false;
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
          if (event.mouseButton.button == sf::Mouse::Right) {
            debug("right click\n");
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left) dragable = false;
          break;
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          Vector2f p_offset{point_radius, point_radius};
          auto cur = toWorldCoord({x, y});

          bool lhs_x =
              lhs.x() <= cur.x() + sel_area && lhs.x() >= cur.x() - sel_area;
          bool lhs_y =
              lhs.y() <= cur.y() + sel_area && lhs.y() >= cur.y() - sel_area;
          bool rhs_x =
              rhs.x() <= cur.x() + sel_area && rhs.x() >= cur.x() - sel_area;
          bool rhs_y =
              rhs.y() <= cur.y() + sel_area && rhs.y() >= cur.y() - sel_area;
          bool origin_x = origin.x() <= cur.x() + sel_area &&
                          origin.x() >= cur.x() - sel_area;
          bool origin_y = origin.y() <= cur.y() + sel_area &&
                          origin.y() >= cur.y() - sel_area;

          if (dragable) {
            if (lhs_selected) selected.setPosition(toScreenCoord(lhs - area));
            if (rhs_selected) selected.setPosition(toScreenCoord(rhs - area));
            if (origin_selected)
              selected.setPosition(toScreenCoord(origin - area));
          } else if (lhs_x && lhs_y) {
            selected.setPosition(toScreenCoord(lhs - area));
            lhs_selected = true;
            rhs_selected = origin_selected = false;
          } else if (rhs_x && rhs_y) {
            selected.setPosition(toScreenCoord(rhs - area));
            rhs_selected = true;
            lhs_selected = origin_selected = false;
          } else if (origin_x && origin_y) {
            selected.setPosition(toScreenCoord(origin - area));
            origin_selected = true;
            lhs_selected = rhs_selected = false;
          } else {
            rhs_selected = lhs_selected = origin_selected = false;
          }

          if (dragable && (lhs_selected || rhs_selected || origin_selected)) {
            if (lhs_selected) lhs = cur;
            if (rhs_selected) rhs = cur;
            if (origin_selected) origin = cur;

            lhs -= origin;
            rhs -= origin;
            lhs_length = lhs.norm();
            rhs_length = rhs.norm();

            lhs.normalize();
            rhs.normalize();

            float dot = lhs.dot(rhs);
            float cross = lhs.cross(rhs);
            float angle = std::acos(dot);

            info.update(Info::INDEX::DOT, dot);
            info.update(Info::INDEX::CROSS, cross);
            info.update(Info::INDEX::ANGLE, angle * kDegree);
            info.update(Info::INDEX::COS, std::cos(angle));
            info.update(Info::INDEX::SIN, std::sin(angle));
            info.update(Info::INDEX::LHS_LENGTH, lhs_length);
            info.update(Info::INDEX::RHS_LENGTH, rhs_length);
            info.update(Info::INDEX::ATAN,
                        std::atan2(rhs.y(), rhs.x()) * kDegree);

            auto lhs_cos = std::cos(angle) * lhs_length;

            projection_cos[0].position = toScreenCoord(origin);
            projection_cos[1].position = toScreenCoord(rhs * lhs_cos + origin);
            projection_sin[0].position =
                toScreenCoord(lhs * lhs_length + origin);
            projection_sin[1].position = toScreenCoord(rhs * lhs_cos + origin);

            rhs_perpendicular[0].position = toScreenCoord(
                Vector2f{-rhs.y(), rhs.x()} * rhs_length + origin);
            rhs_perpendicular[1].position = toScreenCoord(
                Vector2f{rhs.y(), -rhs.x()} * rhs_length + origin);
            rhs_inverted[0].position = toScreenCoord(origin);
            rhs_inverted[1].position =
                toScreenCoord(rhs * -rhs_length + origin);

            lhs_line[0].position = toScreenCoord(origin);
            lhs_line[1].position = toScreenCoord(lhs * lhs_length + origin);

            rhs_line[0].position = toScreenCoord(origin);
            rhs_line[1].position = toScreenCoord(rhs * rhs_length + origin);

            lhs_label.setPosition(toScreenCoord(lhs * (lhs_length + 30) +
                                                origin - Vector2f{15, 9}));
            rhs_label.setPosition(toScreenCoord(rhs * (rhs_length + 30) +
                                                origin - Vector2f{15, 9}));

            rhs_unit_circle_1_quarter_label.setPosition(
                toScreenCoord(Rotation2Df(-3.14 / 4) * rhs * rhs_length / 2 +
                              origin - Vector2f{20.f, 25.f}));
            rhs_unit_circle_2_quarter_label.setPosition(toScreenCoord(
                Rotation2Df(-3.14 / 4 * 3) * rhs * rhs_length / 2 + origin -
                Vector2f{20.f, 25.f}));
            rhs_unit_circle_3_quarter_label.setPosition(toScreenCoord(
                Rotation2Df(-3.14 - 3.14 / 4) * rhs * rhs_length / 2 + origin -
                Vector2f{20.f, 25.f}));
            rhs_unit_circle_4_quarter_label.setPosition(toScreenCoord(
                Rotation2Df(-3.14 - 3.14 / 4 * 3) * rhs * rhs_length / 2 +
                origin - Vector2f{20.f, 25.f}));

            for (float i = 0; i < 11; ++i) {
              auto rotation_angle = angle * (cross > 0 ? -0.1f : 0.1f) * i;
              angle_arc[i].position =
                  toScreenCoord(Rotation2D(rotation_angle) * rhs * 30 + origin);
            }

            lhs = lhs * lhs_length + origin;
            rhs = rhs * rhs_length + origin;

            origin_point.setPosition(toScreenCoord(origin - p_offset));

            lhs_arrow.position = lhs;
            lhs_arrow.rotation = (lhs - origin).normalized();

            rhs_arrow.position = rhs;
            rhs_arrow.rotation = (rhs - origin).normalized();
          }
          break;
      }
      if (event.type == sf::Event::Closed) window.close();
    }

    sf::CircleShape rhs_unit_circle(rhs_length);
    rhs_unit_circle.setPointCount(75);
    rhs_unit_circle.setPosition(
        toScreenCoord(origin - Vector2f{rhs_length, rhs_length}));
    rhs_unit_circle.setFillColor(BG_COLOR);
    rhs_unit_circle.setOutlineThickness(1);
    rhs_unit_circle.setOutlineColor(TRD_COLOR);

    window.clear(BG_COLOR);
    if (dragable) window.draw(rhs_unit_circle);
    if (lhs_selected || rhs_selected || origin_selected) window.draw(selected);

    window.draw(rhs_line);
    window.draw(lhs_line);
    if (dragable) window.draw(rhs_perpendicular);
    if (dragable) window.draw(rhs_inverted);
    if (dragable) window.draw(projection_cos);
    if (dragable) window.draw(projection_sin);
    if (dragable) window.draw(rhs_unit_circle_1_quarter_label);
    if (dragable) window.draw(rhs_unit_circle_2_quarter_label);
    if (dragable) window.draw(rhs_unit_circle_3_quarter_label);
    if (dragable) window.draw(rhs_unit_circle_4_quarter_label);

    sf::VertexArray arrow_coords(sf::Triangles, lhs_arrow.size());
    for (int i = 0; i < lhs_arrow.size(); ++i) {
      arrow_coords[i].position = toScreenCoord(lhs_arrow[i]);
      arrow_coords[i].color = SND_COLOR;
    }
    window.draw(arrow_coords);

    sf::VertexArray base_arrow_coords(sf::Triangles, rhs_arrow.size());
    for (int i = 0; i < rhs_arrow.size(); ++i) {
      base_arrow_coords[i].position = toScreenCoord(rhs_arrow[i]);
    }
    window.draw(base_arrow_coords);
    window.draw(angle_arc);

    if (!dragable) window.draw(lhs_label);
    if (!dragable) window.draw(rhs_label);

    for (const auto& point : points) window.draw(point);
    window.draw(info);

    window.display();
  }
}
}  // namespace math
