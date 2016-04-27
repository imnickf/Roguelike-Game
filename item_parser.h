#ifndef ITEM_PARSER_H
# define ITEM_PARSER_H

# include <unordered_map>
# include <vector>

# include "descriptions.h"

class ItemParser {
 public:
  ItemParser(const std::string &filepath);
  std::vector<ItemDescription> parseItems();
 private:
  std::string filepath_;
  std::unordered_map<std::string, item_type_t> types_map;
  void readItem(std::vector<ItemDescription> &items, std::ifstream &file);
};

#endif
