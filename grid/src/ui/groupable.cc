#ifndef GEOM_2D_UI_GROUPABLE_H_
#define GEOM_2D_UI_GROUPABLE_H_

#include <SFML/Graphics.hpp>
#include <iostream>

namespace geom_2d {

class Groupable : public sf::Shape {
 public:
  std::shared_ptr<Groupable> parent;
  std::vector<std::shared_ptr<Groupable>> children;
  /*virtual void draw(sf::RenderTarget& target,
                    const sf::Transform& parent) const {
    sf::Transform combined = parent * getTransform();
    onDraw(target, combined);
    for (const auto& child : children) child->draw(target, combined);
  }*/

  static void assign(std::shared_ptr<Groupable> parent,
                     std::shared_ptr<Groupable> child) {
    parent->children.push_back(child);
    child->parent = parent;
  }

  /*virtual void onDraw(sf::RenderTarget& target,
                      const sf::Transform& transform) const = 0;*/

  virtual ~Groupable() = default;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform = states.transform * getTransform();
    onDraw(target, states);
    for (const auto& child : children) child->draw(target, states);
  }

  virtual void onDraw(sf::RenderTarget& target,
                      sf::RenderStates states) const {};
};

}  // namespace geom_2d
#endif  // GEOM_2D_UI_GROUPABLE_H_
