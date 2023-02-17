#include "info.h"

namespace math {

Info::Info(const sf::Font& font, const sf::Color text_color)
    : rows_(std::vector<sf::Text>(index_count)),
      font_(font),
      text_color(text_color) {
  for (int i = 0; i < index_count; ++i) {
    rows_[i] = sf::Text("", font_, 18);
    rows_[i].setFillColor(text_color);
    rows_[i].setPosition(10, i * 20 + 10);
  }
};

void Info::update(Info::INDEX index, const float val) {
  auto str = std::vformat(formats[index], std::make_format_args(val));
  rows_[static_cast<size_t>(index)].setString(str);
}

void Info::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  for (const auto& it : rows_) target.draw(it, states);
}

};  // namespace math