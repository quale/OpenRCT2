/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "Object.h"

class FootpathObject final : public Object
{
private:
    rct_footpath_entry _legacyType = {};
    PathSurfaceEntry _pathSurfaceEntry = {};
    PathSurfaceEntry _queueEntry = {};
    PathRailingsEntry _pathRailingsEntry = {};

public:
    explicit FootpathObject(const rct_object_entry& entry)
        : Object(entry)
    {
    }

    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    PathSurfaceEntry* GetPathSurfaceEntry()
    {
        return &_pathSurfaceEntry;
    }

    PathSurfaceEntry* GetQueueEntry()
    {
        return &_queueEntry;
    }

    PathRailingsEntry* GetPathRailingsEntry()
    {
        return &_pathRailingsEntry;
    }

    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void ReadJson(IReadObjectContext* context, const json_t* root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;
};
