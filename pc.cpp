#include "pc.h"

#include <cstdlib>
#include <iostream>

#include "dungeon.h"
#include "item.h"

void Pc::swapItems(int equipIndex, int invIndex)
{
  Item *temp = equipment[equipIndex];
  equipment[equipIndex] = inventory[invIndex];
  inventory[invIndex] = temp;
}

void Pc::equipItem(Item *item, int equipIndex, int invIndex)
{
  if (equipment[equipIndex]) {
      hp -= equipment[equipIndex]->defense;
      speed -= equipment[equipIndex]->speed;
      swapItems(equipIndex, invIndex);
    } else {
      equipment[equipIndex] = item;
      inventory[invIndex] = NULL;
    }
    hp += equipment[equipIndex]->defense;
    speed += equipment[equipIndex]->speed;
    if (speed <= 0) {
      speed = 1;
    }

    if (item->type == itemtype_LIGHT) {
      viewDistance = 5;
    }
}

Pc::Pc()
{
  name = "Player";
  description = "The player character duh...";
  symbol = DUNGEON_PC;
  color = WHITE;
  turn = 0;
  moveNum = 0;
  sequence = 0;
  alive = true;
  speed = 10;
  hp = 200;
  maxHp = 200;
  viewDistance = 3;
  damage = Dice(5, 1, 4);
  equipment.reserve(12);
  inventory.reserve(10);
}

int Pc::getAttackDamage()
{
  int damageTotal = damage.roll();

  for (unsigned int i = 0; i < equipment.capacity(); i++) {
    if (equipment[i]) {
      damageTotal += equipment[i]->damage.roll();
    }
  }
  
  return damageTotal;
}

void Pc::equipItem(Item *item, int invIndex)
{
  switch (item->type) {
  case itemtype_WEAPON:
    equipItem(item, 0, invIndex);
    break;
  case itemtype_OFFHAND:
    equipItem(item, 1, invIndex);
    break;
  case itemtype_RANGED:
    equipItem(item, 2, invIndex);
    break;
  case itemtype_ARMOR:
    equipItem(item, 3, invIndex);
    break;
  case itemtype_HELMET:
    equipItem(item, 4, invIndex);
    break;
  case itemtype_CLOAK:
    equipItem(item, 5, invIndex);
    break;
  case itemtype_GLOVES:
    equipItem(item, 6, invIndex);
    break;
  case itemtype_BOOTS:
    equipItem(item, 7, invIndex);
    break;
  case itemtype_AMULET:
    equipItem(item, 8, invIndex);
    break;
  case itemtype_LIGHT:
    equipItem(item, 9, invIndex);
    break;
  case itemtype_RING:
    if (equipment[10] && !equipment[11]) {
      equipItem(item, 11, invIndex);
    } else if (!equipment[10] && equipment[11]) {
      equipItem(item, 10, invIndex);
    } else {
      equipItem(item, 11, invIndex);
    }
    break;
  default:
    break;
  }
}

bool Pc::takeOffItem(Item *item, int equipIndex)
{
  for (unsigned int i = 0; i < inventory.capacity(); i++) {
    if (!inventory[i]) {
      hp -= item->defense;
      equipment[equipIndex] = NULL;
      inventory[i] = item;
      return true;
    }
  }
  return false;
}
