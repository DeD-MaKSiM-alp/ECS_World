#pragma once

class World;

/*
  Четыре системы симуляции. Вызываются из World::tick() строго в этом порядке:
  1. hungerSystem    — увеличивает голод всем сущностям с Hunger
  2. aiSystem        — NPC выбирают цель движения или взаимодействия
  3. movementSystem  — шаг к MoveTarget через World::moveEntity
  4. interactionSystem — поедание еды при совпадении клеток

  Каждая система получает только World& и не знает про ввод, рендер или Game.
*/
void hungerSystem(World& w);
void aiSystem(World& w);
void movementSystem(World& w);
void interactionSystem(World& w);
