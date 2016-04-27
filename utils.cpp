#include "utils.h"

#include <algorithm>

std::string trim(std::string &str)
{
  size_t first = str.find_first_not_of(' ');
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(std::string str, int splitLength)
{
  int numSubstrings = str.length() / splitLength;
  std::vector<std::string> toReturn;

  // Replace all newlines with spaces
  std::replace_if(str.begin() , str.end() ,
		  [] (const char& c) { return c == '\n';},' ');
  
  for (int i = 0; i < numSubstrings; i++)
    {
      toReturn.push_back(str.substr(i * splitLength, splitLength));
    }

  // If there are leftover characters, create a shorter item at the end.
  if (str.length() % splitLength != 0)
    {
      toReturn.push_back(str.substr(splitLength * numSubstrings));
    }

  return toReturn;
}
