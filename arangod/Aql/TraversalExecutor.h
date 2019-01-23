////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_TRAVERSAL_EXECUTOR_H
#define ARANGOD_AQL_TRAVERSAL_EXECUTOR_H

#include "Aql/ExecutionState.h"
#include "Aql/ExecutorInfos.h"
#include "Aql/InputAqlItemRow.h"
#include "Aql/TraversalStats.h"

namespace arangodb {

namespace traverser {
class Traverser;
}

namespace aql {

class Query;
class OutputAqlItemRow;
class ExecutorInfos;
class SingleRowFetcher;

class TraversalExecutorInfos : public ExecutorInfos {
 public:
  enum OutputName { VERTEX, EDGE, PATH };

  TraversalExecutorInfos(std::shared_ptr<std::unordered_set<RegisterId>> inputRegisters,
                         std::shared_ptr<std::unordered_set<RegisterId>> outputRegisters,
                         RegisterId nrInputRegisters, RegisterId nrOutputRegisters,
                         std::unordered_set<RegisterId> registersToClear,
                         std::unique_ptr<traverser::Traverser>&& traverser,
                         std::unordered_map<OutputName, RegisterId> registerMapping,
                         std::string const& fixedSource, RegisterId inputRegister,
                         std::vector<std::pair<Variable const*, RegisterId>> filterConditionVariables);

  TraversalExecutorInfos() = delete;

  TraversalExecutorInfos(TraversalExecutorInfos&&);
  TraversalExecutorInfos(TraversalExecutorInfos const&) = delete;
  ~TraversalExecutorInfos();

  traverser::Traverser& traverser();

  bool useVertexOutput() const;

  RegisterId vertexRegister() const;

  bool useEdgeOutput() const;

  RegisterId edgeRegister() const;

  bool usePathOutput() const;

  RegisterId pathRegister() const;

  bool usesFixedSource() const;

  std::string const& getFixedSource() const;

  RegisterId getInputRegister() const;

  std::vector<std::pair<Variable const*, RegisterId>> const& filterConditionVariables() const;

 private:
  std::unique_ptr<traverser::Traverser> _traverser;
  std::unordered_map<OutputName, RegisterId> _registerMapping;
  std::string const _fixedSource;
  RegisterId _inputRegister;
  std::vector<std::pair<Variable const*, RegisterId>> _filterConditionVariables;
};

/**
 * @brief Implementation of Traversal Node
 */
class TraversalExecutor {
 public:
  using Fetcher = SingleRowFetcher;
  using Infos = TraversalExecutorInfos;
  using Stats = TraversalStats;

  TraversalExecutor() = delete;
  TraversalExecutor(TraversalExecutor&&) = default;
  TraversalExecutor(TraversalExecutor const&) = default;
  TraversalExecutor(Fetcher& fetcher, Infos&);
  ~TraversalExecutor();

  /**
   * @brief Shutdown will be called once for every query
   *
   * @return ExecutionState and no error.
   */
  std::pair<ExecutionState, Result> shutdown(int errorCode);

  /**
   * @brief produce the next Row of Aql Values.
   *
   * @return ExecutionState, and if successful exactly one new Row of AqlItems.
   */
  std::pair<ExecutionState, Stats> produceRow(OutputAqlItemRow& output);

 private:
  /**
   * @brief compute the return state
   * @return DONE if traverser and remote are both done, HASMORE otherwise
   */
  ExecutionState computeState() const;

  void resetTraverser();

 private:
  Infos& _infos;
  Fetcher& _fetcher;
  InputAqlItemRow _input;
  ExecutionState _rowState;
  traverser::Traverser& _traverser;
};

}  // namespace aql
}  // namespace arangodb

#endif
