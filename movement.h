#ifndef MOVEMENT_H
  #define MOVEMENT_H

#include "dungeon.h"
#include "binheap.h"

#define NPC_SMART      0x00000001
#define NPC_TELEPATH   0x00000002
#define NPC_TUNNELING  0x00000004
#define NPC_ERRATIC    0x00000008

int moveCharacter(cell_t *cell, dungeon_t *d, binheap_t *h);

#endif
