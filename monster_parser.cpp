#include "monster_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "dice.h"
#include "npc.h"
#include "utils.h"

static void readMonster(std::vector<MonsterDescription> &monsters, std::ifstream &file);
static int calculateAbilities(const std::string &abilities);

MonsterParser::MonsterParser(const std::string &filepath)
{
  filepath_ = filepath;
}

std::vector<MonsterDescription> MonsterParser::parseMonsters()
{
  static const std::string fileVersion = "RLG327 MONSTER DESCRIPTION 1";
  static const std::string monsterStart = "BEGIN MONSTER";
  std::ifstream f(filepath_.c_str());
  std::vector<MonsterDescription> monsters;
  
  if (!f) {
    std::cerr << "Error: file could not be loaded." << std::endl;
    return monsters;
  }

  std::string inputLine;
  std::getline(f, inputLine);

  if (inputLine != fileVersion) {
    return monsters;
  }
  
  while (std::getline(f, inputLine)) {
    if (inputLine == monsterStart) {
      readMonster(monsters, f);
    }
  }

  return monsters;
}

static void readMonster(std::vector<MonsterDescription> &monsters, std::ifstream &file)
{
  std::string input, name, desc;
  char symbol;
  Color color;
  Dice speed, damage, hp;
  int ability;

  unsigned char monsterComplete = 0;
  
  file >> input;
  while (true) {
    if (input == "NAME" && !(monsterComplete & 0x01)) {
      std::getline(file, name);
      name = trim(name);
      monsterComplete |= 0x01;
    } else if (input == "SYMB" && !(monsterComplete & 0x02)) {
      file >> symbol;
      monsterComplete |= 0x02;
    } else if (input == "COLOR" && !(monsterComplete & 0x04)) {
      std::string c;
      file >> c;
      color = convertColor(c);
      if (color == INVALID) {
	break;
      }
      monsterComplete |= 0x04;
    } else if (input == "DESC" && !(monsterComplete & 0x08)) {
      std::stringstream ss;
      std::getline(file, input);
      std::getline(file, input);
      while (input != ".") {
	ss << input << std::endl;
	std::getline(file, input);
      }
      desc = ss.str();
      monsterComplete |= 0x08;
    } else if (input == "SPEED" && !(monsterComplete & 0x10)) {
      file >> input;
      speed = parseDice(input);
      monsterComplete |= 0x10;
    } else if (input == "DAM" && !(monsterComplete & 0x20)) {
      file >> input;
      damage = parseDice(input);
      monsterComplete |= 0x20;
    } else if (input == "HP" && !(monsterComplete & 0x40)) {
      file >> input;
      hp = parseDice(input);
      monsterComplete |= 0x40;
    } else if (input == "ABIL" && !(monsterComplete & 0x80)) {
      std::string abilities;
      std::getline(file, abilities);
      ability = calculateAbilities(abilities);
      monsterComplete |= 0x80;
    } else if (input == "END" && (monsterComplete == 0xFF)) {
      monsters.push_back(MonsterDescription(name, desc, speed, color, symbol, ability, hp, damage));
      break;
    } else {
      break;
    }
    file >> input;
  }
}

static int calculateAbilities(const std::string &abilities)
{
  int ability = 0;
  
  if (abilities.find("SMART") != std::string::npos) {
    ability |= NPC_SMART;
  }

  if (abilities.find("TELE") != std::string::npos) {
    ability |= NPC_TELEPATH;
  }

  if (abilities.find("TUNNEL") != std::string::npos) {
    ability |= NPC_TUNNELING;
  }

  if (abilities.find("ERRATIC") != std::string::npos) {
    ability |= NPC_ERRATIC;
  }

  if (abilities.find("PASS") != std::string::npos) {
    ability |= NPC_PASS;
  }
  
  return ability;
}
