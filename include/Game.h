#pragma once

#include "ecs/SFMLRenderer.h"
#include "ecs/World.h"

#include <SFML/Graphics.hpp>
#include <chrono>

/*
  Game — оболочка игрового цикла.
  Держит World, окно SFML и рендерер.

  Ответственности:
  - расстановка начальных сущностей (setupScene)
  - обработка событий SFML (handleEvent): WASD/стрелки, пауза, выход
  - тайминг тиков: от ввода (лимит 5 тик/с) и авто-тик при бездействии (~1 Гц)
  - запись MoveTarget для игрока — без прямого изменения Position
  - вызов World::tick() строго один раз за итерацию

  Системы вызываются только из Game::tick() в фиксированном порядке.
*/
class Game
{
public:
    void run();

private:
    World            world_;
    sf::RenderWindow window_;
    SFMLRenderer     renderer_;

    bool     paused_  = false;
    bool     running_ = true;
    EntityId playerId_{};

    /* Последнее намерение игрока до выполнения тика */
    struct PendingInput
    {
        std::int32_t dx = 0;
        std::int32_t dy = 0;
        bool hasInput   = false;
    };
    PendingInput pending_{};

    using Clock     = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Ms        = std::chrono::milliseconds;

    TimePoint lastTickTime_  = Clock::now();
    TimePoint lastInputTime_ = Clock::now();

    /* 200 мс между тиками от ввода = не более 5 тиков/с */
    static constexpr long long kInputTickMs = 200;
    /* Авто-тик при бездействии раз в 1 секунду */
    static constexpr long long kAutoTickMs  = 1000;

    void setupScene();
    void tick();
    void handleEvent(const sf::Event& event);
};
