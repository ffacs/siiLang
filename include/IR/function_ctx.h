#include "IR/address.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
struct FunctionContext {
  std::vector<TemporaryAddressPtr> temporary_addresses_;
  std::vector<VariableAddressPtr> variable_addresses_;

  TemporaryAddressPtr allocate_temporary_address();
  VariableAddressPtr allocate_variable_address();
  void rename_all_addresses();
};

} // namespace SiiIR