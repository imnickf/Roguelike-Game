#include "character.h"

#include <cstdlib>

void Character::placeCharacter(dungeon_t *d)
{
  int placed = 0;
  int x, y;

  while (!placed) {
    x = rand() % DUNGEON_WIDTH;
    y = rand() % DUNGEON_HEIGHT;
    if (d->map[y][x].val == DUNGEON_FLOOR) {
      d->map[y][x].val = this->symbol;
      d->map[y][x].ch = this;
      this->x = x;
      this->y = y;
      placed = 1;
    }
  }
}
