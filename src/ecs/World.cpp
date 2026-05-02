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

/* Добавление позиции к сущности */
void World::addPosition(EntityId entity, Position position)
{
    /* Добавление позиции к сущности в хранилище позиций */
    positions_[entity] = position;
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