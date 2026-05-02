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

    /* Добавление направления движения к сущности */
    world.addVelocity(entity, Velocity{1, 0});

    /* Вызов систем */
    //world.tick();

    for (int i = 0; i < 10; i++)
    {
        // Системы передаются в мир снаружи; пример — заглушка без логики (вариант B).
        world.tick(
            [entity](World& w) {
            
                if (Velocity* v = w.getVelocity(entity)) {
                    *v = Velocity{1, 0}; // откуда намерение (заглушка ИИ/ввода)
                }
            },

            [entity](World& w) {
                Position* p = w.getPosition(entity);
                Velocity* v = w.getVelocity(entity);

                if(!p || !v) {
                    return;
                }

                p->x += static_cast<std::int32_t>(v->dx);
                p->y += static_cast<std::int32_t>(v->dy);
                *v = Velocity{};


            }
        );

        /* Получение позиции сущности */
        if (Position* pos = world.getPosition(entity))
        {
            /* Вывод позиции сущности в консоль */
            std::cout << "Position: " << pos->x << ", " << pos->y << '\n';
        }
    }    

    /* Возвращение 0 */
    return 0;
}
