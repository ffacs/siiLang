#include "IR/address.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
struct FunctionContext {
  std::vector<AddressPtr> allocated_addresses_;

  LabelPtr allocate_label();
  TemporaryAddressPtr allocate_temporary_address();
  VariableAddressPtr allocate_variable_address();
};

} // namespace SiiIR