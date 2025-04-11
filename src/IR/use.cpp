#include "IR/use.h"

namespace SiiIR {

UsePtr NewUse(SiiIRCode* user, ValuePtr value) {
  return std::make_shared< Use >(user, std::move(value));
}

}  // namespace SiiIR
