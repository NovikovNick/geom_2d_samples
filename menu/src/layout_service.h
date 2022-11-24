#ifndef GEOM_2D_LAYOUT_SERVER_H_
#define GEOM_2D_LAYOUT_SERVER_H_

#include <SFML/Graphics.hpp>
#include <iostream>

#include "resource_manager.h"
#include "ui/button.cc"
#include "ui/groupable.cc"
#include "ui/image.cc"
#include "ui/label.cc"
#include "ui/layout.cc"
#include "ui/tile_map.cc"
#include "util.h"

namespace geom_2d {

class LayoutService {
 private:
  void initMenu(const int width, const int height,
                std::shared_ptr<ResourceManager> resource_mng) {
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
  }
  void initGameLayout(const int width, const int height,
                      std::shared_ptr<ResourceManager> resource_mng) {
    game_score_lbl = std::make_shared<Label>(
        sf::Text(L"10 / 10", resource_mng->GetTitleFont(), 100));
    game_score_lbl->label.setFillColor(key_clr);
    game_score_lbl->setPosition(760, 70);

    auto control_lbl = std::make_shared<Label>(sf::Text(
        L"\nУправление\nна стрелочки", resource_mng->GetTitleFont(), 40));
    control_lbl->label.setFillColor(key_clr);
    control_lbl->setPosition(800, 430);

    auto arrows_img =
        std::make_shared<Image>(resource_mng->GetKeyArrowsTexture());
    arrows_img->sprite.scale(0.2f, 0.2f);
    arrows_img->sprite.setColor(key_clr);
    arrows_img->sprite.setPosition(700, 180);

    std::vector<SNAKE_INDEX> level(16 * 16, SNAKE_INDEX::APPLE);
    for (int i = 0; i < 16; ++i) {
      level[i] = SNAKE_INDEX::BODY_HOR;
      level[i * 16] = SNAKE_INDEX::BODY_VER;
      level[i + 15 * 16] = SNAKE_INDEX::BODY_HOR;
      level[i * 16 + 15] = SNAKE_INDEX::BODY_VER;

      level[0] = SNAKE_INDEX::TURN_TL;
      level[15] = SNAKE_INDEX::TURN_TR;
      level[15 * 16] = SNAKE_INDEX::TURN_BL;
      level[16 * 16 - 1] = SNAKE_INDEX::TURN_BR;
    }
    auto map = std::make_shared<TileMap>(resource_mng->GetSnakeSpriteSheet(),
                                         sf::Vector2u(64, 64), level, 16, 16);

    game_layout = std::make_shared<Layout>();
    game_layout->setPosition((width - 1100) / 2, (height - 730) / 2);
    Groupable::assign(game_layout, game_score_lbl);
    Groupable::assign(game_layout, control_lbl);
    Groupable::assign(game_layout, arrows_img);
    Groupable::assign(game_layout, map);
  }

 public:
  sf::Color key_clr, background_clr, txt_clr;
  std::shared_ptr<Layout> menu, game_layout;
  std::shared_ptr<Button> start_btn, exit_btn;
  std::shared_ptr<Label> game_score_lbl;

  LayoutService(const int width, const int height,
                std::shared_ptr<ResourceManager> resource_mng)
      : key_clr(sf::Color(255, 100, 0)),
        background_clr(sf::Color(25, 25, 25)),
        txt_clr(sf::Color::White) {
    initMenu(width, height, resource_mng);
    initGameLayout(width, height, resource_mng);
  };
};

};      // namespace geom_2d
#endif  // GEOM_2D_LAYOUT_SERVER_H_
