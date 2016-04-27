#ifndef MONSTER_PARSER_H
# define MONSTER_PARSER_H

# include <string>
# include <vector>

# include "descriptions.h"

class MonsterParser {
 public:
  MonsterParser(const std::string &filepath);
  std::vector<MonsterDescription> parseMonsters();
 private:
  std::string filepath_;
};

#endif
