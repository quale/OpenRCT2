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
#include "../drawing/Drawing.h"

#include <memory>
#include <vector>

struct IReadObjectContext;
namespace OpenRCT2
{
    struct IStream;
}

class ImageTable
{
private:
    std::unique_ptr<uint8_t[]> _data;
    std::vector<rct_g1_element> _entries;

public:
    ImageTable() = default;
    ImageTable(const ImageTable&) = delete;
    ImageTable& operator=(const ImageTable&) = delete;
    ~ImageTable();

    void Read(IReadObjectContext* context, OpenRCT2::IStream* stream);
    const rct_g1_element* GetImages() const
    {
        return _entries.data();
    }
    uint32_t GetCount() const
    {
        return static_cast<uint32_t>(_entries.size());
    }
    void AddImage(const rct_g1_element* g1);
};
