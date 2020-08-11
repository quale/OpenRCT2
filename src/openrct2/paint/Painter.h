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
#include "Paint.h"

#include <ctime>
#include <memory>
#include <vector>

struct rct_drawpixelinfo;

namespace OpenRCT2
{
    namespace Drawing
    {
        struct IDrawingEngine;
    } // namespace Drawing

    namespace Ui
    {
        struct IUiContext;
    } // namespace Ui

    namespace Paint
    {
        struct Painter final
        {
        private:
            std::shared_ptr<Ui::IUiContext> const _uiContext;
            std::vector<std::unique_ptr<paint_session>> _paintSessionPool;
            std::vector<paint_session*> _freePaintSessions;
            time_t _lastSecond = 0;
            int32_t _currentFPS = 0;
            int32_t _frames = 0;

        public:
            explicit Painter(const std::shared_ptr<Ui::IUiContext>& uiContext);
            void Paint(Drawing::IDrawingEngine& de);

            paint_session* CreateSession(rct_drawpixelinfo* dpi, uint32_t viewFlags);
            void ReleaseSession(paint_session* session);

        private:
            void PaintReplayNotice(rct_drawpixelinfo* dpi, const char* text);
            void PaintFPS(rct_drawpixelinfo* dpi);
            void MeasureFPS();
        };
    } // namespace Paint
} // namespace OpenRCT2
