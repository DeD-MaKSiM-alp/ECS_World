#include "ecs/SFMLRenderer.h"

#include <string>

bool SFMLRenderer::init()
{
    /* Consolas — моноширинный шрифт, стандартно есть на Windows */
    fontLoaded_ = font_.loadFromFile("C:\\Windows\\Fonts\\consola.ttf");
    return fontLoaded_;
}

/*
  Рисует сетку Grid::kHeight × Grid::kWidth.
  Вызывается каждый кадр — состояние мира читается напрямую из World.

  Порядок приоритетов символов при нескольких сущностях в клетке:
    игрок (@) > NPC (N) > еда (*) > препятствие (#)
*/
void SFMLRenderer::render(const World& world, sf::RenderWindow& window) const
{
    window.clear(sf::Color(20, 20, 20));

    sf::RectangleShape cell(sf::Vector2f(kCellPx - 1.f, kCellPx - 1.f));

    sf::Text text;
    if (fontLoaded_)
    {
        text.setFont(font_);
        text.setCharacterSize(static_cast<unsigned>(kCellPx * 0.65f));
        text.setFillColor(sf::Color::White);
    }

    for (std::int32_t y = 0; y < Grid::kHeight; ++y)
    {
        for (std::int32_t x = 0; x < Grid::kWidth; ++x)
        {
            const auto& entities = world.grid().entitiesAt(x, y);

            char           sym   = '.';
            sf::Color      color(40, 40, 40);

            for (EntityId id : entities)
            {
                if (world.hasPlayerControlled(id))
                {
                    sym = '@'; color = sf::Color(50, 100, 200);
                    break;
                }
                if (world.getHunger(id))
                {
                    sym = 'N'; color = sf::Color(200, 120, 50);
                }
                else if (world.hasFood(id) && sym == '.')
                {
                    sym = '*'; color = sf::Color(50, 180, 50);
                }
                else if (world.hasBlocking(id) && sym == '.')
                {
                    sym = '#'; color = sf::Color(120, 120, 120);
                }
            }

            /* Фон клетки */
            cell.setFillColor(color);
            cell.setPosition(
                static_cast<float>(x) * kCellPx,
                static_cast<float>(y) * kCellPx);
            window.draw(cell);

            /* Символ поверх (только для непустых клеток) */
            if (fontLoaded_ && sym != '.')
            {
                text.setString(std::string(1, sym));

                /* Центрировать символ в клетке */
                const sf::FloatRect bounds = text.getLocalBounds();
                text.setPosition(
                    static_cast<float>(x) * kCellPx + (kCellPx - bounds.width)  / 2.f - bounds.left,
                    static_cast<float>(y) * kCellPx + (kCellPx - bounds.height) / 2.f - bounds.top);

                window.draw(text);
            }
        }
    }

    window.display();
}
