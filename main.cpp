#include "include/IR/Pass/memory_to_register.h"
#include "include/IR/function.h"
#include "include/front/ASTPrinter.h"
#include "include/front/IR_generator.h"
#include "include/front/parser.h"
#include <cstring>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  if(argc <= 1) {
    std::cerr << "error: no input files\n";
    exit(0);
  }
  for(int i = 1; i < argc; i++) {
    std::string   file_name(argv[i], std::strlen(argv[i]));
    std::ifstream input(file_name);
    if(!input.is_open()) {
      std::cerr << "Failed to open: " << file_name << "\n";
      exit(0);
    }
    auto parser       = front::CreateParser(file_name, input);
    auto AST          = parser->work();
    auto IR_generator = front::CreateIRGenerator(AST);
    auto IR_list      = IR_generator->work();
    for(auto& IR: *IR_list) {
      if(IR->kind_ == SiiIR::SiiIRCodeKind::FUNCTION_DEFINITION) {
        SiiIR::SiiIRFunctionDefinition* function_definition
            = static_cast<SiiIR::SiiIRFunctionDefinition*>(IR.get());
        SiiIR::FunctionPtr func = SiiIR::BuildFunction(
            std::move(*function_definition->function_->codes_),
            std::move(function_definition->function_->ctx_),
            std::move(function_definition->function_->name_));
        SiiIR::MemoryToRegisterPass().run(func);
        std::cout << func->to_string() << std::endl;
      } else {
        throw std::runtime_error("Not a function definition");
      }
    }
  }
}
