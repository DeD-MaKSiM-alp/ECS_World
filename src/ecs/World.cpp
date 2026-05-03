#include "ecs/World.h"

/* --- Управление сущностями --- */

EntityId World::createEntity()
{
    return EntityId{nextEntityValue_++};
}

/*
  Удаляет сущность из всех хранилищ.
  Сначала убирает из индекса сетки (если есть позиция), затем из каждого контейнера.
*/
void World::removeEntity(EntityId entity)
{
    auto posIt = positions_.find(entity);
    if (posIt != positions_.end())
    {
        grid_.remove(posIt->second.x, posIt->second.y, entity);
        positions_.erase(posIt);
    }

    velocities_.erase(entity);
    hungers_.erase(entity);
    foods_.erase(entity);
    walkables_.erase(entity);
    blockings_.erase(entity);
    moveTargets_.erase(entity);
    interactTargets_.erase(entity);
    playerControlleds_.erase(entity);
}

/* --- Position --- */

/*
  Позиция сущности — источник правды «где объект».
  grid_ — производный индекс; после записи позиции всегда пересобирается.
*/
void World::addPosition(EntityId entity, Position position)
{
    positions_[entity] = position;
    rebuildGridIndex();
}

Position* World::getPosition(EntityId entity)
{
    auto it = positions_.find(entity);
    if (it == positions_.end())
        return nullptr;
    return &it->second;
}

/*
  Единственная точка смены позиции.
  1) Проверить наличие позиции.
  2) Посчитать новую клетку.
  3) Проверить границы сетки.
  4) Проверить проходимость клетки через Walkable.
  5) Проверить отсутствие Blocking в целевой клетке.
  6) Обновить индекс инкрементально (remove + add).
  7) Сохранить новую позицию.
*/
bool World::moveEntity(EntityId entity, std::int32_t dx, std::int32_t dy)
{
    auto it = positions_.find(entity);
    if (it == positions_.end())
        return false;

    const std::int32_t nx = it->second.x + dx;
    const std::int32_t ny = it->second.y + dy;

    if (!Grid::inBounds(nx, ny))
        return false;

    if (!isCellWalkable(nx, ny))
        return false;

    for (EntityId occupant : grid_.entitiesAt(nx, ny))
    {
        if (blockings_.count(occupant))
            return false;
    }

    grid_.remove(it->second.x, it->second.y, entity);
    grid_.add(nx, ny, entity);

    it->second.x = nx;
    it->second.y = ny;

    return true;
}

/* --- Velocity --- */

void World::addVelocity(EntityId entity, Velocity velocity)
{
    velocities_[entity] = velocity;
}

Velocity* World::getVelocity(EntityId entity)
{
    auto it = velocities_.find(entity);
    if (it == velocities_.end())
        return nullptr;
    return &it->second;
}

/* --- Hunger --- */

void World::addHunger(EntityId entity, Hunger hunger)
{
    hungers_[entity] = hunger;
}

Hunger* World::getHunger(EntityId entity)
{
    auto it = hungers_.find(entity);
    if (it == hungers_.end())
        return nullptr;
    return &it->second;
}

const Hunger* World::getHunger(EntityId entity) const
{
    auto it = hungers_.find(entity);
    if (it == hungers_.end())
        return nullptr;
    return &it->second;
}

void World::removeHunger(EntityId entity)
{
    hungers_.erase(entity);
}

/* --- Food --- */

void World::addFood(EntityId entity)
{
    foods_.insert(entity);
}

bool World::hasFood(EntityId entity) const
{
    return foods_.count(entity) > 0;
}

/* --- Walkable --- */

void World::addWalkable(EntityId entity, Walkable walkable)
{
    walkables_[entity] = walkable;
}

Walkable* World::getWalkable(EntityId entity)
{
    auto it = walkables_.find(entity);
    if (it == walkables_.end())
        return nullptr;
    return &it->second;
}

const Walkable* World::getWalkable(EntityId entity) const
{
    auto it = walkables_.find(entity);
    if (it == walkables_.end())
        return nullptr;
    return &it->second;
}

bool World::isCellWalkable(std::int32_t x, std::int32_t y) const
{
    if (!Grid::inBounds(x, y))
        return false;

    for (EntityId occupant : grid_.entitiesAt(x, y))
    {
        auto it = walkables_.find(occupant);
        if (it != walkables_.end() && !it->second.value)
            return false;
    }
    return true;
}

/* --- Blocking --- */

void World::addBlocking(EntityId entity)
{
    blockings_.insert(entity);
}

bool World::hasBlocking(EntityId entity) const
{
    return blockings_.count(entity) > 0;
}

/* --- MoveTarget --- */

void World::addMoveTarget(EntityId entity, MoveTarget target)
{
    moveTargets_[entity] = target;
}

MoveTarget* World::getMoveTarget(EntityId entity)
{
    auto it = moveTargets_.find(entity);
    if (it == moveTargets_.end())
        return nullptr;
    return &it->second;
}

void World::removeMoveTarget(EntityId entity)
{
    moveTargets_.erase(entity);
}

/* --- InteractTarget --- */

void World::addInteractTarget(EntityId entity, InteractTarget target)
{
    interactTargets_[entity] = target;
}

InteractTarget* World::getInteractTarget(EntityId entity)
{
    auto it = interactTargets_.find(entity);
    if (it == interactTargets_.end())
        return nullptr;
    return &it->second;
}

void World::removeInteractTarget(EntityId entity)
{
    interactTargets_.erase(entity);
}

/* --- PlayerControlled --- */

void World::addPlayerControlled(EntityId entity)
{
    playerControlleds_.insert(entity);
}

bool World::hasPlayerControlled(EntityId entity) const
{
    return playerControlleds_.count(entity) > 0;
}

/* --- Grid index --- */

/*
  Полная пересборка: очистить все клетки, затем один проход по positions_.
  Используется только после addPosition (при расстановке сцены).
  В игровом цикле индекс обновляется инкрементально через moveEntity/removeEntity.
*/
void World::rebuildGridIndex()
{
    grid_.clear();
    for (const auto& entry : positions_)
    {
        const Position& pos = entry.second;
        if (Grid::inBounds(pos.x, pos.y))
            grid_.add(pos.x, pos.y, entry.first);
    }
}
