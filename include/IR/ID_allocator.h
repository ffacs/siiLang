#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace SiiIR {
struct Value;

class IDAllocator {
public:
  IDAllocator() {}
  std::string alloc( const Value* v );

private:
  int64_t alloc_id( const Value* v ) {
    auto [ iter, inserted ]
        = allocated_ids_.insert( { v, allocated_ids_.size() } );
    return iter->second;
  }
  std::unordered_map< const Value*, int64_t > allocated_ids_;
};

using IDAllocatorPtr = std::shared_ptr< IDAllocator >;

}  // namespace SiiIR
