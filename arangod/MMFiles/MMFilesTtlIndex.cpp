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

#include "MMFilesTtlIndex.h"
#include "Basics/StaticStrings.h"

#include <velocypack/velocypack-aliases.h>

using namespace arangodb;

MMFilesTtlIndex::MMFilesTtlIndex(
    TRI_idx_iid_t iid,
    arangodb::LogicalCollection& collection,
    arangodb::velocypack::Slice const& info
)
    : MMFilesSkiplistIndex(iid, collection, info),
      _expireAfter(info.get(StaticStrings::IndexExpireAfter).getNumericValue<double>()) {}

MMFilesTtlIndex::~MMFilesTtlIndex() {}

void MMFilesTtlIndex::toVelocyPack(arangodb::velocypack::Builder& builder,
                                   std::underlying_type<Index::Serialize>::type flags) const {
  builder.openObject();
  Index::toVelocyPack(builder, flags);
  builder.add(
    arangodb::StaticStrings::IndexUnique,
    arangodb::velocypack::Value(_unique)
  );
  builder.add(
    arangodb::StaticStrings::IndexSparse,
    arangodb::velocypack::Value(_sparse)
  );
  builder.add(StaticStrings::IndexExpireAfter, VPackValue(_expireAfter));
  builder.close();
}
