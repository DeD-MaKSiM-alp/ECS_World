#include "SmokeTests.h"

#include "ecs/Components.h"
#include "ecs/EntityId.h"
#include "ecs/Systems.h"
#include "ecs/World.h"

#include <iostream>

namespace
{
/* -----------------------------------------------------------------------
   Вспомогательная проверка теста.
   Если условие не выполнено — печатает FAIL и останавливает прогон.
   ----------------------------------------------------------------------- */
bool check(bool cond, const char* msg, std::ostream& out, std::ostream& err)
{
    if (!cond) {
        err << "FAIL: " << msg << '\n';
        return false;
    }

    out << "OK:   " << msg << '\n';
    return true;
}

/* -----------------------------------------------------------------------
   Тест коммита 2: индекс Grid совпадает с Position
   ----------------------------------------------------------------------- */
bool testGridIndex()
{
    World w;

    const EntityId a = w.createEntity();
    w.addPosition(a, Position{5, 10});
    const EntityId b = w.createEntity();
    w.addPosition(b, Position{19, 19});

    auto inGrid = [&](EntityId id) -> bool {
        Position* p = w.getPosition(id);
        if (!p) return false;
        for (EntityId e : w.grid().entitiesAt(p->x, p->y))
            if (e == id) return true;
        return false;
    };

    return inGrid(a) && inGrid(b);
}

/* -----------------------------------------------------------------------
   Тест коммита 3: moveEntity — движение, граница, блокировка
   ----------------------------------------------------------------------- */
bool testMoveEntity()
{
    World w;

    const EntityId e = w.createEntity();
    w.addPosition(e, Position{0, 0});

    /* 5 шагов вправо */
    for (int i = 1; i <= 5; ++i)
    {
        bool moved = w.moveEntity(e, 1, 0);
        if (!moved) return false;
        Position* p = w.getPosition(e);
        if (!p || p->x != i || p->y != 0) return false;
        /* Проверить что старая клетка пустая */
        if (!w.grid().entitiesAt(i - 1, 0).empty()) return false;
        /* Проверить что новая клетка содержит e */
        bool found = false;
        for (EntityId id : w.grid().entitiesAt(i, 0)) if (id == e) found = true;
        if (!found) return false;
    }

    /* Граница: на (19,0) двигаться вправо нельзя */
    const EntityId edge = w.createEntity();
    w.addPosition(edge, Position{19, 0});
    if (w.moveEntity(edge, 1, 0)) return false;         /* должен вернуть false */
    if (w.getPosition(edge)->x != 19) return false;     /* позиция не изменилась */

    /* Блокировка */
    const EntityId blocker = w.createEntity();
    w.addPosition(blocker, Position{3, 3});
    w.addBlocking(blocker);
    const EntityId mover = w.createEntity();
    w.addPosition(mover, Position{2, 3});
    if (w.moveEntity(mover, 1, 0)) return false;        /* клетка (3,3) заблокирована */

    return true;
}

/* -----------------------------------------------------------------------
   Тест коммита 5: HungerSystem — голод растёт каждый тик
   ----------------------------------------------------------------------- */
bool testHungerSystem()
{
    World w;
    const EntityId npc = w.createEntity();
    w.addHunger(npc, Hunger{0});

    for (int i = 1; i <= 5; ++i)
    {
        w.tick(hungerSystem);
        Hunger* h = w.getHunger(npc);
        if (!h || h->value != i) return false;
    }
    return true;
}

/* -----------------------------------------------------------------------
   Тест коммита 6: AISystem — NPC выставляет MoveTarget к еде
   ----------------------------------------------------------------------- */
bool testAiSystem()
{
    World w;

    const EntityId npc = w.createEntity();
    w.addPosition(npc, Position{0, 0});
    w.addHunger(npc, Hunger{10});    /* голод выше порога */

    const EntityId food = w.createEntity();
    w.addPosition(food, Position{3, 4});
    w.addFood(food);

    w.tick(aiSystem);

    /* NPC должен получить MoveTarget на (3,4) */
    MoveTarget* mt = w.getMoveTarget(npc);
    if (!mt || mt->x != 3 || mt->y != 4) return false;
    /* InteractTarget не должен быть выставлен */
    if (w.getInteractTarget(npc)) return false;

    /* Поставить NPC на клетку с едой — должен выставить InteractTarget */
    World w2;
    const EntityId npc2 = w2.createEntity();
    w2.addPosition(npc2, Position{3, 4});
    w2.addHunger(npc2, Hunger{10});
    const EntityId food2 = w2.createEntity();
    w2.addPosition(food2, Position{3, 4});
    w2.addFood(food2);

    w2.tick(aiSystem);

    if (w2.getMoveTarget(npc2)) return false;
    InteractTarget* it = w2.getInteractTarget(npc2);
    if (!it || !(it->target == food2)) return false;

    return true;
}

/* -----------------------------------------------------------------------
   Тест коммита 7: MovementSystem — шаг к MoveTarget
   ----------------------------------------------------------------------- */
bool testMovementSystem()
{
    World w;
    const EntityId e = w.createEntity();
    w.addPosition(e, Position{0, 0});
    w.addMoveTarget(e, MoveTarget{3, 0});

    /* 3 тика — должны прийти в (3,0) */
    for (int i = 1; i <= 3; ++i)
    {
        w.tick(movementSystem);
        Position* p = w.getPosition(e);
        if (!p || p->x != i || p->y != 0) return false;
    }
    /* MoveTarget должен быть снят */
    if (w.getMoveTarget(e)) return false;

    return true;
}

/* -----------------------------------------------------------------------
   Тест коммита 8: InteractionSystem — съесть еду
   ----------------------------------------------------------------------- */
bool testInteractionSystem()
{
    World w;

    const EntityId npc = w.createEntity();
    w.addPosition(npc, Position{2, 2});
    w.addHunger(npc, Hunger{8});
    const EntityId food = w.createEntity();
    w.addPosition(food, Position{2, 2});
    w.addFood(food);
    w.addInteractTarget(npc, InteractTarget{food});

    w.tick(interactionSystem);

    /* Еда удалена */
    if (w.getPosition(food)) return false;
    if (w.hasFood(food)) return false;
    /* Голод снизился (8 - 3 = 5) */
    Hunger* h = w.getHunger(npc);
    if (!h || h->value != 5) return false;
    /* InteractTarget снят */
    if (w.getInteractTarget(npc)) return false;

    return true;
}
} // namespace

bool runSmokeTests(std::ostream& out, std::ostream& err)
{
    out << "=== Smoke tests ===\n";

    if (!check(testGridIndex(), "Grid index совпадает с Position", out, err)) return false;
    if (!check(testMoveEntity(), "moveEntity: движение, граница, блокировка", out, err)) return false;
    if (!check(testHungerSystem(), "HungerSystem: голод растёт на 1 за тик", out, err)) return false;
    if (!check(testAiSystem(), "AISystem: MoveTarget и InteractTarget", out, err)) return false;
    if (!check(testMovementSystem(), "MovementSystem: шаг к MoveTarget", out, err)) return false;
    if (!check(testInteractionSystem(), "InteractionSystem: съесть еду", out, err)) return false;

    out << "\n=== Все тесты пройдены. Запуск игры... ===\n\n";
    return true;
}
