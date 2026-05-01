#include <iostream>
#include <unordered_map>

using namespace std;

// Структура, в которой хранятся координаты местоположения сущности
struct Position
{
    float x;
    float y;
};

// Структура, в которой хранится скорость передвижения сущности
struct Velocity
{
    float x;
    float y;
};

// Объявдение структуры "Мир" для избежания конфликтов со структурой MovementSystem
struct World;

struct MovementSystem
{
    void update(World& world);
};

struct World
{
    /* 
    Счетчик для создания новых сущностей. Каждая новая сущность получает уникальный номер. 
    Примем, что 0 - это невалидное значение
    */
    int nextEntity = 1;

    /* Хранилище компонентов. Ключевая идея - хранить компоненты отдельно, а не в одной структуре */
    unordered_map<int, Position> positions;
    unordered_map<int, Velocity> velocities;

    /*
    Объявление систем
    */
    MovementSystem movementSystem;

    /* 
    Функция "Создание сущности" 
    - Взяли текущий номер и вернули его как порядковый номер сущности, увеличили текущий номер следующей сущности
    */
    int createEntity()
    {
        int entity = nextEntity;
        nextEntity++;
        return entity;
    }

    /*
    Функция "Добавление компонента позиции"
    - В словаре компонента Position присваиваем номеру сущности позицию из аргумента функции 
    */
    void addPosition(int entity, Position position)
    {
        positions[entity] = position;
    }

    /*
    Функция "Добавление компонента скорости"
    - В словаре компонента Velocity присваиваем номеру сущности позицию из аргумента функции 
    */
    void addVelocity(int entity, Velocity velocity)
    {
        velocities[entity] = velocity;
    }

    /*
    Функция "Запрос компонента Position"
    - Ищет в словаре комопнента Position ключ-порядковый номер сущности
    - Если не находит, возвращает пустой указатель
    - Если находит, возвращает указатель на данные Position
    */
    Position* getPosition(int entity)
    {
        auto it = positions.find(entity);
        if (it == positions.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    /*
    Функция "Запрос компонента Velocity"
    - Ищет в словаре комопнента Velocity ключ-порядковый номер сущности
    - Если не находит, возвращает пустой указатель
    - Если находит, возвращает указатель на данные Velocity
    */
    Velocity* getVelocity(int entity)
    {
        auto it = velocities.find(entity);
        if (it == velocities.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    /*
    Функция "Обновление" - мир теперь отвечает за то, чтобы обновлять себя
    */
    void update()
    {
        movementSystem.update(*this);
    }
};

/*
Реализация функции update структуры MovementSystem, объявленной выше.
Перебирает все сущности, у которых есть компонент Position
Получает ключ-порядковый номер сущности
Получает позицию и скорость
И если скорость не равна пустому указателю - изменить позицию сущности.
*/
void MovementSystem::update(World& world)
{

    for (auto& pair : world.positions)
    {
        int entity = pair.first;
        Position& position = pair.second;
        Velocity* velocity = world.getVelocity(entity);

        if (velocity != nullptr)
        {
            position.x += velocity->x;
            position.y += velocity->y;
        }
    }
}

int main()
{
    World world;

    int entity = world.createEntity();
    world.addPosition(entity, Position{0.0f, 0.0f});
    world.addVelocity(entity, Velocity{1.0f, 2.0f});

    world.update();

    Position* position = world.getPosition(entity);
    if (position != nullptr)
    {
        cout << "Position: " << position->x << ", " << position->y << endl;
    }

    return 0;
}
