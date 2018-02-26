#include <cstddef>
#include <string>
#include <unordered_map>
#include "stats.h"

struct HTMLParser {
  void parse(const std::string &url, const uint8_t* buf, size_t len, Stats* stats);
  void onAttributeValue(const uint8_t* buf, size_t len);
  void onAttributeName(const uint8_t* buf, size_t len);

private:
  std::unordered_map<std::string, size_t> attrName;
  std::unordered_map<std::string, size_t> attrValue;
  Stats* stats;
};
