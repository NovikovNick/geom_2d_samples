#ifndef GEOM_2D_MATH_INFO_H
#define GEOM_2D_MATH_INFO_H
#include <SFML/Graphics.hpp>
#include <iostream>

namespace math {

class Info : public sf::Drawable {
  const static uint8_t index_count = 11;
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
    LHS_ATAN = 7,
    RHS_ATAN = 8,
    LHS_LENGTH = 9,
    RHS_LENGTH = 10
  };
  std::vector<sf::Text> rows_;

  Info(const sf::Font& font, const sf::Color text_color);

  void update(Info::INDEX index, const float val);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

 private:
  std::unordered_map<INDEX, std::string> formats{
      {INDEX::FPS,          "FPS: {:.0f}\n"},
      {INDEX::DX,           "Delta(sec): {:.6f}\n"},
      {INDEX::DOT,          "Dot(Scalar) product: LHS * RHS     = {:.5f}\n"},
      {INDEX::CROSS,        "Cross(Vector) product: LHS x RHS = {:.5f}\n"},
      {INDEX::ANGLE,        "Angle: {:3.0f}\n"},
      {INDEX::COS,          "cos(angle): {:6.3f}\n"},
      {INDEX::SIN,          "sin(angle): {:6.3f}\n"},
      {INDEX::LHS_ATAN,     "LHS atan2: {:5.2f}\n"},
      {INDEX::RHS_ATAN,     "RHS atan2: {:5.2f}\n"},
      {INDEX::LHS_LENGTH,   "LHS length = {:7.2f}\n"},
      {INDEX::RHS_LENGTH,   "RHS length = {:7.2f}\n"}
  };
};

};      // namespace math
#endif  // GEOM_2D_MATH_INFO_H