#include "front/diagnose.h"
#include <iostream>
#include <sstream>
#include <vector>

namespace front {

class DiagnoseHandlerImpl : public DiagnoseHandler {
public:
  DiagnoseHandlerImpl(std::string file_name, std::string_view content)
      : DiagnoseHandler(std::move(file_name), content) {
    size_t begin = 0;
    for (size_t i = 0; i < content.size(); i++) {
      if (content[i] == '\n') {
        lines_.push_back(content.substr(begin, i - begin));
        begin = i + 1;
      }
    }
    if (begin < content.size()) {
      lines_.push_back(content.substr(begin));
    }
  }

  void mismatch(DiagnoseLevel level, LexInfo lex_info, std::string expected,
                std::string found) override {
    handle(level, lex_info, "Expected " + expected + ", but found " + found);
  }

  void unexpect(DiagnoseLevel level, LexInfo lex_info,
                std::string message) override {
    handle(level, lex_info, message);
  }

private:
  void handle(DiagnoseLevel level, LexInfo lex_info, std::string message) {
    std::stringstream diagnose_msg;
    switch (level) {
    case DiagnoseLevel::kError:
      diagnose_msg << "Error: ";
      break;
    case DiagnoseLevel::kWarning:
      diagnose_msg << "Warning: ";
      break;
    case DiagnoseLevel::kInfo:
      diagnose_msg << "Info: ";
      break;
    }
    // Print diagnose message, file_name, line number and column number
    diagnose_msg << message << " in " << file_name_ << ":"
                 << lex_info.position_.begin_line_ << ":"
                 << lex_info.position_.begin_column_ << std::endl;
    // Print the line of code where the diagnose message is
    diagnose_msg << lines_[lex_info.position_.begin_line_ - 1] << std::endl;
    // Print the pointer to the column where the diagnose message is
    diagnose_msg << std::string(lex_info.position_.begin_column_ - 1, ' ')
                 << '^' << std::endl;
    if (level == DiagnoseLevel::kError) {
      throw std::invalid_argument(diagnose_msg.str());
    }
  }

protected:
  std::vector<std::string_view> lines_;
};

DiagnoseHandlerPtr CreateDiagnoseHandler(std::string file_name,
                                         std::string_view content) {
  return std::make_shared<DiagnoseHandlerImpl>(std::move(file_name), content);
}

} // namespace front