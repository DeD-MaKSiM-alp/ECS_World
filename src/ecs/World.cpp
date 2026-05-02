#include "ecs/World.h"

/* Реализация методов World */

/* Создание новой сущности */       
EntityId World::createEntity()
{
    /* 
    Создание нового идентификатора сущности и значение счетчика увеличивается на 1, чтобы следующая сущность получила новый идентификатор
    */
    const EntityId id{nextEntityValue_++};

    /* Возвращение нового идентификатора сущности */
    return id;
}

/*
  Позиция сущности хранится в positions_ — это источник правды «где объект».
  grid_ — вторичный индекс «в этой клетке какие id», чтобы не перебирать всех сущностей,
  когда нужно узнать, кто стоит на клетке (еда, блок, бой и т.д.).
  После записи позиции индекс должен совпадать с positions_; здесь проще всего
  пересобрать его целиком (см. rebuildGridIndex).
  Прямое изменение координат через getPosition() без вызова rebuildGridIndex рассинхронизирует индекс —
  либо вызывайте rebuildGridIndex после таких правок, либо позже единый API движения (например moveEntity).
*/
void World::addPosition(EntityId entity, Position position)
{
    positions_[entity] = position;
    rebuildGridIndex();
}

/*
  Полная пересборка индекса клеток из текущих позиций:
  1) clear — опустошить все списки в клетках, иначе останутся устаревшие id (объект уехал, удалён и т.п.);
  2) один проход по positions_ — заново разложить каждую сущность в клетку по её (x, y).
  Вне сетки [0, kWidth) × [0, kHeight) в индекс не кладём (границы мира обработает позже отдельным слоем).
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

/* Получение позиции сущности */
Position* World::getPosition(EntityId entity)
{
    /* Поиск позиции сущности в хранилище позиций */
    auto it = positions_.find(entity);
    /* Если позиция не найдена, возвращается nullptr */
    if (it == positions_.end())
    {
        return nullptr;
    }
    /* Возвращение позиции сущности */
    /* &it->second - возвращает указатель на значение позиции сущности */
    return &it->second;
}

/* Добавление направления движения к сущности */
void World::addVelocity(EntityId entity, Velocity velocity)
{
    /* Добавление направления движения к сущности в хранилище направлений движения */
    velocities_[entity] = velocity;
}

/* Получение направления движения сущности */
Velocity* World::getVelocity(EntityId entity)
{
    /* Поиск направления движения сущности в хранилище направлений движения */
    auto it = velocities_.find(entity);
    /* Если направление движения не найдено, возвращается nullptr */
    if (it == velocities_.end())
    {
        return nullptr;
    }
    /* Возвращение направления движения сущности */
    return &it->second;
}   