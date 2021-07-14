/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#ifndef PHASAR_PHASARLLVM_DATAFLOWSOLVER_IFDSIDE_PROBLEMS_IDESOLVERTEST_H
#define PHASAR_PHASARLLVM_DATAFLOWSOLVER_IFDSIDE_PROBLEMS_IDESOLVERTEST_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "phasar/PhasarLLVM/DataFlowSolver/IfdsIde/IDETabulationProblem.h"
#include "phasar/PhasarLLVM/Domain/AnalysisDomain.h"

namespace llvm {
class Instruction;
class Function;
class StructType;
class Value;
} // namespace llvm

namespace psr {

class LLVMBasedICFG;
class LLVMTypeHierarchy;
class LLVMPointsToInfo;

struct IDESolverTestAnalysisDomain : public LLVMAnalysisDomainDefault {
  using l_t = const llvm::Value *;
};

class IDESolverTest : public IDETabulationProblem<IDESolverTestAnalysisDomain> {
private:
  std::vector<std::string> EntryPoints;

public:
  using IDETabProblemType = IDETabulationProblem<IDESolverTestAnalysisDomain>;
  using typename IDETabProblemType::d_t;
  using typename IDETabProblemType::f_t;
  using typename IDETabProblemType::i_t;
  using typename IDETabProblemType::l_t;
  using typename IDETabProblemType::n_t;
  using typename IDETabProblemType::t_t;
  using typename IDETabProblemType::v_t;

  IDESolverTest(const ProjectIRDB *IRDB, const LLVMTypeHierarchy *TH,
                const LLVMBasedICFG *ICF, LLVMPointsToInfo *PT,
                std::set<std::string> EntryPoints = {"main"});

  ~IDESolverTest() override = default;

  // start formulating our analysis by specifying the parts required for IFDS

  FlowFunctionPtrType getNormalFlowFunction(n_t Curr, n_t Succ) override;

  FlowFunctionPtrType getCallFlowFunction(n_t CallSite, f_t DestFun) override;

  FlowFunctionPtrType getRetFlowFunction(n_t CallSite, f_t CalleeFun,
                                         n_t ExitInst, n_t RetSite) override;

  FlowFunctionPtrType getCallToRetFlowFunction(n_t CallSite, n_t RetSite,
                                               std::set<f_t> Callees) override;

  FlowFunctionPtrType getSummaryFlowFunction(n_t CallSite,
                                             f_t DestFun) override;

  std::map<n_t, std::set<d_t>> initialSeeds() override;

  [[nodiscard]] d_t createZeroValue() const override;

  bool isZeroValue(d_t D) const override;

  // in addition provide specifications for the IDE parts

  std::shared_ptr<EdgeFunction<l_t>>
  getNormalEdgeFunction(n_t Curr, d_t CurrNode, n_t Succ,
                        d_t SuccNode) override;

  std::shared_ptr<EdgeFunction<l_t>>
  getCallEdgeFunction(n_t CallSite, d_t SrcNode, f_t DestinationFunction,
                      d_t DestNode) override;

  std::shared_ptr<EdgeFunction<l_t>>
  getReturnEdgeFunction(n_t CallSite, f_t CalleeFunction, n_t ExitInst,
                        d_t ExitNode, n_t ReSite, d_t RetNode) override;

  std::shared_ptr<EdgeFunction<l_t>>
  getCallToRetEdgeFunction(n_t CallSite, d_t CallNode, n_t RetSite,
                           d_t RetSiteNode, std::set<f_t> Callees) override;

  std::shared_ptr<EdgeFunction<l_t>>
  getSummaryEdgeFunction(n_t CallSite, d_t CallNode, n_t RetSite,
                         d_t RetSiteNode) override;

  l_t topElement() override;

  l_t bottomElement() override;

  l_t join(l_t Lhs, l_t Rhs) override;

  std::shared_ptr<EdgeFunction<l_t>> allTopFunction() override;

  class IDESolverTestAllTop
      : public EdgeFunction<l_t>,
        public std::enable_shared_from_this<IDESolverTestAllTop> {
    l_t computeTarget(l_t Source) override;

    std::shared_ptr<EdgeFunction<l_t>>
    composeWith(std::shared_ptr<EdgeFunction<l_t>> SecondFunction) override;

    std::shared_ptr<EdgeFunction<l_t>>
    joinWith(std::shared_ptr<EdgeFunction<l_t>> OtherFunction) override;

    bool equalTo(std::shared_ptr<EdgeFunction<l_t>> Other) const override;
  };

  void printNode(std::ostream &Os, n_t N) const override;

  void printDataFlowFact(std::ostream &Os, d_t D) const override;

  void printFunction(std::ostream &Os, f_t M) const override;

  void printEdgeFact(std::ostream &Os, l_t L) const override;
};

} // namespace psr

#endif
