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

struct rct_drawpixelinfo;

namespace OpenRCT2::Drawing
{
    struct IRainDrawer;
}

// clang-format off
static constexpr const uint8_t RainPattern[] =
{
    32, 32, 0, 12, 0, 14, 0, 16, 255, 0, 255, 0, 255, 0, 255, 0, 255,
    0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
    255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
    0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 0, 0
};
// clang-format on

void DrawRain(rct_drawpixelinfo* dpi, OpenRCT2::Drawing::IRainDrawer* rainDrawer);
