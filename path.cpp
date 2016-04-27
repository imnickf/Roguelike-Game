#include "path.h"

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

static int32_t compare_distance(const void *key, const void *with)
{
  return ((cell_t *) key)->distance - ((cell_t *) with)->distance;
}

void generateTunnelingPath(dungeon_t *d)
{
  int i, j, x, y, alt;
  binheap_t h;
  cell_t *cell;

  binheap_init(&h, compare_distance, NULL);
  
  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      if (d->map[i][j].val == '@') {
	d->map[i][j].distance = 0;
      } else {
	d->map[i][j].distance = USHRT_MAX;
      }
      if (i != 0 && j != 0 && i != DUNGEON_HEIGHT - 1 && j != DUNGEON_WIDTH - 1) {
	d->map[i][j].hn = binheap_insert(&h, &(d->map[i][j]));
      } else {
	d->map[i][j].hn = NULL;
      }
    }
  }
  
  while ((cell = (cell_t *) binheap_remove_min(&h))) {
    x = cell->x;
    y = cell->y;
    cell->hn = NULL;
    
    for (i = y - 1; i <= y + 1; i++) {
      for (j = x - 1; j <= x + 1; j++) {
	if (d->map[i][j].hn) {
	  alt = cell->distance + (cell->hardness / 60) + 1;
	  if (alt < d->map[i][j].distance && i != y && j != x) {
	    d->map[i][j].distance = alt;
	    binheap_decrease_key(&h, d->map[i][j].hn);
	  }
	}
      }
    }
  }
  
  binheap_delete(&h);
}

void generateNonTunnelingPath(dungeon_t *d)
{
  int i, j, x, y, alt;
  binheap_t h;
  cell_t *cell;
  
  binheap_init(&h, compare_distance, NULL);
  
  for (i = 1; i < DUNGEON_HEIGHT - 1; i++) {
    for (j = 1; j < DUNGEON_WIDTH - 1; j++) {
      if (d->map[i][j].hardness == 0) {
	if (d->map[i][j].val == '@') {
	  d->map[i][j].distance = 0;
	} else {
	  d->map[i][j].distance = USHRT_MAX;
	}
	d->map[i][j].hn = binheap_insert(&h, &(d->map[i][j]));
      } 
    }
  }
  
  while ((cell = (cell_t *) binheap_remove_min(&h))) {
    x = cell->x;
    y = cell->y;
    cell->hn = NULL;
    
    for (i = y - 1; i <= y + 1; i++) {
      for (j = x - 1; j <= x + 1; j++) {
	if (d->map[i][j].hardness == 0) {
	  alt = cell->distance + 1;
	  if (alt < d->map[i][j].distance && i != y && j != x) {
	    d->map[i][j].distance = alt;
	    binheap_decrease_key(&h, d->map[i][j].hn);
	  }
	}
      }
    }
  }
  
  binheap_delete(&h);
}

void printTunnelingPath(dungeon_t *d)
{
  int i, j, distance;

  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      distance = d->map[i][j].distance;
      if (distance < 10) {
	printf("%c", 48 + distance);
      } else if (distance < 36) {
	printf("%c", 87 + distance);
      } else if (distance < 62) {
	printf("%c", 29 + distance);
      } else {
	printf("%c", d->map[i][j].val);
      }
    }
    printf("\n");
  }
}

void printNonTunnelingPath(dungeon_t *d)
{
  int i, j, distance;

  for (i = 0; i < DUNGEON_HEIGHT; i++) {
    for (j = 0; j < DUNGEON_WIDTH; j++) {
      if (d->map[i][j].hardness == (int)0) {
	distance = d->map[i][j].distance;
	if (distance < 10) {
	  printf("%c", 48 + distance);
	} else if (distance < 36) {
	  printf("%c", 87 + distance);
	} else if (distance < 62) {
	  printf("%c", 29 + distance);
	} else {
	  printf("%c", d->map[i][j].val);
	}
      } else {
	printf(" ");
      }
    }
    printf("\n");
  }
}
