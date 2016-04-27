#include "dice.h"

#include <cstdlib>
#include <sstream>

Dice parseDice(std::string &diceString)
{
  std::stringstream ss;
  std::string::iterator it = diceString.begin();
  int offset, num, sides;
  
  while (*it != '+') {
    ss << *it;
    it++;
  }
  ss >> offset;
  ss.clear();
  ss.str("");
  it++;
  
  while (*it != 'd') {
    ss << *it;
    it++;
  }
  
  ss >> num;
  ss.clear();
  ss.str("");
  it++;

  while (it != diceString.end()) {
    ss << *it;
    it++;
  }
  ss >> sides;

  Dice dice (offset, num, sides);

  return dice;
}

Dice::Dice(int offset, int num, int sides)
{
  offset_ = offset;
  number_of_dice_ = num;
  sides_ = sides;
}

int Dice::roll() const
{
  int roll = 0;
  
  for (int i = 0; i < number_of_dice_; i++) {
    roll += std::rand() % sides_ + 1;
  }
  
  return roll + offset_;
}

int Dice::getOffset()
{
  return offset_;
}

int Dice::getNumber()
{
  return number_of_dice_;
}

int Dice::getSides()
{
  return sides_;
}
