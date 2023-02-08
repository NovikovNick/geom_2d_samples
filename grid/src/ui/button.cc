#include <SFML/Graphics.hpp>
#include <iostream>

#include "groupable.cc"
namespace geom_2d {


class Button final : public Groupable {
 public:
  bool hover;
  sf::RectangleShape form;
  sf::RectangleShape on_hover_form;
  sf::Text label;
  std::function<void(const Button&)> onHover;
  std::function<void()> onClick;

  Button(const int width, const int heignt, const sf::Text& label)
      : form(sf::Vector2f(width, heignt)),
        on_hover_form(sf::Vector2f(width, heignt)),
        label(label),
        hover(false),
        onHover(nullptr),
        onClick(nullptr) {}

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, states);
  }

  virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (hover) {
      target.draw(on_hover_form, states);
    } else {
      target.draw(form, states);
    }
    target.draw(label, states);
  }

  void setHover(bool val) {
    if (!hover && val && onHover) {
      onHover(*this);
    }
    hover = val;
  }

  void click() {
    setHover(false);
    if (onClick) onClick();
  }

  bool intersects(const sf::FloatRect& bound) const {
    auto t = getTransform();

    auto curr = parent;
    while (curr != nullptr) {
      t = t * parent->getTransform();
      curr = parent->parent;
    }

    auto matrix = t.getMatrix();
    auto [_, __, width, height] = form.getGlobalBounds();
    return bound.intersects({matrix[12], matrix[13], width, height});
  }

  virtual std::size_t getPointCount() const { return form.getPointCount(); }

  virtual sf::Vector2f getPoint(std::size_t index) const {
    return form.getPoint(index);
  }
};

}  // namespace geom_2d

