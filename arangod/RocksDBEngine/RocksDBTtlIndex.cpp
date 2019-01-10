////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "RocksDBTtlIndex.h"
#include "Basics/StaticStrings.h"
#include "VocBase/LogicalCollection.h"

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

using namespace arangodb;
  
RocksDBTtlIndex::RocksDBTtlIndex(
    TRI_idx_iid_t iid,
    LogicalCollection& coll,
    arangodb::velocypack::Slice const& info
)
    : RocksDBSkiplistIndex(iid, coll, info),
      _expireAfter(info.get(StaticStrings::IndexExpireAfter).getNumericValue<double>()) {}

void RocksDBTtlIndex::toVelocyPack(arangodb::velocypack::Builder& builder,
                                   std::underlying_type<Index::Serialize>::type flags) const {
  builder.openObject();
  RocksDBIndex::toVelocyPack(builder, flags);
  builder.add(
    arangodb::StaticStrings::IndexUnique, arangodb::velocypack::Value(false)
  );
  builder.add(
    arangodb::StaticStrings::IndexSparse, arangodb::velocypack::Value(false)
  );
  builder.add(StaticStrings::IndexExpireAfter, VPackValue(_expireAfter));
  builder.close();
}
