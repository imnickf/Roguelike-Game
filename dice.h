#ifndef DICE_H
# define DICE_H

# include <string>

class Dice {
 public:
  Dice() {}
  Dice(int offset, int num, int sides);
  int roll() const;
  int getOffset();
  int getNumber();
  int getSides();
 private:
  int offset_;
  int number_of_dice_;
  int sides_;
};

Dice parseDice(std::string &diceString);

#endif
