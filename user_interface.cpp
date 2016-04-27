#include "user_interface.h"

#include <ncurses.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "item.h"
#include "npc.h"
#include "pc.h"
#include "utils.h"

static void displayMonsterList(dungeon_t *d);
static void clearMonsterList(dungeon_t *d);
static void createMonsterListString(char *string, Npc &monster, dungeon_t *d);
static void generateNewDungeon(dungeon_t *d, binheap_t *h);
static void handleWearItem(dungeon_t *d);
static void handleTakeOffItem(dungeon_t *d);
static void handleDropItem(dungeon_t *d);
static void handleDeleteItem(dungeon_t *d);
static void handleViewInventory(dungeon_t *d);
static void handleViewEquipment(dungeon_t *d);
static void handleInspectItem(dungeon_t *d);
static void handleInspectCell(dungeon_t *d);
static void displayPcInventory(dungeon_t *d);
static void displayPcEquipment(dungeon_t *d);

void ui_init(dungeon_t *d)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
}

void ui_refresh(dungeon_t *d)
{
  int x, y;
  int pc_x = d->player->x;
  int pc_y = d->player->y;
  
  for (y = pc_y - d->player->viewDistance; y <= pc_y + d->player->viewDistance; y++) {
    for (x = pc_x - d->player->viewDistance; x <= pc_x + d->player->viewDistance; x++) {
      if (y > 0 && x > 0 && y < DUNGEON_HEIGHT && x < DUNGEON_WIDTH) {
	d->map[y][x].remembered = d->map[y][x].val;
	if (d->map[y][x].ch) {
	  d->map[y][x].remembered = d->map[y][x].ch->symbol;
	  attron(COLOR_PAIR(d->map[y][x].ch->color));
	  mvaddch(y + 1, x, d->map[y][x].ch->symbol);
	  attroff(COLOR_PAIR(d->map[y][x].ch->color));
	} else if (d->map[y][x].item) {
	  d->map[y][x].remembered = d->map[y][x].item->symbol;
	  attron(COLOR_PAIR(d->map[y][x].item->color));
	  mvaddch(y + 1, x, d->map[y][x].item->symbol);
	  attroff(COLOR_PAIR(d->map[y][x].item->color));
	} else {
	  mvaddch(y + 1, x, d->map[y][x].val);
	}
      }
    }
  }
  refresh();
}

void ui_printMessage(std::string output, int row)
{
  if (row == 0) {
    mvprintw(20, 0, "                                          ");
    mvprintw(20, 0, output.c_str());
  } else {
    mvprintw(21, 0, "                                          ");
    mvprintw(21, 0, output.c_str());
  }
}

int ui_getPlayerDirection(int *nextX, int *nextY, dungeon_t *d, binheap_t *h)
{
  int input, tempX, tempY;
  input = getch();
  
  tempX = *nextX;
  tempY = *nextY;
  
  switch(input)
  {
  case '7':
  case 'y':
    if (d->map[tempY - 1][tempX - 1].hardness > 0) {
      return 1;
    }
    (*nextX)--;
    (*nextY)--;
    break;
  case '8':
  case 'k':
    if (d->map[tempY - 1][tempX].hardness > 0) {
      return 1;
    }
    (*nextY)--;
    break;
  case '9':
  case 'u':
    if (d->map[tempY - 1][tempX + 1].hardness > 0) {
      return 1;
    }
    (*nextX)++;
    (*nextY)--;
    break;
  case '6':
  case 'l':
    if (d->map[tempY][tempX + 1].hardness > 0) {
      return 1;
    }
    (*nextX)++;
    break;
  case '3':
  case 'n':
    if (d->map[tempY + 1][tempX + 1].hardness > 0) {
      return 1;
    }
    (*nextX)++;
    (*nextY)++;
    break;
  case '2':
  case 'j':
    if (d->map[tempY + 1][tempX].hardness > 0) {
      return 1;
    }
    (*nextY)++;
    break;
  case '1':
  case 'b':
    if (d->map[tempY + 1][tempX - 1].hardness > 0) {
      return 1;
    }
    (*nextX)--;
    (*nextY)++;
    break;
  case '4':
  case 'h':
    if (d->map[tempY][tempX - 1].hardness > 0) {
      return 1;
    }
    (*nextX)--;
    break;
  case '<':
    if (d->map[tempY][tempX].o_val != '<') {
      return 1;
    }
    generateNewDungeon(d, h);
    return 2;
    break;
  case '>':
    if (d->map[tempY][tempX].o_val != '>') {
      return 1;
    }
    generateNewDungeon(d, h);
    return 2;
    break;
  case ' ':    // Rest a turn
    return 0;
    break;
  case 'm':    // Display monster list
    displayMonsterList(d);
    return 1;
    break;
  case 'w':
    handleWearItem(d);
    return 1;
    break;
  case 't':
    handleTakeOffItem(d);
    return 1;
    break;
  case 'd':
    handleDropItem(d);
    return 1;
    break;
  case 'x':
    handleDeleteItem(d);
    return 1;
    break;
  case 'i':
    handleViewInventory(d);
    return 1;
    break;
  case 'e':
    handleViewEquipment(d);
    return 1;
    break;
  case 'I':
    handleInspectItem(d);
    return 1;
    break;
  case 'S': // Exit Game and save (added in the future)
    return 3;
    break;
  case 'C':
    handleInspectCell(d);
    return 1;
    break;
  default:
    mvprintw(0, 0, "Invalid key press");
    return 1;
    break;
  }
  mvprintw(0,0, "                 ");
  return 0;
}

void ui_stop()
{
  endwin();
}

static void handleWearItem(dungeon_t *d)
{
  int input;
  displayPcInventory(d);
  mvprintw(15, 10, "#     Enter number to equip      #");

  input = getch();
  while (input < 48 || input >= 58 || !d->player->inventory[input-48]) {
    if (input == 27) {
      clearMonsterList(d);
      return;
    }
    input = getch();
  }

  d->player->equipItem(d->player->inventory[input-48], input-48);
  clearMonsterList(d);
}

static void handleTakeOffItem(dungeon_t *d)
{
  int input;
  displayPcEquipment(d);
  mvprintw(17, 10, "#    Enter letter to take off    #");

  input = getch();
  while (input < 97 || input >= 109 || !d->player->equipment[input-97]) {
    if (input == 27) {
      clearMonsterList(d);
      return;
    }
    input = getch();
  }

  if (!d->player->takeOffItem(d->player->equipment[input-97], input-97)) {
    clearMonsterList(d);
    ui_printMessage("Item cannot be unequipped. No inventory space.", 1);
  } else {
    clearMonsterList(d);
  }
}

static void handleDropItem(dungeon_t *d)
{
  int input;
  displayPcInventory(d);
  mvprintw(15, 10, "#      Enter number to drop      #");

  input = getch();
  while (input < 48 || input >= 58 || !d->player->inventory[input-48]) {
    if (input == 27) {
      clearMonsterList(d);
      return;
    }
    input = getch();
  }

  if (d->map[d->player->y][d->player->x].item) {
    clearMonsterList(d);
    ui_printMessage("Item cannot be dropped here.", 1);
  } else {
    d->map[d->player->y][d->player->x].item = d->player->inventory[input-48];
    d->player->inventory[input-48] = NULL;
    clearMonsterList(d);
  }
}

static void handleDeleteItem(dungeon_t *d)
{
  int input;
  displayPcInventory(d);
  mvprintw(15, 10, "#     Enter number to delete     #");

  input = getch();
  while (input < 48 || input >= 58 || !d->player->inventory[input-48]) {
    if (input == 27) {
      clearMonsterList(d);
      return;
    }
    input = getch();
  }

  d->player->inventory[input-48] = NULL;

  clearMonsterList(d);
}

static void handleViewInventory(dungeon_t *d)
{
  int input;
  displayPcInventory(d);

  input = getch();
  while (input != 27) {
    input = getch();
  }
  clearMonsterList(d);
}

static void handleViewEquipment(dungeon_t *d)
{
  int input;
  displayPcEquipment(d);

  input = getch();
  while (input != 27) {
    input = getch();
  }
  clearMonsterList(d);
}

static void handleInspectItem(dungeon_t *d)
{
  int input;
  displayPcInventory(d);
  mvprintw(15, 10, "#   Enter Number to view desc    #");
  
  input = getch();
  while (input < 48 || input >= 58 || !d->player->inventory[input-48]) {
    if (input == 27) {
      clearMonsterList(d);
      return;
    }
    input = getch();
  }

  clearMonsterList(d);
  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#           Description          #");
  mvprintw(4, 10, "#          -------------         #");

  std::vector<std::string> descStrings = split(d->player->inventory[input-48]->description, 30);

  for (unsigned int i = 0; i < descStrings.size(); i++) {
    char output[40];
    char *line = strdup(descStrings[i].c_str());
    sprintf(output, "# %-30s #", line);
    mvprintw(5 + i, 10, output);
  }
  mvprintw(5 + descStrings.size(), 10, "#                                #");
  mvprintw(6 + descStrings.size(), 10, "##################################");

  input = getch();
  while (input != 27) {
    input = getch();
  }
  clearMonsterList(d);
}

static void handleInspectCell(dungeon_t *d)
{
  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#         Inspect A Cell         #");
  mvprintw(4, 10, "#        ----------------        #");
  mvprintw(5, 10, "#                                #");
  mvprintw(6, 10, "#  type X coord, then enter      #");
  mvprintw(7, 10, "#                                #");
  mvprintw(8, 10, "##################################");

  int input = getch();
  char x_string[4];
  int count = 0;
  while (input != 10) {
    if (input < 48 || input > 57) {
      mvprintw(7, 10, "#  please enter a number         #");
    } else if (count < 3) {
      x_string[count++] = input;
      mvprintw(7, 10, "#                                #");
    } else {
      clearMonsterList(d);
      ui_printMessage("Number too large.", 1);
      return;
    }
    input = getch();
  }
  int x = atoi(x_string);

  mvprintw(6, 10, "#  type Y coord, then enter      #");
  input = getch();
  count = 0;
  char y_string[4];
  while (input != 10) {
    if (input < 48 || input > 57) {
      mvprintw(7, 10, "#  please enter a number         #");
    } else if (count < 3) {
      y_string[count++] = input;
      mvprintw(7, 10, "#                                #");
    } else {
      clearMonsterList(d);
      ui_printMessage("Number too large.", 1);
      return;
    }
    input = getch();
  }
  int y = atoi(y_string);

  if (x > DUNGEON_WIDTH - 1 || y > DUNGEON_HEIGHT - 1) {
    clearMonsterList(d);
    ui_printMessage("Invalid coordinates.", 1);
    return;
  }

  std::string description;
  if (d->map[y][x].ch) {
    description = d->map[y][x].ch->description;
  } else if (d->map[y][x].item) {
    description = d->map[y][x].item->description;
  } else {
    clearMonsterList(d);
    ui_printMessage("No item or monster.", 1);
    return;
  }

  clearMonsterList(d);
  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#           Description          #");
  mvprintw(4, 10, "#          -------------         #");

  std::vector<std::string> descStrings = split(description, 30);

  for (unsigned int i = 0; i < descStrings.size(); i++) {
    char output[40];
    char *line = strdup(descStrings[i].c_str());
    sprintf(output, "# %-30s #", line);
    mvprintw(5 + i, 10, output);
  }
  mvprintw(5 + descStrings.size(), 10, "#                                #");
  mvprintw(6 + descStrings.size(), 10, "##################################");

  input = getch();
  while (input != 27) {
    input = getch();
  }
  clearMonsterList(d);
}

static void displayPcInventory(dungeon_t *d)
{
  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#        Player Inventory        #");
  mvprintw(4, 10, "#        ----------------        #");
  
  for (unsigned int i = 0; i < d->player->inventory.capacity(); i++) {
    if (d->player->inventory[i]) {
      char output[40];
      char *itemName = strdup(d->player->inventory[i]->name.c_str());
      sprintf(output, "# %d   %-26s #", i, itemName);
      mvprintw(5 + i, 10, output);
    } else {
      mvprintw(5 + i, 10, "#                                #");
    }
  }
  mvprintw(15, 10, "#                                #");
  mvprintw(16, 10, "##################################");
}

static void displayPcEquipment(dungeon_t *d)
{
  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#        Player Equipment        #");
  mvprintw(4, 10, "#        ----------------        #");
  
  for (unsigned int i = 0; i < d->player->equipment.capacity(); i++) {
    char output[40];
    if (d->player->equipment[i]) {
      char *itemName = strdup(d->player->equipment[i]->name.c_str());
      sprintf(output, "# %c   %-26s #", i + 97, itemName);
      mvprintw(5 + i, 10, output);
    } else {
      sprintf(output, "# %-30c #", i + 97);
      mvprintw(5 + i, 10, output);
    }
  }
  mvprintw(17, 10, "#                                #");
  mvprintw(18, 10, "##################################");
}

static void displayMonsterList(dungeon_t *d)
{
  int input, i, count;
  int monsterListRows = 5;
  int upperBound = 0;
  
  char monsterStrings[d->monsterCount][40];
  for (i = 0, count = 0; i < d->monsterCount; i++, count++) {
    while (!d->monsters[count]->alive) {
      count++;
    }
      createMonsterListString(monsterStrings[i], *d->monsters[count], d);
  }

  mvprintw(2, 10, "##################################");
  mvprintw(3, 10, "#          Monster List          #");
  mvprintw(4, 10, "#          ------------          #");
  for (i = 0; i < monsterListRows; i++) {
    if (i < d->monsterCount) {
      mvprintw(5 + i, 10, "%s", monsterStrings[i]);
    } else {
      mvprintw(5 + i, 10, "#                                #");
    }
  }
  mvprintw(5 + monsterListRows, 10, "#                                #");
  mvprintw(6 + monsterListRows, 10, "#   scroll with arrows up/down   #");
  mvprintw(7 + monsterListRows, 10, "##################################");
  
  input = getch();
  while(input != 27) {

    switch (input) {
    case KEY_DOWN:
      if (upperBound < d->monsterCount - monsterListRows) {
	upperBound++;
	for (i = upperBound; i < upperBound + monsterListRows; i++) {
	  if (i < d->monsterCount) {
	    mvprintw(5 + i - upperBound, 10, "%s", monsterStrings[i]);
	  } else {
	    mvprintw(5 + i, 10, "#                                #");
	  }
	}
      }
      break;
    case KEY_UP:
      if (upperBound > 0) {
	upperBound--;
	for (i = upperBound; i < upperBound + monsterListRows; i++) {
	  if (i < d->monsterCount) {
	    mvprintw(5 + i - upperBound, 10, "%s", monsterStrings[i]);
	  } else {
	    mvprintw(5 + i, 10, "#                                #");
	  }
	}
      }
      break;
    default:
      break;
    }
    input = getch();
  }
  clearMonsterList(d);
}

static void clearMonsterList(dungeon_t *d)
{
  int x, y;

  for (y = 0; y < DUNGEON_HEIGHT; y++) {
    for (x = 0; x < DUNGEON_WIDTH; x++) {
      if (d->map[y][x].ch && d->map[y][x].remembered != ' ') {
	  attron(COLOR_PAIR(d->map[y][x].ch->color));
	  mvaddch(y + 1, x, d->map[y][x].remembered);
	  attroff(COLOR_PAIR(d->map[y][x].ch->color));
	} else if (d->map[y][x].item && d->map[y][x].remembered != ' ') {
	  d->map[y][x].remembered = d->map[y][x].item->symbol;
	  attron(COLOR_PAIR(d->map[y][x].item->color));
	  mvaddch(y + 1, x, d->map[y][x].remembered);
	  attroff(COLOR_PAIR(d->map[y][x].item->color));
	} else {
	  mvaddch(y + 1, x, d->map[y][x].remembered);
	}
    }
  }
  refresh();
}

static void createMonsterListString(char *string, Npc &monster, dungeon_t *d)
{
  int diff_x, diff_y;

  std::string northSouth, eastWest;

  diff_x = monster.x - d->player->x;
  diff_y = monster.y - d->player->y;

  if (diff_y < 0) {
    northSouth = "north";
    diff_y *= -1;
  } else {
    northSouth = "south";
  }

  if (diff_x < 0) {
    eastWest = "west";
    diff_x *= -1;
  } else {
    eastWest = "east";
  }
  
  sprintf(string, "#    %c, %-2d %s and %-2d %s     #", monster.symbol, diff_y, northSouth.c_str(), diff_x, eastWest.c_str());
}

static void generateNewDungeon(dungeon_t *d, binheap_t *h)
{
  while (!binheap_is_empty(h)) {
    binheap_remove_min(h);
  }
  
  d->monsterCount = 10;
  dungeon_delete(d);
  dungeon_init(d);
  generateDungeon(d);

  d->player->hn = binheap_insert(h, d->player);
  add_npcs_to_heap(d, h);

  clear();
  refresh();
  ui_refresh(d);
}
