#ifndef USER_INTERFACE_H
# define USER_INTERFACE_H

# include <string>

# include "dungeon.h"
# include "binheap.h"

void ui_init(dungeon_t *d);
void ui_refresh(dungeon_t *d);
int  ui_getPlayerDirection(int *nextX, int *nextY, dungeon_t *d, binheap_t *h);
void ui_printMessage(std::string output, int row);
void ui_updateCell(cell_t *cell);
void ui_stop();

#endif
