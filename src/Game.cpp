#include "Game.h"
#include "ecs/Systems.h"

#include <iostream>

/* -----------------------------------------------------------------------
   setupScene
   ----------------------------------------------------------------------- */
void Game::setupScene()
{
    /* NPC: голодный, блокирует клетку */
    const EntityId npc = world_.createEntity();
    world_.addPosition(npc, Position{0, 0});
    world_.addHunger(npc, Hunger{0});
    world_.addBlocking(npc);

    /* Три единицы еды */
    auto makeFood = [&](std::int32_t x, std::int32_t y)
    {
        const EntityId food = world_.createEntity();
        world_.addPosition(food, Position{x, y});
        world_.addFood(food);
    };
    makeFood(5,  5);
    makeFood(10, 3);
    makeFood(15, 15);

    /* Игрок */
    playerId_ = world_.createEntity();
    world_.addPosition(playerId_, Position{19, 19});
    world_.addHunger(playerId_, Hunger{0});
    world_.addBlocking(playerId_);
    world_.addPlayerControlled(playerId_);
}

/* -----------------------------------------------------------------------
   tick — один шаг симуляции
   ----------------------------------------------------------------------- */
void Game::tick()
{
    /*
      Если есть ввод — записать MoveTarget для игрока.
      movementSystem исполнит движение так же, как для NPC.
      Game не меняет Position напрямую.
    */
    if (pending_.hasInput)
    {
        Position* p = world_.getPosition(playerId_);
        if (p)
        {
            world_.addMoveTarget(playerId_, MoveTarget{
                p->x + pending_.dx,
                p->y + pending_.dy
            });
        }
        pending_ = PendingInput{};
    }

    world_.tick(hungerSystem, aiSystem, movementSystem, interactionSystem);
}

/* -----------------------------------------------------------------------
   handleEvent — обработка одного события SFML
   ----------------------------------------------------------------------- */
void Game::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::Closed)
    {
        running_ = false;
        window_.close();
        return;
    }

    if (event.type != sf::Event::KeyPressed)
        return;

    lastInputTime_ = Clock::now();

    switch (event.key.code)
    {
        case sf::Keyboard::W: case sf::Keyboard::Up:
            pending_ = {0, -1, true}; break;

        case sf::Keyboard::S: case sf::Keyboard::Down:
            pending_ = {0, 1, true}; break;

        case sf::Keyboard::A: case sf::Keyboard::Left:
            pending_ = {-1, 0, true}; break;

        case sf::Keyboard::D: case sf::Keyboard::Right:
            pending_ = {1, 0, true}; break;

        case sf::Keyboard::Space:
            paused_ = !paused_;
            std::cout << (paused_ ? "[ПАУЗА]\n" : "[ПРОДОЛЖЕНИЕ]\n");
            break;

        case sf::Keyboard::Q: case sf::Keyboard::Escape:
            running_ = false;
            window_.close();
            break;

        default:
            break;
    }
}

/* -----------------------------------------------------------------------
   run — главный игровой цикл
   ----------------------------------------------------------------------- */
void Game::run()
{
    const unsigned winW = Grid::kWidth  * SFMLRenderer::kCellPx;
    const unsigned winH = Grid::kHeight * SFMLRenderer::kCellPx;

    window_.create(
        sf::VideoMode(winW, winH),
        "ECS Game",
        sf::Style::Close);
    window_.setFramerateLimit(60);

    if (!renderer_.init())
        std::cerr << "Шрифт не найден — символы не будут отображаться\n";

    setupScene();

    while (window_.isOpen() && running_)
    {
        /* Обработка событий SFML (ввод, закрытие окна) */
        sf::Event event;
        while (window_.pollEvent(event))
            handleEvent(event);

        if (!running_)
            break;

        if (!paused_)
        {
            const auto now            = Clock::now();
            const auto sinceLastTick  = std::chrono::duration_cast<Ms>(now - lastTickTime_).count();
            const auto sinceLastInput = std::chrono::duration_cast<Ms>(now - lastInputTime_).count();

            bool doTick = false;

            /* Тик от ввода: pending + прошло достаточно времени */
            if (pending_.hasInput && sinceLastTick >= kInputTickMs)
                doTick = true;

            /* Авто-тик при бездействии */
            if (!pending_.hasInput && sinceLastInput >= kAutoTickMs && sinceLastTick >= kAutoTickMs)
                doTick = true;

            if (doTick)
            {
                tick();
                lastTickTime_ = Clock::now();
            }
        }

        /* Рисуем каждый кадр — состояние мира не меняется до следующего тика */
        renderer_.render(world_, window_);
    }
}
