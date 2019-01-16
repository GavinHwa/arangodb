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
/// @author Tobias Gödderz
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_AQL_ITEM_BLOCK_SHELL_H
#define ARANGOD_AQL_AQL_ITEM_BLOCK_SHELL_H

#include "Aql/AqlItemBlock.h"
#include "Aql/AqlItemBlockManager.h"

#include <memory>
#include <utility>

namespace arangodb {
namespace aql {

// Deleter usable for smart pointers that return an AqlItemBlock to its manager
class AqlItemBlockDeleter {
 public:
  explicit AqlItemBlockDeleter(AqlItemBlockManager& manager)
      : _manager(manager) {}

  void operator()(AqlItemBlock* block) { _manager.returnBlock(block); }

 private:
  AqlItemBlockManager& _manager;
};

/**
 * @brief This class is a decorator for AqlItemBlock.
 *
 * For one thing, it automatically returns the AqlItemBlock to the
 * AqlItemBlockManager upon destruction. If only this is needed, it would also
 * suffice to use the AqlItemBlockDeleter above and pass it as custom deleter
 * to a unique_ptr or shared_ptr.
 *
 * Secondly, Input- and OutputAqlItemBlockShell are aware of the registers that
 * are allowed to be read or written at the current ExecutionBlock, for usage
 * with InputAqlItemRow or OutputAqlItemRow, respectively.
 *
 * TODO We should do variable-to-register mapping here. This further reduces
 * dependencies of Executors, Fetchers etc. on internal knowledge of ItemBlocks,
 * and probably shrinks ExecutorInfos.
 */
class AqlItemBlockShell {
 public:
  using SmartAqlItemBlockPtr =
      std::unique_ptr<AqlItemBlock, AqlItemBlockDeleter>;

  AqlItemBlock const& block() const { return *_block; };
  AqlItemBlock& block() { return *_block; };

 protected:
  // This could be made public, but first, there's currently no use for that.
  // And second, just creating a
  // std::unique_ptr<AqlItemBlock, AqlItemBlockDeleter> would accomplish the
  // same.
  AqlItemBlockShell(AqlItemBlockManager& manager,
                    std::unique_ptr<AqlItemBlock> block);

 protected:
  SmartAqlItemBlockPtr _block;
};

class InputAqlItemBlockShell : public AqlItemBlockShell {
 public:
  InputAqlItemBlockShell(
      AqlItemBlockManager& manager, std::unique_ptr<AqlItemBlock> block,
      std::shared_ptr<const std::unordered_set<RegisterId>> inputRegisters);

 public:
  std::unordered_set<RegisterId> const& inputRegisters() const {
    return *_inputRegisters;
  };

  bool isInputRegister(RegisterId registerId) const {
    return inputRegisters().find(registerId) != inputRegisters().end();
  }

  /**
   * @brief Compares blocks by pointer.
   */
  bool operator==(InputAqlItemBlockShell const& other) const {
    // There must be only one AqlItemBlockShell instance per AqlItemBlock
    TRI_ASSERT((this == &other) == (&block() == &other.block()));
    return &block() == &other.block();
  }

 private:
  std::shared_ptr<const std::unordered_set<RegisterId>> _inputRegisters;
};

class OutputAqlItemBlockShell : public AqlItemBlockShell {
 public:
  // TODO This constructor would be able to fetch a new block itself from the
  // manager, which is needed anyway. Maybe, at least additionally, we should
  // write a constructor that takes the block dimensions instead of the block
  // itself for convenience.
  OutputAqlItemBlockShell(
      AqlItemBlockManager& manager, std::unique_ptr<AqlItemBlock> block,
      std::shared_ptr<const std::unordered_set<RegisterId>> outputRegisters,
      std::shared_ptr<const std::unordered_set<RegisterId>> registersToKeep);

 public:
  std::unordered_set<RegisterId> const& outputRegisters() const {
    return *_outputRegisters;
  };

  std::unordered_set<RegisterId> const& registersToKeep() const {
    return *_registersToKeep;
  };

  bool isOutputRegister(RegisterId registerId) const {
    return outputRegisters().find(registerId) != outputRegisters().end();
  }

  /**
   * @brief Steals the block, in a backwards-compatible unique_ptr. The shell
   *        is broken after this.
   */
  std::unique_ptr<AqlItemBlock> stealBlockCompat() {
    return std::unique_ptr<AqlItemBlock>(_block.release());
  }

 private:
  std::shared_ptr<const std::unordered_set<RegisterId>> _outputRegisters;
  std::shared_ptr<const std::unordered_set<RegisterId>> _registersToKeep;
};

}  // namespace aql
}  // namespace arangodb

#endif  // ARANGOD_AQL_AQL_ITEM_BLOCK_SHELL_H
