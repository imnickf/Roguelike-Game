#ifndef ITEM_H
# define ITEM_H

# include <string>

# include "character.h"
# include "dice.h"
# include "dungeon.h"

typedef enum item_type {
  itemtype_no_type,
  itemtype_WEAPON,
  itemtype_OFFHAND,
  itemtype_RANGED,
  itemtype_LIGHT,
  itemtype_ARMOR,
  itemtype_HELMET,
  itemtype_CLOAK,
  itemtype_GLOVES,
  itemtype_BOOTS,
  itemtype_AMULET,
  itemtype_RING,
  itemtype_SCROLL,
  itemtype_BOOK,
  itemtype_FLASK,
  itemtype_GOLD,
  itemtype_AMMUNITION,
  itemtype_FOOD,
  itemtype_WAND,
  itemtype_CONTAINER
} item_type_t;

class Item {
 public:
  Item() {}
  Item(const std::string &name, const std::string &desc, const item_type_t type, const char symbol, const Color color, const Dice &damage, const int hit, const int dodge, const int defense, const int weight, const int speed, const int attribute, const int value);
  std::string name;
  std::string description;
  void placeItem(dungeon_t *d);
  item_type_t type;
  char symbol;
  Color color;
  Dice damage;
  int hit, dodge, defense, weight, speed, attribute, value;
};

#endif
