#ifndef NPC_H
# define NPC_H

# include "character.h"

# define NPC_SMART      0x00000001
# define NPC_TELEPATH   0x00000002
# define NPC_TUNNELING  0x00000004
# define NPC_ERRATIC    0x00000008
# define NPC_PASS       0x00000010

void add_npcs_to_heap(dungeon_t *d, binheap_t *h);

class Npc : public Character {
 public:
  Npc();
  Npc(const std::string &name, const std::string &desc, const uint8_t speed, const Color color, const char symb, const int ability, const int hp, const Dice &dam);
  int getAttackDamage();
  int ability;
  bool hasLineOfSight;
  int los_x;
  int los_y;
};

#endif
