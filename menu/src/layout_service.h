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
  int width, height;

  void initMenu(const int screen_width, const int screen_height,
                std::shared_ptr<ResourceManager> resource_mng) {
    auto version_lbl = std::make_shared<Label>(
        sf::Text(L"v0.2.0", resource_mng->GetDefaultFont(), 25));
    version_lbl->setPosition(236, 180);
    version_lbl->label.setFillColor(txt_clr);

    auto game_title_lbl = std::make_shared<Label>(
        sf::Text(L"Змейка", resource_mng->GetTitleFont(), 150));
    game_title_lbl->label.setFillColor(key_clr);
    game_title_lbl->setPosition(550, 230);
    Groupable::assign(game_title_lbl, version_lbl);

    start_btn = std::make_shared<Button>(
        400, 105, sf::Text(L"Начать игру", resource_mng->GetDefaultFont(), 50));
    start_btn->form.setFillColor(key_clr);
    start_btn->on_hover_form.setFillColor(key_clr);
    start_btn->on_hover_form.setOutlineColor(txt_clr);
    start_btn->on_hover_form.setOutlineThickness(5.f);
    start_btn->label.setPosition(37, 14);
    start_btn->label.setFillColor(txt_clr);
    start_btn->setPosition(100, 267);

    exit_btn = std::make_shared<Button>(
        400, 105, sf::Text(L"Выход", resource_mng->GetDefaultFont(), 50));
    exit_btn->form.setFillColor(key_clr);
    exit_btn->on_hover_form.setFillColor(key_clr);
    exit_btn->on_hover_form.setOutlineColor(txt_clr);
    exit_btn->on_hover_form.setOutlineThickness(5.f);
    exit_btn->label.setPosition(119, 14);
    exit_btn->label.setFillColor(txt_clr);
    exit_btn->setPosition(100, 390);

    menu = std::make_shared<Layout>();
    menu->setPosition((screen_width - width) / 2, (screen_height - height) / 2);

    Groupable::assign(menu, start_btn);
    menu->buttons.push_back(start_btn);

    Groupable::assign(menu, exit_btn);
    menu->buttons.push_back(exit_btn);

    Groupable::assign(menu, game_title_lbl);
  }

  void initGameLayout(const int screen_width, const int screen_height,
                      std::shared_ptr<ResourceManager> resource_mng) {
    auto game_score_lbl = std::make_shared<Label>(
        sf::Text(L"10 / 10", resource_mng->GetTitleFont(), 100));
    game_score_lbl->label.setFillColor(key_clr);
    game_score_lbl->setPosition(760, 70);

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
    game_layout->setPosition((screen_width - width) / 2,
                             (screen_height - height) / 2);
    Groupable::assign(game_layout, game_score_lbl);
    Groupable::assign(game_layout, map);
  }

  void initGameRulesLayout(const int screen_width, const int screen_height,
                           std::shared_ptr<ResourceManager> resource_mng) {
    auto lbl_1 = std::make_shared<Label>(sf::Text(
        L"\nУправление на стрелочки", resource_mng->GetTitleFont(), 60));
    lbl_1->label.setFillColor(key_clr);
    lbl_1->setPosition(229, 71);

    auto img = std::make_shared<Image>(resource_mng->GetKeyArrowsTexture());
    img->sprite.scale(0.2f, 0.2f);
    img->sprite.setColor(key_clr);
    img->setPosition(359, 214);

    auto lbl_2 = std::make_shared<Label>(
        sf::Text(L"Нажмите на любую клавишу, чтобы начать",
                 resource_mng->GetTitleFont(), 50));
    lbl_2->label.setFillColor(key_clr);
    lbl_2->setPosition(105, 512);
    game_rules = std::make_shared<Layout>();
    game_rules->setPosition((screen_width - width) / 2,
                            (screen_height - height) / 2);
    Groupable::assign(game_rules, lbl_2);
    Groupable::assign(game_rules, lbl_1);
    Groupable::assign(game_rules, img);
  }

  void initGameLoseLayout(const int screen_width, const int screen_height,
                          std::shared_ptr<ResourceManager> resource_mng) {
    auto lbl = std::make_shared<Label>(
        sf::Text(L"Проигрыш...", resource_mng->GetTitleFont(), 150));
    lbl->label.setFillColor(key_clr);
    lbl->setPosition(233, 249);
    game_lose = std::make_shared<Layout>();
    game_lose->setPosition((screen_width - width) / 2,
                           (screen_height - height) / 2);
    Groupable::assign(game_lose, lbl);
  }

  void initGameWinLayout(const int screen_width, const int screen_height,
                         std::shared_ptr<ResourceManager> resource_mng) {
    auto lbl = std::make_shared<Label>(
        sf::Text(L"Победа!", resource_mng->GetTitleFont(), 150));
    lbl->label.setFillColor(key_clr);
    lbl->setPosition(319, 246);
    game_win = std::make_shared<Layout>();
    game_win->setPosition((screen_width - width) / 2,
                          (screen_height - height) / 2);
    Groupable::assign(game_win, lbl);
  }

 public:
  sf::Color key_clr, background_clr, txt_clr;
  std::shared_ptr<Layout> menu, game_layout, game_rules, game_lose, game_win;
  std::shared_ptr<Button> start_btn, exit_btn;
  std::shared_ptr<Layout> active_layout;
  // std::shared_ptr<Label> tmp;

  LayoutService(const int screen_width, const int screen_height,
                std::shared_ptr<ResourceManager> resource_mng)
      : key_clr(sf::Color(255, 100, 0)),
        background_clr(sf::Color(25, 25, 25)),
        txt_clr(sf::Color::White),
        width(1100),
        height(730) {
    initMenu(screen_width, screen_height, resource_mng);
    initGameLayout(screen_width, screen_height, resource_mng);
    initGameRulesLayout(screen_width, screen_height, resource_mng);
    initGameWinLayout(screen_width, screen_height, resource_mng);
    initGameLoseLayout(screen_width, screen_height, resource_mng);

    active_layout = menu;
    active_layout->active = true;
  };

  void onStart() { changeLayout(game_rules); }
  void onRulesRead() { changeLayout(game_layout); }
  void onWin() { changeLayout(game_win); }
  void onLose() { changeLayout(game_lose); }

 private:
  void changeLayout(std::shared_ptr<Layout> layout) {
    active_layout->active = false;
    active_layout = layout;
    active_layout->active = true;
  }
};

};      // namespace geom_2d
#endif  // GEOM_2D_LAYOUT_SERVER_H_
