/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <string_view>

struct IObjectRepository;
class Object;
struct rct_object_entry;

namespace ObjectFactory
{
    Object* CreateObjectFromLegacyFile(IObjectRepository& objectRepository, const utf8* path);
    Object* CreateObjectFromLegacyData(
        IObjectRepository& objectRepository, const rct_object_entry* entry, const void* data, size_t dataSize);
    Object* CreateObjectFromZipFile(IObjectRepository& objectRepository, const std::string_view& path);
    Object* CreateObject(const rct_object_entry& entry);

    Object* CreateObjectFromJsonFile(IObjectRepository& objectRepository, const std::string& path);
} // namespace ObjectFactory
