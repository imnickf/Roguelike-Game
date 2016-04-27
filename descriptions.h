#ifndef DESCRIPTIONS_H
# define DESCRIPTIONS_H

# include <string>

# include "dice.h"
# include "item.h"
# include "npc.h"

Color convertColor(const std::string &color);

class MonsterDescription {
 public:
  MonsterDescription() {}
  MonsterDescription(const std::string &name, const std::string &desc, const Dice &speed, const Color color, const char symb, const int ability, const Dice &hp, const Dice &dam);
  Npc * createMonster();
 private:
  std::string name_;
  std::string description_;
  char symbol_;
  Color color_;
  Dice speed_;
  int ability_;
  Dice hp_;
  Dice damage_;
};

class ItemDescription {
 public:
  ItemDescription() {}
  ItemDescription(const std::string &name, const std::string &desc, const item_type_t type, const Color color, const Dice &hit, const Dice &damage, const Dice &defense, const Dice &weight, const Dice &speed, const Dice &attribute, const Dice &value);
  Item * createItem();
 private:
  std::string name_;
  std::string description_;
  item_type_t type_;
  char symbol_;
  Color color_;
  Dice hit_, damage_, dodge_, defense_, weight_, speed_, attribute_, value_;
};

#endif
