#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#include <vector>

#include "binheap.h"
#include "dungeon.h"
#include "movement.h"
#include "npc.h"
#include "path.h"
#include "pc.h"
#include "user_interface.h"

static int32_t compare_character(const void *key, const void *with) {
  int32_t comp = ((Character *) key)->turn - ((Character *) with)->turn;

  if (!comp) {
    return ((Character *) key)->sequence - ((Character *) with)->sequence;
  } else {
    return comp;
  }
}

int main(int argc, char *argv[])
{  
  int seed = (int)time(NULL); 
  int save = 0;
  int load = 0;
  int monsterCount = 10;
  dungeon_t d;
  int i;
  binheap_t character_queue;
  
  // Determine load/save filepath
  char saveFilepath[] = "./.rlg327/dungeon";
    
  if (argc > 1) {
    for (i = 0; i < argc; i++) {
      if (!strcasecmp(argv[i], "--save")) {
	save = 1;
      }
      if (!strcasecmp(argv[i], "--load")) {
	load = 1;
      }
      if (!strcasecmp(argv[i], "--nummon")) {
	if (i + 1 < argc) {
	  monsterCount = atoi(argv[i + 1]);
	  printf("Monster count: %d\n", monsterCount);
	} else {
	  printf("Missing paramter after --nummon flag\n");
	  return 1;
	}
      }
      if (!strcasecmp(argv[i], "--seed")) {
	if (i + 1 < argc) {
	  seed = atoi(argv[i + 1]);
	} else {
	  printf("Missing seed value after --seed flag\n");
	  return 1;
	}
      }
    }
  }

  srand(seed);
  d.monsterCount = monsterCount;

  if (load) {
    loadDungeon(&d, saveFilepath);
  } else {
    printf("Seed: %d\n", seed);
    dungeon_init(&d);
    generateDungeon(&d);
  }
  binheap_init(&character_queue, compare_character, NULL);

  Character *current;
  int gameover = 0;

  d.player->hn = binheap_insert(&character_queue, d.player);
  
  // Place Monsters into queue
  add_npcs_to_heap(&d, &character_queue);
  
  ui_init(&d);
  ui_refresh(&d);

  // Run loop
  while (!gameover) {
    current = (Character *) binheap_remove_min(&character_queue);
    if (current->alive) {
      current->hn = NULL;
      current->moveNum++;
      current->turn = current->turn + (100 / current->speed);
      current->hn = binheap_insert(&character_queue, current);
      gameover = moveCharacter(&d.map[current->y][current->x], &d, &character_queue);
      ui_refresh(&d);
      if (current->symbol == '@') { 
	ui_refresh(&d);
      }
    }
  }

  ui_stop();
  
  if (gameover == 1) {
    printf("Game Over: All monstered killed\n");
  } else if (gameover == 2) {
    printf("Game Over: Player killed\n");
  } else if (gameover == 3) {
    printf("Player exited game\n");
  }
  
  if (save) {
    saveDungeon(&d, saveFilepath);
  }

  binheap_delete(&character_queue);
  dungeon_delete(&d);
  
  return 0;
}

