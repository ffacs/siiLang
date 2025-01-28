#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace front {
struct LexPosition {
  uint64_t begin_index_;
  uint64_t begin_line_;
  uint64_t begin_column_;

  LexPosition(uint64_t begin_index, uint64_t begin_line, uint64_t begin_column)
      : begin_index_(begin_index), begin_line_(begin_line),
        begin_column_(begin_column) {}
};

struct LexInfo {
  LexPosition position_;
  uint64_t length_;

  LexInfo() : position_(0, 0, 0), length_(0) {}
  LexInfo(LexPosition position, uint64_t length)
      : position_(position), length_(length) {}
};

enum class DiagnoseLevel { kError, kWarning, kInfo };

class DiagnoseHandler {
public:
  DiagnoseHandler(std::string file_name, std::string_view content)
      : file_name_(std::move(file_name)), content_(content) {}
  virtual void mismatch(DiagnoseLevel level, LexInfo lex_info,
                        std::string expected, std::string found) = 0;
  virtual void unexpect(DiagnoseLevel level, LexInfo lex_info,
                        std::string message) = 0;
  virtual ~DiagnoseHandler() {}

protected:
  std::string file_name_;
  std::string_view content_;
};

using DiagnoseHandlerPtr = std::shared_ptr<DiagnoseHandler>;

DiagnoseHandlerPtr CreateDiagnoseHandler(std::string filename,
                                         std::string_view content);

} // namespace front