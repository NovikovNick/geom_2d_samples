#ifndef GEOM_2D_LAYOUT_SERVER_H_
#define GEOM_2D_LAYOUT_SERVER_H_

#include <SFML/Graphics.hpp>
#include <iostream>

#include "resource_manager.h"
#include "ui/button.cc"
#include "ui/groupable.cc"
#include "ui/label.cc"
#include "ui/layout.cc"
#include "util.h"

namespace geom_2d {

class LayoutService {
 public:
  int width, height;
  sf::Color key_clr, background_clr, txt_clr;
  std::shared_ptr<Layout> menu;
  std::shared_ptr<Button> start_btn, exit_btn;

  LayoutService(const int width, const int height,
                std::shared_ptr<ResourceManager> resource_mng)
      : width(width),
        height(height),
        key_clr(sf::Color(255, 165, 0)),
        background_clr(sf::Color::Black),
        txt_clr(sf::Color::White) {
    auto version_lbl = std::make_shared<Label>(
        sf::Text(L"v0.2.0", resource_mng->GetDefaultFont(), 25));
    version_lbl->setPosition(186, 181);
    version_lbl->label.setFillColor(txt_clr);

    auto game_title_lbl = std::make_shared<Label>(
        sf::Text(L"Змейка", resource_mng->GetTitleFont(), 150));
    game_title_lbl->label.setFillColor(key_clr);
    game_title_lbl->setPosition(500, -20);
    Groupable::assign(game_title_lbl, version_lbl);

    start_btn = std::make_shared<Button>(
        400, 105, sf::Text(L"Начать игру", resource_mng->GetDefaultFont(), 50));
    start_btn->form.setFillColor(key_clr);
    start_btn->on_hover_form.setFillColor(key_clr);
    start_btn->on_hover_form.setOutlineColor(txt_clr);
    start_btn->on_hover_form.setOutlineThickness(5.f);
    start_btn->label.setPosition(37, 13);
    start_btn->label.setFillColor(txt_clr);

    exit_btn = std::make_shared<Button>(
        400, 105, sf::Text(L"Выход", resource_mng->GetDefaultFont(), 50));
    exit_btn->form.setFillColor(key_clr);
    exit_btn->on_hover_form.setFillColor(key_clr);
    exit_btn->on_hover_form.setOutlineColor(txt_clr);
    exit_btn->on_hover_form.setOutlineThickness(5.f);
    exit_btn->label.setPosition(119, 14);
    exit_btn->label.setFillColor(txt_clr);
    exit_btn->setPosition(0, 127);

    menu = std::make_shared<Layout>();
    menu->setPosition((width - 960) / 2, (height - 250) / 2);
    Groupable::assign(menu, start_btn);
    Groupable::assign(menu, exit_btn);
    Groupable::assign(menu, game_title_lbl);
  };
};

};      // namespace geom_2d
#endif  // GEOM_2D_LAYOUT_SERVER_H_
