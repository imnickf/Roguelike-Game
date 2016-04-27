#ifndef DUNGEON_H
  #define DUNGEON_H

typedef struct dungeon dungeon_t;
typedef struct cell cell_t;

class Pc;
class Npc;
class Character;
class Item;

# include <vector>

# include "binheap.h"

# define DUNGEON_WIDTH        80
# define DUNGEON_HEIGHT       21

# define DUNGEON_STAIR_DOWN  (char)'>'
# define DUNGEON_STAIR_UP    (char)'<'
# define DUNGEON_FLOOR       (char)'.'
# define DUNGEON_CORRIDOR    (char)'#'
# define DUNGEON_PC          (char)'@'
# define DUNGEON_WALL        (char)' '

typedef struct room {
  int height;
  int width;
  int x;
  int y;
  int center_x;
  int center_y;
} room_t;

struct cell {
  int x;
  int y;
  int hardness;
  char val;
  char o_val;
  char remembered;
  int distance;
  binheap_node_t *hn;
  Character *ch;
  Item *item;
};

struct dungeon {
  int numRooms;
  room_t *rooms;
  cell_t map[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  Pc *player;
  std::vector<Npc *> monsters;
  std::vector<Item *> items;
  int monsterCount;
};

void dungeon_init(dungeon_t *dungeon);
int generateDungeon(dungeon_t *dungeon);
int loadDungeon(dungeon_t *dungeon, char* filepath);
int saveDungeon(dungeon_t *dungeon, char* filepath);
int printDungeon(dungeon_t *dungeon);
void dungeon_delete(dungeon_t *dungeon);

#endif
