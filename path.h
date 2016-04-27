#ifndef PATH_H
 #define PATH_H

#include "binheap.h"
#include "dungeon.h"

void generateTunnelingPath(dungeon_t *d);
void generateNonTunnelingPath(dungeon_t *d);
void printTunnelingPath(dungeon_t *d);
void printNonTunnelingPath(dungeon_t *d);

#endif
