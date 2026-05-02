#pragma once

#include "EntityId.h"

#include <array>
#include <cstdint>
#include <vector>

/* Индекс по клеткам: только хранение списков id, без игровой логики. */
class Grid
{
public:
    /* Ширина сетки */
    static constexpr std::int32_t kWidth = 20;
    /* Высота сетки */
    static constexpr std::int32_t kHeight = 20;

    /* Проверка, находится ли точка в границах сетки */
    static constexpr bool inBounds(std::int32_t x, std::int32_t y) noexcept
    {
        return x >= 0 && x < kWidth && y >= 0 && y < kHeight;
    }

    /*
    Очистка сетки
    -auto& row : cells_ - цикл по строкам сетки
    -for (auto& cell : row) - цикл по клеткам строки
    -cell.clear() - очистка клетки
    Нужна при полной пересборке, чтобы не тащить в новый индекс мусор из прошлого кадра
    !! ВАЖНО !! БУДЕТ УЗКИМ ГОРЛЫШКОМ ПРИ БОЛЬШОЙ КАРТЕ
    */
    void clear()
    {
        for (auto& row : cells_)
            for (auto& cell : row)
                cell.clear();
    }

    /*
    Добавление сущности в клетку
    -x - координата x
    -y - координата y
    -id - идентификатор сущности
    -if (!inBounds(x, y)) - проверка, находится ли точка в границах сетки
    -cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)].push_back(id) - добавление идентификатора сущности в клетку
    Нужно при полной пересборке, чтобы не тащить в новый индекс мусор из прошлого кадра
    */
    void add(std::int32_t x, std::int32_t y, EntityId id)
    {
        if (!inBounds(x, y))
            return;
        cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)].push_back(id);
    }

    /*
    Удаление сущности из клетки
    -x - координата x
    -y - координата y
    -id - идентификатор сущности
    -if (!inBounds(x, y)) - проверка, находится ли точка в границах сетки
    -auto& cell = cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] - получение клетки
    -for (auto it = cell.begin(); it != cell.end(); ++it) - цикл по клетке
    -if (*it == id) - проверка, является ли идентификатор сущности текущим элементом
    -cell.erase(it) - удаление идентификатора сущности из клетки
    Требуется для удаления сущности из клетки, чтобы не тащить в новый индекс мусор из прошлого кадра
    */
    void remove(std::int32_t x, std::int32_t y, EntityId id)
    {
        if (!inBounds(x, y))
            return;
        auto& cell = cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
        for (auto it = cell.begin(); it != cell.end(); ++it)
        {
            if (*it == id)
            {
                cell.erase(it);
                return;
            }
        }
    }

    /*
    Получение списка сущностей в клетке
    -x - координата x
    -y - координата y
    -if (!inBounds(x, y)) - проверка, находится ли точка в границах сетки
    -return cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] - возвращение списка сущностей в клетке
    Требуется для получения списка сущностей в клетке
    */
    const std::vector<EntityId>& entitiesAt(std::int32_t x, std::int32_t y) const
    {
        static const std::vector<EntityId> kEmpty;
        if (!inBounds(x, y))
            return kEmpty;
        return cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
    }

private:
    /* Тип клетки - вектор идентификаторов сущностей */
    using Cell = std::vector<EntityId>;
    /* Массив клеток - массив массивов клеток */
    std::array<std::array<Cell, kWidth>, kHeight> cells_{};
};
