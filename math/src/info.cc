#include "info.h"

namespace math {

Info::Info(const sf::Font& font)
    : rows_(std::vector<sf::Text>(2)), font_(font) {
  for (int i = 0; i < 2; ++i) {
    rows_[i] = sf::Text("", font_, 18);
    rows_[i].setFillColor(sf::Color(255,100,0));
    rows_[i].setPosition(700, i * 20 + 20);
  }
};

void Info::update(Info::INDEX index, const std::string& str) {
  rows_[static_cast<int>(index)].setString(str);
}

void Info::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  for (const auto& it : rows_) target.draw(it, states);
}

};  // namespace math