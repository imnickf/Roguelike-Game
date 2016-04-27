#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>

# include <string>

# include "binheap.h"
# include "dice.h"
# include "dungeon.h"

enum Color {
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  INVALID
};

class Character {
 public: 
  void placeCharacter(dungeon_t *d);
  virtual int getAttackDamage()=0;
  std::string name;
  std::string description;
  char symbol;
  Color color;
  int x;
  int y;
  uint8_t turn;
  uint8_t sequence;
  uint8_t speed;
  int moveNum;
  int hp, maxHp;
  Dice damage;
  bool alive;
  binheap_node_t *hn;
};

#endif
