#ifndef PC_H
# define PC_H

# include "character.h"

class Pc : public Character {
 public:
  Pc();
  std::vector<Item *> equipment;
  std::vector<Item *> inventory;
  int getAttackDamage();
  void equipItem(Item *item, int invIndex);
  bool takeOffItem(Item *item, int equipIndex);
  int viewDistance;
 private:
  void equipItem(Item *item, int equipIndex, int invIndex);
  void swapItems(int equipIndex, int invIndex); 
};

#endif
