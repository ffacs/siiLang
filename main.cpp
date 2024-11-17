#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include "include/parser.h"

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "error: no input files\n";
    exit(0);
  }
  std::vector<std::unique_ptr<Parser>> parsers;
  for (int i = 1; i < argc; i++) {
    std::string file_name(argv[i], std::strlen(argv[i]));
    std::ifstream input(file_name);
    if (!input.is_open()) {
      std::cerr << "Failed to open: " << file_name << "\n";
      exit(0);
    }
    parsers.emplace_back(create_parser(input, std::cout));
    parsers.back()->work();
  }
}
