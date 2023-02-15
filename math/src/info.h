#ifndef GEOM_2D_MATH_INFO_H
#define GEOM_2D_MATH_INFO_H
#include <SFML/Graphics.hpp>
#include <iostream>

namespace math {

class Info : public sf::Drawable {
  sf::Font font_;

 public:
  std::vector<sf::Text> rows_;
  enum class INDEX : uint8_t { FPS = 0, DX = 1 };

  Info(const sf::Font& font);

  void update(Info::INDEX index, const std::string& str);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_INFO_H