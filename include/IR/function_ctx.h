#include "IR/address.h"
#include "IR/type.h"

namespace SiiIR {
struct FunctionContext;
typedef std::shared_ptr<FunctionContext> FunctionContextPtr;
struct FunctionContext {
  std::vector<AddressPtr> allocated_addresses_;

  LabelPtr allocate_label();
  TemporaryAddressPtr allocate_temporary_address(TypePtr type);
  VariableAddressPtr allocate_variable_address(TypePtr type);
};

} // namespace SiiIR