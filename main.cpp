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

    /*
      Дополнительные сущности без Velocity — они не участвуют в лямбдах tick(), их позиции не меняются.

      Зачем они здесь:
      - Индекс сетки (grid_) должен знать про всех, у кого есть Position. Если проверять только одну
        движущуюся сущность, ошибка в пересборке могла бы затронуть «чужие» клетки, а тест этого не заметил бы.
      - Две разные клетки (5,10) и (19,19) дают простую проверку: в списке каждой клетки должен лежать
        ровно нужный id; углы/край поля иногда ловят отдельные баги с границами сетки.

      createEntity() выдаёт новый id, addPosition() записывает координаты в positions_ и вызывает
      rebuildGridIndex() — неподвижные сущности сразу попадают в правильные клетки индекса.
    */
    const EntityId stationaryA = world.createEntity();
    world.addPosition(stationaryA, Position{5, 10});
    const EntityId stationaryB = world.createEntity();
    world.addPosition(stationaryB, Position{19, 19});

    /* Вызов систем */
    //world.tick();

    for (int i = 0; i < 10; i++)
    {
        /* Системы передаются в мир снаружи; пример — заглушка без логики (вариант B). */
        world.tick(
            [entity](World& w) {
            
                if (Velocity* v = w.getVelocity(entity)) {
                    *v = Velocity{1, 0}; /* откуда намерение (заглушка ИИ/ввода) */
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

        /*
          Вторая лямбда меняет координаты через указатель из getPosition — World об этом не узнаёт автоматически,
          поэтому индекс клеток после tick() может расходиться с positions_, пока мы его не пересоберём.

          rebuildGridIndex() заново строит grid_ только из актуальных Position: очищает клетки и снова
          раскладывает id по (x,y). Тогда grid и компоненты снова говорят одно и то же.
        */
        world.rebuildGridIndex();

        /*
          Проверка инварианта для начального разработчика:
          для каждого id с позицией (x,y) этот id обязан присутствовать в списке сущностей клетки (x,y) в grid.

          gridMatches(id):
          - читает позицию из источника правды (getPosition);
          - берёт список id в этой клетке через grid().entitiesAt(x,y);
          - если наш id есть в списке — индекс согласован с Position для этой сущности.

          lambda захватывает world по ссылке [&world]: без этого внутри не было бы доступа к текущему миру.

          Проверяем все три сущности: движущуюся и две неподвижные — чтобы не пропустить ошибку только на одном id.
        */
        auto gridMatches = [&world](EntityId id) -> bool {
            Position* p = world.getPosition(id);
            if (!p)
                return false;
            for (EntityId e : world.grid().entitiesAt(p->x, p->y))
            {
                if (e == id)
                    return true;
            }
            return false;
        };
        if (!gridMatches(entity) || !gridMatches(stationaryA) || !gridMatches(stationaryB))
        {
            std::cerr << "grid index mismatch\n";
            return 1;
        }

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
