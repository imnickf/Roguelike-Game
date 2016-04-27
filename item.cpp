#include "item.h"

#include <cstdlib>

Item::Item(const std::string &name, const std::string &desc, const item_type_t type, const char symbol, const Color color, const Dice &damage, const int hit, const int dodge, const int defense, const int weight, const int speed, const int attribute, const int value)
{
  this->name = name;
  this->description = desc;
  this->type = type;
  this->symbol = symbol;
  this->color = color;
  this->damage = damage;
  this->hit = hit;
  this->dodge = dodge;
  this->defense = defense;
  this->weight = weight;
  this->speed = speed;
  this->attribute = attribute;
  this->value = value;
}

void Item::placeItem(dungeon_t *d)
{
  bool placed = false;
  int x, y;
  
  while (!placed) {
    x = rand() % DUNGEON_WIDTH;
    y = rand() % DUNGEON_HEIGHT;
    if (d->map[y][x].val == DUNGEON_FLOOR && d->map[y][x].item == NULL) {
      d->map[y][x].item = this;
      placed = true;
    }
  }
}
