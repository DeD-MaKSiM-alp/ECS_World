#include "ecs/Systems.h"
#include "ecs/World.h"

#include <cstdlib>
#include <limits>

/* -----------------------------------------------------------------------
   Константы симуляции
   ----------------------------------------------------------------------- */

/* Порог голода: при hunger.value >= kHungerThreshold AI начинает искать еду */
static constexpr int kHungerThreshold = 5;

/* На сколько снижается голод при поедании еды */
static constexpr int kFoodHungerReduction = 3;

/* -----------------------------------------------------------------------
   Вспомогательные функции
   ----------------------------------------------------------------------- */

/* Знак числа: -1, 0 или 1 */
static int sign(std::int32_t n)
{
    if (n > 0) return  1;
    if (n < 0) return -1;
    return 0;
}

/* Манхэттенское расстояние между двумя точками */
static int manhattan(std::int32_t ax, std::int32_t ay, std::int32_t bx, std::int32_t by)
{
    return std::abs(ax - bx) + std::abs(ay - by);
}

/* -----------------------------------------------------------------------
   Коммит 5: HungerSystem
   Каждый тик увеличивает hunger.value у всех сущностей с компонентом Hunger.
   Система не знает о карте — только читает/пишет компонент.
   ----------------------------------------------------------------------- */
void hungerSystem(World& w)
{
    /* Собираем id отдельно, чтобы безопасно менять значения через getHunger */
    std::vector<EntityId> ids;
    for (const auto& [id, _] : w.hungers())
        ids.push_back(id);

    for (EntityId id : ids)
    {
        Hunger* h = w.getHunger(id);
        if (h)
            h->value++;
    }
}

/* -----------------------------------------------------------------------
   Коммит 6: AISystem
   Для каждого NPC (есть Hunger, нет PlayerControlled):
   - Если голод < порога — ничего не делать.
   - Найти ближайшую еду (Manhattan, tie-break по меньшему id).
   - Если NPC уже стоит на клетке с едой — снять MoveTarget, выставить InteractTarget.
   - Иначе — выставить MoveTarget на еду, снять InteractTarget.
   ----------------------------------------------------------------------- */
void aiSystem(World& w)
{
    for (auto& [npcId, hunger] : w.hungers())
    {
        /* Пропускаем игрока — он управляется вводом */
        if (w.hasPlayerControlled(npcId))
            continue;

        if (hunger.value < kHungerThreshold)
            continue;

        Position* npcPos = w.getPosition(npcId);
        if (!npcPos)
            continue;

        /* Поиск ближайшей еды */
        EntityId bestFood  = EntityId::invalid();
        int      bestDist  = std::numeric_limits<int>::max();

        for (EntityId foodId : w.foods())
        {
            Position* foodPos = w.getPosition(foodId);
            if (!foodPos)
                continue;

            const int dist = manhattan(npcPos->x, npcPos->y, foodPos->x, foodPos->y);

            /* Tie-break: меньший EntityId при равном расстоянии */
            if (dist < bestDist || (dist == bestDist && foodId.value < bestFood.value))
            {
                bestDist = dist;
                bestFood = foodId;
            }
        }

        if (bestFood == EntityId::invalid())
        {
            /* Целей еды нет: не оставляем устаревшие намерения */
            w.removeMoveTarget(npcId);
            w.removeInteractTarget(npcId);
            continue;
        }

        Position* foodPos = w.getPosition(bestFood);

        /* NPC на клетке с едой — взаимодействие */
        if (npcPos->x == foodPos->x && npcPos->y == foodPos->y)
        {
            w.removeMoveTarget(npcId);
            w.addInteractTarget(npcId, InteractTarget{bestFood});
        }
        else
        {
            /* NPC идёт к еде */
            w.addMoveTarget(npcId, MoveTarget{foodPos->x, foodPos->y});
            w.removeInteractTarget(npcId);
        }
    }
}

/* -----------------------------------------------------------------------
   Коммит 7: MovementSystem
   Для каждой сущности с MoveTarget делает один шаг через World::moveEntity.
   Работает одинаково для NPC и игрока — никакой разницы в логике.
   Приоритет оси: сначала X, потом Y.
   ----------------------------------------------------------------------- */
void movementSystem(World& w)
{
    /*
      Итерируемся по копии ключей, потому что removeMoveTarget меняет контейнер.
      Прямая итерация по moveTargets() во время удаления — неопределённое поведение.
    */
    std::vector<EntityId> toMove;
    for (const auto& [id, _] : w.moveTargets())
        toMove.push_back(id);

    for (EntityId id : toMove)
    {
        MoveTarget* target = w.getMoveTarget(id);
        if (!target)
            continue;

        Position* pos = w.getPosition(id);
        if (!pos)
            continue;

        const int dx = sign(target->x - pos->x);
        const int dy = sign(target->y - pos->y);

        bool moved = false;
        if (dx != 0)
        {
            moved = w.moveEntity(id, dx, 0);
            if (!moved && dy != 0)
                moved = w.moveEntity(id, 0, dy);
        }
        else if (dy != 0)
        {
            moved = w.moveEntity(id, 0, dy);
        }

        Position* newPos = w.getPosition(id);
        if (newPos && newPos->x == target->x && newPos->y == target->y)
            w.removeMoveTarget(id);
        else if (!moved)
            w.removeMoveTarget(id);
    }
}

/* -----------------------------------------------------------------------
   Коммит 8: InteractionSystem
   Для каждой сущности с InteractTarget:
   - Проверить, что актор и цель стоят в одной клетке.
   - Если цель — еда: уменьшить голод, удалить еду из мира, снять InteractTarget.
   ----------------------------------------------------------------------- */
void interactionSystem(World& w)
{
    std::vector<EntityId> toInteract;
    for (const auto& [id, _] : w.interactTargets())
        toInteract.push_back(id);

    for (EntityId actorId : toInteract)
    {
        InteractTarget* interact = w.getInteractTarget(actorId);
        if (!interact)
            continue;

        const EntityId targetId = interact->target;

        Position* actorPos  = w.getPosition(actorId);
        Position* targetPos = w.getPosition(targetId);

        if (!actorPos || !targetPos)
        {
            /* Цель уже удалена или нет позиции — снять намерение */
            w.removeInteractTarget(actorId);
            continue;
        }

        /* Взаимодействие только если стоим в одной клетке */
        if (actorPos->x != targetPos->x || actorPos->y != targetPos->y)
            continue;

        /* Цель — еда */
        if (w.hasFood(targetId))
        {
            Hunger* hunger = w.getHunger(actorId);
            if (hunger)
            {
                hunger->value -= kFoodHungerReduction;
                if (hunger->value < 0)
                    hunger->value = 0;
            }

            w.removeEntity(targetId);
            w.removeInteractTarget(actorId);
        }
    }
}
