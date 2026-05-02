#include "ecs/Components.h"
#include "ecs/EntityId.h"
#include "ecs/World.h"

#include <iostream>

int main()
{
    /* Создание нового мира */
    World world;

    /* Создание новой сущности */
    const EntityId entity = world.createEntity();
    /* Если идентификатор сущности невалидный, выводится сообщение об ошибке */
    if (entity == EntityId::invalid())
    {
        std::cerr << "createEntity returned invalid id\n";
        return 1;
    }

    /* Добавление позиции к сущности */
    world.addPosition(entity, Position{0, 0});

    /* Вызов систем */
    world.tick();

    // Системы передаются в мир снаружи; пример — заглушка без логики (вариант B).
    world.tick([](World& w) {
        (void)w;
        // Здесь позже будут вызовы HungerSystem, Movement и т.д.
    });

    /* Получение позиции сущности */
    if (Position* pos = world.getPosition(entity))
    {
        /* Вывод позиции сущности в консоль */
        std::cout << "Position: " << pos->x << ", " << pos->y << '\n';
    }

    /* Возвращение 0 */
    return 0;
}
