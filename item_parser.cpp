#include "item_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "dice.h"
#include "utils.h"

ItemParser::ItemParser(const std::string &filepath)
{
  filepath_ = filepath;
  types_map.insert({
    {"WEAPON", itemtype_WEAPON},
    {"OFFHAND", itemtype_OFFHAND},
    {"RANGED", itemtype_RANGED},
    {"ARMOR", itemtype_ARMOR},
    {"HELMET", itemtype_HELMET},
    {"CLOAK", itemtype_CLOAK},
    {"GLOVES", itemtype_GLOVES},
    {"BOOTS", itemtype_BOOTS},
    {"RING", itemtype_RING},
    {"AMULET", itemtype_AMULET},
    {"LIGHT", itemtype_LIGHT},
    {"SCROLL", itemtype_SCROLL},
    {"BOOK", itemtype_BOOK},
    {"FLASK", itemtype_FLASK},
    {"GOLD", itemtype_GOLD},
    {"AMMUNITION", itemtype_AMMUNITION},
    {"FOOD", itemtype_FOOD},
    {"WAND", itemtype_WAND},
    {"CONTAINER", itemtype_CONTAINER}
  });
}

std::vector<ItemDescription> ItemParser::parseItems()
{
  static const std::string fileVersion = "RLG327 OBJECT DESCRIPTION 1";
  static const std::string itemStart = "BEGIN OBJECT";
  std::ifstream f(filepath_.c_str());
  std::vector<ItemDescription> items;

  if (!f) {
    std::cerr << "Error: file could not be loaded." << std::endl;
    return items;
  }

  std::string inputLine;
  std::getline(f, inputLine);

  if (inputLine != fileVersion) {
    return items;
  }

  while (std::getline(f, inputLine)) {
    if (inputLine == itemStart) {
      readItem(items, f);
    }
  }
  return items;
}

void ItemParser::readItem(std::vector<ItemDescription> &items, std::ifstream &file)
{
  std::string input, name, desc;
  item_type_t type;
  Color color;
  Dice weight, hit, damage, attribute, val, dodge, defense, speed;
  
  uint16_t itemComplete = 0;

  file >> input;
  while (true) {
    if (input == "NAME" && !(itemComplete & 0x0001)) {
      std::getline(file, name);
      name = trim(name);
      itemComplete |= 0x0001;
    } else if (input == "TYPE" && !(itemComplete & 0x0002)) {
      file >> input;
      std::unordered_map<std::string, item_type_t>::const_iterator got = types_map.find(input);
      if (got == types_map.end()) {
	break;
      } else {
	type = types_map[input];
      } 
      itemComplete |= 0x0002;
    } else if (input == "COLOR" && !(itemComplete & 0x0004)) {
      std::string c;
      file >> c;
      color = convertColor(c);
      if (color == INVALID) {
	break;
      }
      itemComplete |= 0x0004;
    } else if (input == "DESC" && !(itemComplete & 0x0008)) {
      std::stringstream ss;
      std::getline(file, input);
      std::getline(file, input);
      while (input != ".") {
	ss << input << std::endl;
	std::getline(file, input);
      }
      desc = ss.str();
      itemComplete |= 0x0008;
    } else if (input == "WEIGHT" && !(itemComplete & 0x0010)) {
      file >> input;
      weight = parseDice(input);
      itemComplete |= 0x0010;
    } else if (input == "HIT" && !(itemComplete & 0x0020)) {
      file >> input;
      hit = parseDice(input);
      itemComplete |= 0x0020;
    } else if (input == "DAM" && !(itemComplete & 0x0040)) {
      file >> input;
      damage = parseDice(input);
      itemComplete |= 0x0040;
    } else if (input == "ATTR" && !(itemComplete & 0x0080)) {
      file >> input;
      attribute = parseDice(input);
      itemComplete |= 0x0080;
    } else if (input == "VAL" && !(itemComplete & 0x0100)) {
      file >> input;
      val = parseDice(input);
      itemComplete |= 0x0100;
    } else if (input == "DODGE" && !(itemComplete & 0x0200)) {
      file >> input;
      dodge = parseDice(input);
      itemComplete |= 0x0200;
    } else if (input == "DEF" && !(itemComplete & 0x0400)) {
      file >> input;
      defense = parseDice(input);
      itemComplete |= 0x0400;
    } else if (input == "SPEED" && !(itemComplete & 0x0800)) {
      file >> input;
      speed = parseDice(input);
      itemComplete |= 0x0800;
    } else if (input == "END" && (itemComplete == 0x0FFF)) {
      items.push_back(ItemDescription(name, desc, type, color, hit, damage, defense, weight, speed, attribute, val));
      break;
    } else {
      break;
    }
    file >> input;
  }
}
