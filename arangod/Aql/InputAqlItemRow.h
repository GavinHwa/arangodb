////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018-2018 ArangoDB GmbH, Cologne, Germany
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
/// @author Michael Hackstein
/// @author Heiko Kernbach
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_INPUT_AQL_ITEM_ROW_H
#define ARANGOD_AQL_INPUT_AQL_ITEM_ROW_H 1

#include "Aql/types.h"
#include "Basics/Common.h"

namespace arangodb {
namespace aql {

class InputAqlItemBlockShell;
class AqlItemBlock;
struct AqlValue;

struct CreateInvalidInputRowHint {
  // Forbid creating this via `{}`
  explicit CreateInvalidInputRowHint() = default;
};

/**
 * @brief One row within an AqlItemBlock, for reading.
 *
 * Does not keep a reference to the data.
 * Caller needs to make sure that the underlying
 * AqlItemBlock is not going out of scope.
 *
 * Note that this class will be copied a lot, and therefore should be small
 * and not do too complex things when copied!
 */
class InputAqlItemRow {
 public:
  // The default constructor contains an invalid item row
  explicit InputAqlItemRow(CreateInvalidInputRowHint);

  InputAqlItemRow(std::shared_ptr<InputAqlItemBlockShell> blockShell,
                  size_t baseIndex);

  /**
   * @brief Get a reference to the value of the given Variable Nr
   *
   * @param registerId The register ID of the variable to read.
   *
   * @return Reference to the AqlValue stored in that variable.
   */
  const AqlValue& getValue(RegisterId registerId) const;

  std::size_t getNrRegisters() const;

  bool operator==(InputAqlItemRow const& other) const noexcept;
  bool operator!=(InputAqlItemRow const& other) const noexcept;

  bool isInitialized() const noexcept { return _blockShell != nullptr; }

  explicit operator bool() const noexcept { return isInitialized(); }

 private:
  AqlItemBlock& block();
  AqlItemBlock const& block() const;

 private:
  /**
   * @brief Underlying AqlItemBlock storing the data.
   */
  std::shared_ptr<InputAqlItemBlockShell> _blockShell;

  /**
   * @brief The offset into the AqlItemBlock. In other words, the row's index.
   */
  size_t _baseIndex;
};

}  // namespace aql
}  // namespace arangodb
#endif
