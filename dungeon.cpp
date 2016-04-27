#include "dungeon.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <machine/endian.h>
#include <math.h>

#include "endian.h"
#include "descriptions.h"
#include "item.h"
#include "item_parser.h"
#include "monster_parser.h"
#include "npc.h"
#include "pc.h"

#define NUM_ROOMS 7

int i, j, k;

static int createNewRoom(dungeon_t *dungeon, int count)
{
  int h = (rand() % 12) + 2;
  int w = (rand() % 12) + 3;
  int x = (rand() % 73) + 1;
  int y = (rand() % 17) + 1;

  if (y + h > 19 || x + w > 78) {
    return -1;
  } else {
    for (i = y - 1; i <= y + h; i++) {
      for (j = x - 1; j <= x + w; j++) {
	if (dungeon->map[i][j].val == DUNGEON_FLOOR) {
	  return -1;
	}
      }
    }
  }

  for (i = y; i < y + h; i++) {
    for (j = x; j < x + w; j++) {
      dungeon->map[i][j].val = DUNGEON_FLOOR;
      dungeon->map[i][j].o_val = DUNGEON_FLOOR;
      dungeon->map[i][j].hardness = 0;
    }
  }
 
  dungeon->rooms[count].height = h;
  dungeon->rooms[count].width = w;
  dungeon->rooms[count].x = x;
  dungeon->rooms[count].y = y;
  dungeon->rooms[count].center_x = x + (w / 2);
  dungeon->rooms[count].center_y = y + (h / 2);

  return 0;
}

static int distance(dungeon_t *dungeon, int room1, int room2)
{  
  int x1 = dungeon->rooms[room1].center_x;
  int y1 = dungeon->rooms[room1].center_y;
  int x2 = dungeon->rooms[room2].center_x;
  int y2 = dungeon->rooms[room2].center_y;
  
  return (int)pow((double)(x2 - x1), 2) + (int)pow((double)(y2 - y1), 2);
}

static int nearestRoom(dungeon_t *dungeon, int numConnected)
{
  int closest = 0;
  for (i = 1; i < numConnected; i++) {
    if (distance(dungeon, i, numConnected) > distance(dungeon, i - 1, numConnected)) {
      closest = i;
    }
  }

  return closest; 
}

static int connectRooms(dungeon_t *dungeon, int room1, int room2)
{
  int currentX = dungeon->rooms[room1].center_x;
  int currentY = dungeon->rooms[room1].center_y;
  int targetX = dungeon->rooms[room2].center_x;
  int targetY = dungeon->rooms[room2].center_y;
  
  while (currentX != targetX) {
    if (dungeon->map[currentY][currentX].val != DUNGEON_FLOOR) {
      dungeon->map[currentY][currentX].val = DUNGEON_CORRIDOR;
      dungeon->map[currentY][currentX].o_val = DUNGEON_CORRIDOR;
      dungeon->map[currentY][currentX].hardness = 0;
    }
    
    if (currentX < targetX) {
      currentX++;
    } else {
      currentX--;
    }
  }

  while (currentY != targetY) {
    if (dungeon->map[currentY][currentX].val != DUNGEON_FLOOR) {
      dungeon->map[currentY][currentX].val = DUNGEON_CORRIDOR;
      dungeon->map[currentY][currentX].o_val = DUNGEON_CORRIDOR;
      dungeon->map[currentY][currentX].hardness = 0;
    }
    
    if (currentY < targetY) {
      currentY++;
    } else {
      currentY--;
    }
  }

  return 0;
}

static void placeStairs(dungeon_t *dungeon, char direction)
{
  int placed = 0;
  int x, y;

  while (!placed) {
    x = rand() % DUNGEON_WIDTH;
    y = rand() % DUNGEON_HEIGHT;
    if (dungeon->map[y][x].val == DUNGEON_FLOOR) {
      dungeon->map[y][x].val = direction;
      dungeon->map[y][x].o_val = direction;
      placed = 1;
    }
  }
}

void dungeon_init(dungeon_t *dungeon)
{
  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      dungeon->map[i][j].val = DUNGEON_WALL;
      dungeon->map[i][j].o_val = DUNGEON_WALL;
      dungeon->map[i][j].x = j;
      dungeon->map[i][j].y = i;
      dungeon->map[i][j].ch = NULL;
      dungeon->map[i][j].item = NULL;
      dungeon->map[i][j].remembered = DUNGEON_WALL;
      if (i == 0 || j == 0 || j == (DUNGEON_WIDTH - 1) || i == (DUNGEON_HEIGHT - 1)) {
	dungeon->map[i][j].hardness = 255;
      } else {
	dungeon->map[i][j].hardness = (rand() % 254) + 1;
      }
    }
  }
}


int generateDungeon(dungeon_t *dungeon)
{
  int count = 0;
  int added, nearest;

  int fp_length = strlen(getenv("HOME")) + 30;
  
  dungeon->rooms = (room_t *) malloc(sizeof(room_t) * NUM_ROOMS);
  dungeon->numRooms = NUM_ROOMS;
  
  // Create rooms and add to dungeon
  while (count < dungeon->numRooms) {
    added = createNewRoom(dungeon, count);

    if (added == 0) {
      count++;
    }
  }

  // Connect Rooms
  connectRooms(dungeon, 0, 1);
  
  for (i = 2; i < dungeon->numRooms; i++) {
    nearest = nearestRoom(dungeon, i);
    connectRooms(dungeon, nearest, i);
  }

  // Place Staircases
  placeStairs(dungeon, DUNGEON_STAIR_UP);
  placeStairs(dungeon, DUNGEON_STAIR_DOWN);

  // Create and place PC
  dungeon->player = new Pc();
  dungeon->player->placeCharacter(dungeon);

  // Create and place monsters
  char monstersFilepath[] = "./.rlg327/monster_desc.txt";
  MonsterParser mp(monstersFilepath);
  std::vector<MonsterDescription> monsters = mp.parseMonsters();

  for (int i = 0; i < dungeon->monsterCount; i++) {
    dungeon->monsters.push_back(monsters[rand() % monsters.size()].createMonster());
    dungeon->monsters[i]->sequence = i + 1;
    dungeon->monsters[i]->placeCharacter(dungeon);
  }
 
  // Create and place items
  char itemsFilepath[] = "./.rlg327/object_desc.txt";
  ItemParser ip(itemsFilepath);
  std::vector<ItemDescription> items = ip.parseItems();

  for (int i = 0; i < 10; i++) {
    dungeon->items.push_back(items[rand() % items.size()].createItem());
    dungeon->items[i]->placeItem(dungeon);
  }

  return 0;
}

int loadDungeon(dungeon_t *dungeon, char* filepath)
{
  char charbuffer[100];
  unsigned int fileversion, filesize;
  uint8_t hardness, x, y, h, w;
  FILE *fp;
  
  fp = fopen(filepath, "r");

  if (!fp) {
    printf("Could not open file.\n");
    return -1;
  }
  
  fread(charbuffer, sizeof(char), 6, fp);
  fread(&fileversion, sizeof(int), 1, fp);
  fread(&filesize, sizeof(int), 1, fp);

  fileversion = be32toh(fileversion);
  filesize = be32toh(filesize);
  
  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      dungeon->map[i][j].val = DUNGEON_WALL;
      dungeon->map[i][j].o_val = DUNGEON_WALL;
      dungeon->map[i][j].y = i;
      dungeon->map[i][j].x = j;
      dungeon->map[i][j].ch = NULL;
      if (i == 0 || j == 0 || i == DUNGEON_HEIGHT - 1 || j == DUNGEON_WIDTH - 1) {
	dungeon->map[i][j].hardness = 255;
      } else {
	fread(&hardness, sizeof(uint8_t), 1, fp);
	dungeon->map[i][j].hardness = hardness;
	if (hardness == 0) {
	  dungeon->map[i][j].val = DUNGEON_CORRIDOR;
	  dungeon->map[i][j].o_val = DUNGEON_CORRIDOR;
	}
      }
    }
  }

  dungeon->numRooms = (filesize - 1496) / 4;
  dungeon->rooms = (room_t *) malloc(sizeof(room_t) * dungeon->numRooms);

  for (i = 0; i < dungeon->numRooms; i++) {
    fread(&x, sizeof(uint8_t), 1, fp);
    fread(&y, sizeof(uint8_t), 1, fp);
    fread(&w, sizeof(uint8_t), 1, fp);
    fread(&h, sizeof(uint8_t), 1, fp);

    
    dungeon->rooms[i].x = x;
    dungeon->rooms[i].y = y;
    dungeon->rooms[i].width = w;
    dungeon->rooms[i].height = h;

    for (k = y; k < y + h; k++) {
      for (j = x; j < x + w; j++) {
	dungeon->map[k][j].val = DUNGEON_FLOOR;
	dungeon->map[k][j].o_val = DUNGEON_FLOOR;
      }
    }
  }
  
  fclose(fp);
  return 0;
}

int saveDungeon(dungeon_t *dungeon, char* filepath)
{
  FILE *fp;
  char filemarker[] = "RGL327";
  uint32_t fileversion = 0;
  uint32_t filesize = 1496 + dungeon->numRooms * 4;
  uint8_t hardness, x, y, w, h;
  
  fileversion = htobe32(fileversion);
  filesize = htobe32(filesize);

  fp = fopen(filepath, "w");

  if (!fp) {
    printf("Could not open file.\n");
    return -1;
  }
  
  fwrite(filemarker, 6, 1, fp);
  fwrite(&fileversion, sizeof(int), 1, fp);
  fwrite(&filesize, sizeof(int), 1, fp);

  for (i = 1; i < DUNGEON_HEIGHT - 1; i++) {
    for (j = 1; j < DUNGEON_WIDTH - 1; j++) {
      hardness = dungeon->map[i][j].hardness;
      fwrite(&hardness, sizeof(uint8_t), 1, fp);
    }
  }

  for (i = 0; i < dungeon->numRooms; i++) {
    x = dungeon->rooms[i].x;
    y = dungeon->rooms[i].y;
    w = dungeon->rooms[i].width;
    h = dungeon->rooms[i].height;

    fwrite(&x, sizeof(uint8_t), 1, fp);
    fwrite(&y, sizeof(uint8_t), 1, fp);
    fwrite(&w, sizeof(uint8_t), 1, fp);
    fwrite(&h, sizeof(uint8_t), 1, fp);
  }

  fclose(fp);
  return 0;
}

int printDungeon(dungeon_t *dungeon)
{
  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      printf("%c", dungeon->map[i][j].val);
    }
    printf("\n");
  }
  
  return 0;
}

void dungeon_delete(dungeon_t *dungeon)
{
  free(dungeon->rooms);
  
  for (std::vector<Npc *>::iterator it = dungeon->monsters.begin() ; it != dungeon->monsters.end(); ++it)
  {
    delete (*it);
  }
  dungeon->monsters.clear();
  
  for (std::vector<Item *>::iterator it = dungeon->items.begin() ; it != dungeon->items.end(); ++it)
  {
    delete (*it);
  }
  dungeon->items.clear();

  Pc *player = dungeon->player;
  dungeon->player = NULL;
  delete player;
}
