/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * PluginTest.h
 *
 *  Created on: 14.06.2017
 *      Author: philipp
 */

#ifndef SRC_ANALYSIS_PLUGINS_IFDSSIMPLETAINTANALYSIS_H_
#define SRC_ANALYSIS_PLUGINS_IFDSSIMPLETAINTANALYSIS_H_

#include <phasar/PhasarLLVM/Plugins/Interfaces/IfdsIde/IFDSTabulationProblemPlugin.h>

namespace psr {
struct ValueFlowFactWrapper : public FlowFactWrapper<const llvm::Value *> {

  using FlowFactWrapper::FlowFactWrapper;

  void print(std::ostream &OS,
             const llvm::Value *const &NonZeroFact) const override {
    OS << llvmIRToShortString(NonZeroFact) << '\n';
  }
};

class IFDSSimpleTaintAnalysis : public IFDSTabulationProblemPlugin {
  FlowFactManager<ValueFlowFactWrapper> ffManager;

public:
  IFDSSimpleTaintAnalysis(const ProjectIRDB *IRDB, const LLVMTypeHierarchy *TH,
                          const LLVMBasedICFG *ICF, LLVMPointsToInfo *PT,
                          std::set<std::string> EntryPoints = {});
  ~IFDSSimpleTaintAnalysis() = default;

  const FlowFact *createZeroValue() const override;

  FlowFunctionPtrType
  getNormalFlowFunction(const llvm::Instruction *curr,
                        const llvm::Instruction *succ) override;

  FlowFunctionPtrType
  getCallFlowFunction(const llvm::Instruction *callSite,
                      const llvm::Function *destFun) override;

  FlowFunctionPtrType
  getRetFlowFunction(const llvm::Instruction *callSite,
                     const llvm::Function *calleeFun,
                     const llvm::Instruction *exitSite,
                     const llvm::Instruction *retSite) override;

  FlowFunctionPtrType
  getCallToRetFlowFunction(const llvm::Instruction *callSite,
                           const llvm::Instruction *retSite,
                           std::set<const llvm::Function *> callees) override;

  FlowFunctionPtrType
  getSummaryFlowFunction(const llvm::Instruction *callSite,
                         const llvm::Function *destFun) override;

  InitialSeeds<const llvm::Instruction *, const FlowFact *, BinaryDomain>
  initialSeeds() override;
};

extern "C" std::unique_ptr<IFDSTabulationProblemPlugin>
makeIFDSSimpleTaintAnalysis(LLVMBasedICFG &I,
                            std::vector<std::string> EntryPoints);
} // namespace psr

#endif /* SRC_ANALYSIS_PLUGINS_IFDSSIMPLETAINTANALYSIS_H_ */
