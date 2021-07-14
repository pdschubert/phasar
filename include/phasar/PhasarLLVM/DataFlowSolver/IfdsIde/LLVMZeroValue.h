/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * LLVMZeroValue.h
 *
 *  Created on: 23.05.2017
 *      Author: philipp
 */

#ifndef PHASAR_PHASARLLVM_DATAFLOWSOLVER_IFDSIDE_LLVMZEROVALUE_H
#define PHASAR_PHASARLLVM_DATAFLOWSOLVER_IFDSIDE_LLVMZEROVALUE_H

#include <memory>

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Alignment.h"

namespace llvm {
class Value;
} // namespace llvm

namespace psr {

// do not touch, its only purpose is to make ZeroValue working
static const std::unique_ptr<llvm::LLVMContext>
    LLVMZeroValueCTX(new llvm::LLVMContext);
static const std::unique_ptr<llvm::Module>
    LLVMZeroValueMod(new llvm::Module("zero_module", *LLVMZeroValueCTX));

/**
 * This class may be used to represent the special zero value for IFDS
 * and IDE problems. The LLVMZeroValue is implemented as a singleton.
 */
class LLVMZeroValue : public llvm::GlobalVariable {
private:
  LLVMZeroValue()
      : llvm::GlobalVariable(
            *LLVMZeroValueMod, llvm::Type::getIntNTy(*LLVMZeroValueCTX, 2),
            true, llvm::GlobalValue::LinkageTypes::ExternalLinkage,
            llvm::ConstantInt::get(*LLVMZeroValueCTX,
                                   llvm::APInt(/*nbits*/ 2,
                                               /*value*/ 0,
                                               /*signed*/ true)),
            LLVMZeroValueInternalName) {
    setAlignment(llvm::MaybeAlign(4));
  }
  static constexpr char LLVMZeroValueInternalName[] = "zero_value";

public:
  LLVMZeroValue(const LLVMZeroValue &Z) = delete;
  LLVMZeroValue &operator=(const LLVMZeroValue &Z) = delete;
  LLVMZeroValue(LLVMZeroValue &&Z) = delete;
  LLVMZeroValue &operator=(LLVMZeroValue &&Z) = delete;

  [[nodiscard]] static llvm::StringRef getName() {
    return LLVMZeroValueInternalName;
  }

  static bool isLLVMZeroValue(const llvm::Value *V) {
    if (V && V->hasName()) {
      // checks if V's name start with "zero_value"
      return V->getName().find(LLVMZeroValueInternalName) !=
             llvm::StringRef::npos;
    }
    return false;
  }

  // Do not specify a destructor (at all)!
  static LLVMZeroValue *getInstance() {
    static auto *Zv = new LLVMZeroValue;
    return Zv;
  }
};
} // namespace psr

#endif
