#include "movement.h"

#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <string>

#include "item.h"
#include "npc.h"
#include "path.h"
#include "pc.h"
#include "user_interface.h"

static void pickUpItem(int x, int y, dungeon_t *d);
static void randomDirection(int *x, int *y, int canTunnel, dungeon_t *d);
static void calculateNextCell_Telepathy(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d);
static void calculateNextCell_Smart(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d);
static void calculateNextCell_LoS(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d);
static void digTunnel(int x, int y, cell_t *c, dungeon_t *d);
static void determineLoS(cell_t *c, dungeon_t *d);
static int moveCharacterFromCellTo(int nextX, int nextY, cell_t *c, dungeon_t *d, binheap_t *h);
static int moveTelepathyCharacter(cell_t *c, dungeon_t *d, binheap_t *h);
static int moveSmartCharacter(cell_t *c, dungeon_t *d, binheap_t *h);
static int moveErraticCharacter(cell_t *c, dungeon_t *d, binheap_t *h);
static int moveBoringCharacter(cell_t *c, dungeon_t *d, binheap_t *h);
static int movePlayerCharacter(cell_t *c, dungeon_t *d, binheap_t *h);

int moveCharacter(cell_t *cell, dungeon_t *d, binheap_t *h)
{
  if (cell->ch->moveNum % 5 == 0) {
    cell->ch->hp *= 1.1; 
    if (cell->ch->hp > cell->ch->maxHp) {
      cell->ch->hp = cell->ch->maxHp;
    }
  }

  if (cell->ch->symbol == DUNGEON_PC) {
    return movePlayerCharacter(cell, d, h);
  }
  Npc *monster = (Npc *) cell->ch;
  
  if (monster->ability & NPC_ERRATIC) {
    return moveErraticCharacter(cell, d, h);
  } else if (monster->ability & NPC_SMART) {
    return moveSmartCharacter(cell, d, h);
  } else if (monster->ability & NPC_TELEPATH) {
    return moveTelepathyCharacter(cell, d, h);
  } else {
    return moveBoringCharacter(cell, d, h);
  }
}

static void pickUpItem(int x, int y, dungeon_t *d)
{
  for (unsigned int i = 0; i < d->player->inventory.capacity(); i++) {
    if (d->player->inventory[i] == NULL) {
      d->player->inventory[i] = d->map[y][x].item;
      std::string output = "Picked up " + d->map[y][x].item->name + ".";
      ui_printMessage(output, 1);
      d->map[y][x].item = NULL;
      d->map[y][x].remembered = DUNGEON_WALL;
      return;
    }
  }
  std::string output = "Cannot pick up " + d->map[y][x].item->name + ".";
  ui_printMessage(output, 1);
}

static bool doBattle(Character *attacker, Character *defender)
{
  int attackDamage = attacker->getAttackDamage();
  defender->hp -= attackDamage;

  std::stringstream ss;
  ss << attacker->name << " hit " << defender->name << " for " << attackDamage;
  ui_printMessage(ss.str(), 0);
  
  if (defender->hp <= 0) {
    return true;     // Defender killed
  } else {
    return false;    // Defender survived
  }
}

static void digTunnel(int x, int y, cell_t *c, dungeon_t *d)
{
  if (d->map[y][x].hardness - 85 <= 0) {
    d->map[y][x].hardness = 0;
    d->map[y][x].o_val = DUNGEON_CORRIDOR;
    d->map[y][x].val = c->ch->symbol;
    d->map[y][x].ch = c->ch;
    d->map[y][x].ch->x = x;
    d->map[y][x].ch->y = y;
    
    c->ch = NULL;
    d->map[c->y][c->x].val = c->o_val;
  } else {
    d->map[y][x].hardness -= 85;
  }
}

static int moveCharacterFromCellTo(int nextX, int nextY, cell_t *c, dungeon_t *d, binheap_t *h)
{
  if (nextX == c->x && nextY == c->y) {
    return 0;
  }
  
  d->map[c->y][c->x].val = c->o_val;
  
  // Check for end game
  if (d->map[nextY][nextX].ch) {
    if (d->map[nextY][nextX].ch->symbol == DUNGEON_PC) {
      if (doBattle(c->ch, d->player)) {
	d->player->alive = false;
	d->map[nextY][nextX].ch = c->ch;
	d->map[nextY][nextX].ch->x = nextX;
	d->map[nextY][nextX].ch->y = nextY;
	d->map[nextY][nextX].val = c->ch->symbol;
	return 2;    // GAME OVER! Player killed!
      } else {
	return 0;
      }      
    } else if (c->ch->symbol == DUNGEON_PC) {
      if (doBattle(d->player, d->map[nextY][nextX].ch)) {
	d->map[nextY][nextX].ch->x = nextX;
	d->map[nextY][nextX].ch->y = nextY;

	d->map[nextY][nextX].ch->alive = false;

	if (d->monsterCount == 0) {
	  d->map[nextY][nextX].ch = c->ch;
	  d->map[nextY][nextX].val = c->ch->symbol;

	  return 1;   //GAME OVER! All monsters killed
	}
      } else {
	return 0;
      }
    } else {
      return 0;
    }
  }

  if (c->ch == d->player && d->map[nextY][nextX].item) {
    pickUpItem(nextX, nextY, d);
  }
    
  d->map[nextY][nextX].ch = c->ch;
  d->map[nextY][nextX].val = c->ch->symbol;
  d->map[nextY][nextX].ch->x = nextX;
  d->map[nextY][nextX].ch->y = nextY;
  c->ch = NULL;
  
  return 0;
}

static int movePlayerCharacter(cell_t *c, dungeon_t *d, binheap_t *h)
{  
  int nextX = c->x;
  int nextY = c->y;
  int move = ui_getPlayerDirection(&nextX, &nextY, d, h);
  
  while(move) {
    if (move == 3) {
      return 3;
    } else if (move == 2) {
      return 0;
    }
    move = ui_getPlayerDirection(&nextX, &nextY, d, h);
  }
  return moveCharacterFromCellTo(nextX, nextY, c, d, h);
}

static int moveErraticCharacter(cell_t *c, dungeon_t *d, binheap_t *h)
{
  Npc *monster = (Npc *) c->ch;

  if (rand() % 2) {
    int rand_x = c->x;
    int rand_y = c->y;
    
    if (monster->ability & NPC_TUNNELING) {
      randomDirection(&rand_x, &rand_y, 1, d);
      if (d->map[rand_y][rand_x].hardness != 0) {
	digTunnel(rand_x, rand_y, c, d);
      } else {
	return moveCharacterFromCellTo(rand_x, rand_y, c, d, h);
      }
    } else {
      randomDirection(&rand_x, &rand_y, 0, d);
      return moveCharacterFromCellTo(rand_x, rand_y, c, d, h);
    }
  } else {
    if (monster->ability & NPC_SMART) {
      return moveSmartCharacter(c, d, h);
    } else if (monster->ability & NPC_TELEPATH) {
      return moveTelepathyCharacter(c, d, h);
    } else {
      return moveBoringCharacter(c, d, h);
    }
  }
  return 0;
}

static int moveSmartCharacter(cell_t *c, dungeon_t *d, binheap_t *h)
{
  int next_x = c->x;
  int next_y = c->y;
  int canTunnel = 0;
  Npc *monster = (Npc *) c->ch;
  
  if (monster->ability & NPC_TUNNELING) {
    generateTunnelingPath(d);
    canTunnel = 1;
  } else {
    generateNonTunnelingPath(d);
    canTunnel = 0;
  }

  if (monster->ability & NPC_TELEPATH) {
    calculateNextCell_Smart(&next_x, &next_y, canTunnel, c, d);
  } else {
    determineLoS(c, d);
    if (monster->hasLineOfSight) {
      calculateNextCell_Smart(&next_x, &next_y, canTunnel, c, d);
    } else if (monster->los_x != 0 && monster->los_y != 0) {
      calculateNextCell_LoS(&next_x, &next_y, canTunnel, c, d);
    } else {
      randomDirection(&next_x, &next_y, canTunnel, d);
    }
  }

  if ((monster->ability & NPC_TUNNELING) && d->map[next_y][next_x].hardness != 0) {
    digTunnel(next_x, next_y, c, d);
  } else {
    return moveCharacterFromCellTo(next_x, next_y, c, d, h);
  }
  
  return 0;
}

static int moveTelepathyCharacter(cell_t *c, dungeon_t *d, binheap_t *h)
{
  int next_x = c->x;
  int next_y = c->y;
  
  if (((Npc *) c->ch)->ability & NPC_TUNNELING) {
    calculateNextCell_Telepathy(&next_x, &next_y, 1, c, d);
    if (d->map[next_y][next_x].hardness != 0) {
      digTunnel(next_x, next_y, c, d);
    } else {
      return moveCharacterFromCellTo(next_x, next_y, c, d, h);
    }
  } else {
    calculateNextCell_Telepathy(&next_x, &next_y, 0, c, d);
    return moveCharacterFromCellTo(next_x, next_y, c, d, h);
  }
  return 0;
}

static int moveBoringCharacter(cell_t *c, dungeon_t *d, binheap_t *h)
{
  int next_x = c->x;
  int next_y = c->y;
  Npc *monster = (Npc *) c->ch;
  
  determineLoS(c, d);

  if (monster->hasLineOfSight) {
    if (monster->ability & NPC_TUNNELING) {
      calculateNextCell_LoS(&next_x, &next_y, 1, c, d);
      if (d->map[next_y][next_x].hardness != 0) {
	digTunnel(next_x, next_y, c, d);
      } else {
	return moveCharacterFromCellTo(next_x, next_y, c, d, h);
      }
    } else {
      calculateNextCell_LoS(&next_x, &next_y, 0, c, d);
      return moveCharacterFromCellTo(next_x, next_y, c, d, h);
    }
  } else {
    if (monster->ability & NPC_TUNNELING) {
      randomDirection(&next_x, &next_y, 1, d);
      if (d->map[next_y][next_x].hardness != 0) {
	digTunnel(next_x, next_y, c, d);
      } else {
	return moveCharacterFromCellTo(next_x, next_y, c, d, h);
      }
    } else {
      randomDirection(&next_x, &next_y, 0, d);
      return moveCharacterFromCellTo(next_x, next_y, c, d, h);
    }
  }
  return 0;
}

static void calculateNextCell_Smart(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d)
{
  int i, j;
  int targetDistance = c->distance - 1;

  if (d->map[*y - 1][*x].distance == targetDistance && d->map[*y - 1][*x].hardness == 0 && (canTunnel || d->map[*y - 1][*x].hardness == 0)) {
    *y = *y - 1;
    return;
  } else if (d->map[*y + 1][*x].distance == targetDistance && d->map[*y + 1][*x].hardness == 0 && (canTunnel || d->map[*y + 1][*x].hardness == 0)) {
    *y = *y + 1;
    return;
  } else if (d->map[*y][*x - 1].distance == targetDistance && d->map[*y][*x - 1].hardness == 0 && (canTunnel || d->map[*y][*x - 1].hardness == 0)) {
    *x = *x - 1;
    return;
  } else if (d->map[*y][*x + 1].distance == targetDistance && d->map[*y][*x + 1].hardness == 0 && (canTunnel || d->map[*y][*x + 1].hardness == 0)) {
    *x = *x + 1;
    return;
  }
  
  for (i = *y - 1; i <= *y + 1; i += 2) {
    for (j = *x - 1; j <= *x + 1; j += 2) {
      if (d->map[i][j].distance == targetDistance && (canTunnel || d->map[i][j].hardness == 0)) {
	*x = j;
	*y = i;
	return;
      }
    }
  }
}

static void calculateNextCell_Telepathy(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d)
{
  int temp_x = *x;
  int temp_y = *y;
  
  if (canTunnel) {
    if (c->x < d->player->x) {
      temp_x += 1;
    } else if (c->x > d->player->x) {
      temp_x -= 1;
    } 

    if (c->y < d->player->y) {
      temp_y += 1;
    } else if (c->y > d->player->y) {
      temp_y -= 1;
    }
  } else {
    if (c->x < d->player->x) {
      temp_x += 1;
    } else if (c->x > d->player->x) {
      temp_x -= 1;
    } 

    if (c->y < d->player->y) {
      temp_y += 1;
    } else if (c->y > d->player->y) {
      temp_y -= 1;
    }

    if (d->map[temp_y][temp_x].hardness != 0) {
      if (c->x < d->player->x && d->map[*y][*x + 1].hardness == 0) {
	temp_x = *x + 1;
	temp_y = *y;
      } else if (c->x > d->player->x && d->map[*y][*x - 1].hardness == 0) {
	temp_x = *x - 1;
	temp_y = *y;
      } else if (c->y < d->player->y && d->map[*y + 1][*x].hardness == 0) {
	temp_x = *x;
	temp_y = *y + 1;
      } else if (c->y > d->player->y && d->map[*y - 1][*x].hardness == 0) {
	temp_x = *x;
	temp_y = *y - 1;
      } else {
	temp_x = c->x;
	temp_y = c->y;
      }
    }
  }
  
  *x = temp_x;
  *y = temp_y;
}

static void calculateNextCell_LoS(int *x, int *y, int canTunnel, cell_t *c, dungeon_t *d)
{
  Npc *monster = (Npc *) c->ch;
  int temp_x = *x;
  int temp_y = *y;
  int los_x = monster->los_x;
  int los_y = monster->los_y;
  
  if (canTunnel) {
    if (c->x < los_x) {
      temp_x += 1;
    } else if (c->x > los_x) {
      temp_x -= 1;
    } 

    if (c->y < los_y) {
      temp_y += 1;
    } else if (c->y > los_y) {
      temp_y -= 1;
    }
  } else {
    if (c->x < los_x) {
      temp_x += 1;
    } else if (c->x > los_x) {
      temp_x -= 1;
    } 

    if (c->y < los_y) {
      temp_y += 1;
    } else if (c->y > los_y) {
      temp_y -= 1;
    }

    if (d->map[temp_y][temp_x].hardness != 0) {
      if (c->x < los_x && d->map[*y][*x + 1].hardness == 0) {
	temp_x = *x + 1;
	temp_y = *y;
      } else if (c->x > los_x && d->map[*y][*x - 1].hardness == 0) {
	temp_x = *x - 1;
	temp_y = *y;
      } else if (c->y < los_y && d->map[*y + 1][*x].hardness == 0) {
	temp_x = *x;
	temp_y = *y + 1;
      } else if (c->y > los_y && d->map[*y - 1][*x].hardness == 0) {
	temp_x = *x;
	temp_y = *y - 1;
      } else {
	*x = c->x;
	*y = c->y;
      }
    }
  }

  *x = temp_x;
  *y = temp_y;

  if (*x == los_x && *y == los_y) {
    monster->los_x = 0;
    monster->los_y = 0;
  }
}

static void determineLoS(cell_t *c, dungeon_t *d)
{
  int i, j, k, sameRoom;
  Npc *monster = (Npc *) c->ch;
  
  monster->hasLineOfSight = false;
  
  for (i = 0; i < d->numRooms; i++) {
    sameRoom = 0;
    for (j = d->rooms[i].y; j < d->rooms[i].height + d->rooms[i].y; j++) {
      for (k = d->rooms[i].x; k < d->rooms[i].width + d->rooms[i].x; k++) {
	if (d->map[j][k].val == DUNGEON_PC || d->map[j][k].val == c->ch->symbol) {
	  sameRoom++;
	}
	if (sameRoom == 2) {
	  monster->hasLineOfSight = true;
	  monster->los_x = d->player->x;
	  monster->los_y = d->player->y;
	  return;
	}
      }
    }
    if (sameRoom == 1) {
      break;
    }
  }

  int temp_x = c->x;
  int temp_y = c->y;
  
  if (c->x == d->player->x) {
    while (temp_y != d->player->y) {
      if (temp_y > d->player->y) {
	temp_y--;
      } else if (temp_y < d->player->y) {
	temp_y++;
      }
      if (d->map[temp_y][c->x].hardness != 0) {
	break;
      }
    }
    if (temp_y == d->player->y) {
      monster->hasLineOfSight = true;
      monster->los_x = d->player->x;
      monster->los_y = d->player->y;
    }
  }

  if (c->y == d->player->y) {
    while (temp_x != d->player->x) {
      if (temp_x > d->player->x) {
	temp_x--;
      } else if (temp_x < d->player->x) {
	temp_x++;
      }
      if (d->map[c->y][temp_x].hardness != 0) {
	break;
      }
    }
    if (temp_x == d->player->x) {
      monster->hasLineOfSight = true;
      monster->los_x = d->player->x;
      monster->los_y = d->player->y;
    }
  }
}

static void randomDirection(int *x, int *y, int canTunnel, dungeon_t *d)
{
  int found = 0;
  int random, old_x, old_y, temp_x, temp_y;

  old_x = *x;
  old_y = *y;
  
  while (!found) {
    temp_x = *x;
    temp_y = *y;
    random = rand() % 3;
    if (random == 2) {
      temp_x += 1;
    } else if (random == 1) {
      temp_x -= 1;
    }
    random = rand() % 3;
    if (random == 2) {
      temp_y += 1;
    } else if (random == 1) {
      temp_y -= 1;
    }

    if (temp_x != 0 && temp_y != 0 && temp_x != (DUNGEON_WIDTH - 1) && temp_y != (DUNGEON_HEIGHT - 1) && (temp_x != old_x || temp_y != old_y)) {
      if (canTunnel) {
	found = 1;
      } else {
	if (d->map[temp_y][temp_x].hardness == 0) {
	  found = 1;
	}
      }
    }
  }
  
  *x = temp_x;
  *y = temp_y;
}
