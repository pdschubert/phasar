#include <Types/BaseType.h>
#include <Types/PointerType.h>

namespace CCPP {
namespace Types {

// We decided to not make pointers const
PointerType::PointerType(std::shared_ptr<Type> &underlying)
    : Type(underlying->getName() + "*", false), underlying(underlying) {}

virtual bool PointerType::isPointerType() const override { return true; }
std::shared_ptr<Type> &PointerType::getPointerElementType() const {
  return underlying;
}
virtual bool PointerType::equivalent(Type *other) const override {
  if (other->isPointerType()) {
    auto ty = (PointerType *)other;
    return underlying->equivalent(ty->underlying.get());
  }
  return false;
}
virtual bool PointerType::canBeAssignedTo(Type *other) const override {
  if (other->isPointerType()) {
    auto ty = (PointerType *)other;
    // assign every pointer to void*
    if (ty->underlying->isPrimitiveType()) {
      auto prim = ((BaseType *)ty->underlying.get())->getPrimitiveType();
      if (prim == Type::PrimitiveType::VOID)
        return true;
    }
    // assign nullptr or NULL to every pointer
    else if (underlying->isPrimitiveType()) {
      auto prim = ((BaseType *)underlying.get())->getPrimitiveType();
      if (prim == Type::PrimitiveType::NULL_T)
        return true;
    } else if (underlying->isConstant()) // covariance only with constant types
      return underlying->canBeAssignedTo(ty->underlying.get());
    else
      return underlying->equivalent(ty->underlying.get());
  }
  return false;
}
} // namespace Types
} // namespace CCPP