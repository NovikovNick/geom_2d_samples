#ifndef GEOM_2D_MATH_INFO_H
#define GEOM_2D_MATH_INFO_H
#include <SFML/Graphics.hpp>
#include <iostream>

namespace math {

class Info : public sf::Drawable {
  const static uint8_t index_count = 8;
  sf::Font font_;
  sf::Color text_color;

 public:
  enum class INDEX : uint8_t {
    FPS = 0,
    DX = 1,
    DOT = 2,
    CROSS = 3,
    ANGLE = 4,
    COS = 5,
    SIN = 6,
    ATAN = 7
  };
  std::vector<sf::Text> rows_;

  Info(const sf::Font& font, const sf::Color text_color);

  void update(Info::INDEX index, const float val);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

 private:
  std::unordered_map<INDEX, std::string> formats{
      {INDEX::FPS, "FPS: {:.0f}\n"},     {INDEX::DX, "delta(sec): {:.6f}\n"},
      {INDEX::DOT, "dot: {:.5f}\n"},     {INDEX::CROSS, "cross: {:.5f}\n"},
      {INDEX::ANGLE, "angle: {:.5f}\n"}, {INDEX::COS, "cos: {:.5f}\n"},
      {INDEX::SIN, "sin: {:.5f}\n"},     {INDEX::ATAN, "atan2: {:.5f}\n"}};
};

};      // namespace math
#endif  // GEOM_2D_MATH_INFO_H