#pragma once

#include "ecs/World.h"

#include <SFML/Graphics.hpp>

/*
  Рисует состояние мира в отдельном окне SFML.
  Только читает World — не вызывает системы, не меняет состояние.

  Каждая клетка — цветной прямоугольник со символом поверх:
    @  — игрок     (синий)
    N  — NPC        (оранжевый)
    *  — еда        (зелёный)
    #  — препятствие (серый)
    .  — пусто      (тёмный фон, без символа)
*/
class SFMLRenderer
{
public:
    /* Размер одной клетки в пикселях — используется также в Game для размера окна */
    static constexpr unsigned kCellPx = 30u;

    /* Загружает шрифт. Возвращает false если шрифт не найден — символы не рисуются. */
    bool init();

    void render(const World& world, sf::RenderWindow& window) const;

private:
    sf::Font font_;
    bool     fontLoaded_ = false;
};
