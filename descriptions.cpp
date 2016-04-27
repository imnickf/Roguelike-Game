#include "descriptions.h"

static const char item_symbol[] = {
  '*', /* no_type*/
  '|', /* WEAPON */
  ')', /* OFFHAND */
  '}', /* RANGED */
  '_', /* LIGHT */
  '[', /* ARMOR */
  ']', /* HELMET */
  '(', /* CLOAK */
  '{', /* GLOVES */
  '\\', /* BOOTS */
  '"', /* AMULET */
  '=', /* RING */
  '~', /* SCROLL */
  '?', /* BOOK */
  '!', /* FLASK */
  '$', /* GOLD */
  '/', /* AMMUNITION */
  ',', /* FOOD */
  '-', /* WAND */
  '%', /* CONTAINER */
};

MonsterDescription::MonsterDescription(const std::string &name, const std::string &desc, const Dice &speed, const Color color, const char symb, const int ability, const Dice &hp, const Dice &dam)
{
  name_ = name;
  description_ = desc;
  symbol_ = symb;
  color_ = color;
  speed_ = speed;
  ability_ = ability;
  hp_ = hp;
  damage_ = dam;
}

Npc * MonsterDescription::createMonster()
{
  Npc *monster = new Npc(name_, description_, speed_.roll(), color_, symbol_, ability_, hp_.roll(), damage_);
  return monster;
}

ItemDescription::ItemDescription(const std::string &name, const std::string &desc, const item_type_t type, const Color color, const Dice &hit, const Dice &damage, const Dice &defense, const Dice &weight, const Dice &speed, const Dice &attribute, const Dice &value)
{
  name_ = name;
  description_ = desc;
  type_ = type;
  symbol_ = item_symbol[type];
  color_ = color;
  hit_ = hit;
  damage_ = damage;
  defense_ = defense;
  weight_ = weight;
  speed_ = speed;
  attribute_ = attribute;
  value_ = value;
}

Item * ItemDescription::createItem()
{
  Item *item = new Item(name_, description_, type_, symbol_, color_, damage_, hit_.roll(), dodge_.roll(), defense_.roll(), weight_.roll(), speed_.roll(), attribute_.roll(), value_.roll());
  return item;
}

Color convertColor(const std::string &color)
{
  if (color == "BLACK") {
    return BLACK;
  } else if (color == "RED") {
    return RED;
  } else if (color == "GREEN") {
    return GREEN;
  } else if (color == "BLUE") {
    return BLUE;
  } else if (color == "CYAN") {
    return CYAN;
  } else if (color == "YELLOW") {
    return YELLOW;
  } else if (color == "MAGENTA") {
    return MAGENTA;
  } else if (color == "WHITE"){
    return WHITE;
  } else {
    return INVALID;
  }
}
