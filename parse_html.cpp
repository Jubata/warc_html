#include "html_parser.h"
#include <sstream>
#include <iostream>

int main(int argc, char* argv[]) {
  Stats stats;

  std::stringstream ss;
  ss << std::cin.rdbuf();
  // auto buf = std::cin.rdbuf();
  // ss.str().c_str();
  
  HTMLParser parser;
  parser.parse("http://dummy.com/", reinterpret_cast<const uint8_t*>(ss.str().c_str()), ss.str().size(), &stats);

  stats.print_json();

  return 0;
}