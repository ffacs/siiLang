#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include "include/parser.h"
#include "include/IR_generator.h"

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "error: no input files\n";
    exit(0);
  }
  for (int i = 1; i < argc; i++) {
    std::string file_name(argv[i], std::strlen(argv[i]));
    std::ifstream input(file_name);
    if (!input.is_open()) {
      std::cerr << "Failed to open: " << file_name << "\n";
      exit(0);
    }
    auto parser = CreateParser(input);
    auto IR_generator = CreateIRGenerator(parser->work());
    auto IR_list = IR_generator->work();
    std::stringstream result_builder;
    for (size_t i = 0; i < IR_list.size(); i++) {
      result_builder << IR_list[i]->to_string();
      if (i != IR_list.size() - 1) {
        result_builder << "\n";
      }
    }
    std::cout << "IR of file: " << file_name << ": \n";
    std::cout << result_builder.str();
  }
}
