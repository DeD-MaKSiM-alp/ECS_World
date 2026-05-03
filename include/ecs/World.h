#pragma once

#include "Components.h"
#include "EntityId.h"
#include "Grid.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>

/*
  Центральное хранилище сущностей и компонентов.
  Системы вызываются снаружи через tick() в фиксированном порядке.
  Единственная точка смены позиции — moveEntity().
*/
class World
{
public:
    /* --- Управление сущностями --- */

    EntityId createEntity();

    /*
      Удаляет сущность из всех хранилищ и из индекса сетки.
      Используется InteractionSystem при поедании еды.
    */
    void removeEntity(EntityId entity);

    /* --- Position --- */

    void addPosition(EntityId entity, Position position);
    Position* getPosition(EntityId entity);

    /*
      Единственная точка смены позиции.
      Проверяет границы, проходимость клетки (Walkable) и наличие Blocking в целевой клетке.
      Обновляет positions_ и grid_ инкрементально.
      Возвращает true если перемещение выполнено.
    */
    bool moveEntity(EntityId entity, std::int32_t dx, std::int32_t dy);

    /* --- Velocity (временный компонент-заглушка из коммита 1) --- */

    void addVelocity(EntityId entity, Velocity velocity);
    Velocity* getVelocity(EntityId entity);

    /* --- Hunger --- */

    void addHunger(EntityId entity, Hunger hunger);
    Hunger* getHunger(EntityId entity);
    const Hunger* getHunger(EntityId entity) const;
    void removeHunger(EntityId entity);
    const std::unordered_map<EntityId, Hunger>& hungers() const { return hungers_; }

    /* --- Food --- */

    void addFood(EntityId entity);
    bool hasFood(EntityId entity) const;
    const std::unordered_set<EntityId>& foods() const { return foods_; }

    /* --- Walkable --- */

    void addWalkable(EntityId entity, Walkable walkable);
    Walkable* getWalkable(EntityId entity);
    const Walkable* getWalkable(EntityId entity) const;
    bool isCellWalkable(std::int32_t x, std::int32_t y) const;

    /* --- Blocking --- */

    void addBlocking(EntityId entity);
    bool hasBlocking(EntityId entity) const;

    /* --- MoveTarget --- */

    void addMoveTarget(EntityId entity, MoveTarget target);
    MoveTarget* getMoveTarget(EntityId entity);
    void removeMoveTarget(EntityId entity);
    const std::unordered_map<EntityId, MoveTarget>& moveTargets() const { return moveTargets_; }

    /* --- InteractTarget --- */

    void addInteractTarget(EntityId entity, InteractTarget target);
    InteractTarget* getInteractTarget(EntityId entity);
    void removeInteractTarget(EntityId entity);
    const std::unordered_map<EntityId, InteractTarget>& interactTargets() const { return interactTargets_; }

    /* --- PlayerControlled --- */

    void addPlayerControlled(EntityId entity);
    bool hasPlayerControlled(EntityId entity) const;

    /* --- Grid index --- */

    /* Пересобрать индекс клеток из positions_ (полная пересборка; используется редко). */
    void rebuildGridIndex();
    const Grid& grid() const noexcept { return grid_; }

    /* --- tick --- */

    /*
      Вызывает системы в переданном порядке.
      Каждая система — функция/лямбда вида void(World&).
      Порядок: Hunger → AI → Movement → Interaction.
    */
    template<typename... Fs>
    void tick(Fs&&... systems)
    {
        using expander = int[];
        (void)expander{
            0,
            (std::invoke(std::forward<Fs>(systems), std::ref(*this)), void(), 0)...};
    }

private:
    std::uint32_t nextEntityValue_ = 1;

    std::unordered_map<EntityId, Position>      positions_;
    std::unordered_map<EntityId, Velocity>      velocities_;
    std::unordered_map<EntityId, Hunger>        hungers_;
    std::unordered_set<EntityId>                foods_;
    std::unordered_map<EntityId, Walkable>      walkables_;
    std::unordered_set<EntityId>                blockings_;
    std::unordered_map<EntityId, MoveTarget>    moveTargets_;
    std::unordered_map<EntityId, InteractTarget> interactTargets_;
    std::unordered_set<EntityId>                playerControlleds_;

    Grid grid_;
};
