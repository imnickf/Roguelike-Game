#include "npc.h"

#include <cstdlib>
#include <cstdio>

#include "dungeon.h"

static int randomAbilities();

Npc::Npc()
{
  ability = randomAbilities();
  hasLineOfSight = false;
  alive = true;
  speed = (rand() % 16 + 5);
  turn = 0;
  los_x = 0;
  los_y = 0;
  sprintf(&symbol, "%X", ability);
}

Npc::Npc(const std::string &name, const std::string &desc, const uint8_t speed, const Color color, const char symb, const int ability, const int hp, const Dice &dam)
{
  this->name = name;
  this->description = desc,
  this->color = color;
  this->symbol = symb;
  this->speed = speed;
  this->ability = ability;
  this->hp = hp;
  this->maxHp = hp;
  this->damage = dam;
  
  alive = true;
  hasLineOfSight = false;
  moveNum = 0;
  turn = 0;
  los_x = 0;
  los_y = 0;
}

int Npc::getAttackDamage()
{
  return damage.roll();
}

static int randomAbilities()
{
  int abilities = 0;

  if (rand() % 2) {
    abilities |= NPC_SMART;
  }
  if (rand() % 2) {
    abilities |= NPC_TELEPATH;
  }
  if (rand() % 2) {
    abilities |= NPC_TUNNELING;
  }
  if (rand() % 2) {
    abilities |= NPC_ERRATIC;
  }
  return abilities;
}

void add_npcs_to_heap(dungeon_t *d, binheap_t *h)
{
  int i;
  
  for (i = 0; i < d->monsterCount; i++) {
    Npc *toAdd = d->monsters[i];
    toAdd->hn = binheap_insert(h, toAdd);
  }
}
