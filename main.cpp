#include "include/front/ASTPrinter.h"
#include "include/front/IR_generator.h"
#include "include/front/parser.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

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
    auto parser = front::CreateParser(file_name, input);
    auto AST = parser->work();
    auto IR_generator = front::CreateIRGenerator(AST);
    auto IR_list = IR_generator->work();
    std::stringstream result_builder;
    for (size_t i = 0; i < IR_list->size(); i++) {
      result_builder << (*IR_list)[i]->to_string();
      if (i != IR_list->size() - 1) {
        result_builder << "\n";
      }
    }
    std::cout << "IR of file: " << file_name << ": \n";
    std::cout << result_builder.str();
  }
}
