/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallObject.h"

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Banner.h"
#include "ObjectJsonHelpers.h"

void WallObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.wall.tool_id = stream->ReadValue<uint8_t>();
    _legacyType.wall.flags = stream->ReadValue<uint8_t>();
    _legacyType.wall.height = stream->ReadValue<uint8_t>();
    _legacyType.wall.flags2 = stream->ReadValue<uint8_t>();
    _legacyType.wall.price = stream->ReadValue<uint16_t>();
    _legacyType.wall.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;
    stream->Seek(1, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.wall.scrolling_mode = stream->ReadValue<uint8_t>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(&sgEntry);

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.wall.price <= 0)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Price can not be free or negative.");
    }

    // Autofix this object (will be turned into an official object later).
    auto identifier = GetLegacyIdentifier();
    if (identifier == "XXWLBR03")
    {
        _legacyType.wall.flags2 &= ~WALL_SCENERY_2_DOOR_SOUND_MASK;
        _legacyType.wall.flags2 |= (1u << WALL_SCENERY_2_DOOR_SOUND_SHIFT) & WALL_SCENERY_2_DOOR_SOUND_MASK;
    }
}

void WallObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void WallObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void WallObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

    screenCoords.x += 14;
    screenCoords.y += (_legacyType.wall.height * 2) + 16;

    uint32_t imageId = 0x20D00000 | _legacyType.image;
    if (_legacyType.wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        imageId |= 0x92000000;
    }

    gfx_draw_sprite(dpi, imageId, screenCoords, 0);

    if (_legacyType.wall.flags & WALL_SCENERY_HAS_GLASS)
    {
        imageId = _legacyType.image + 0x44500006;
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
    }
    else if (_legacyType.wall.flags & WALL_SCENERY_IS_DOOR)
    {
        imageId++;
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
    }
}

void WallObject::ReadJson(IReadObjectContext* context, const json_t* root)
{
    auto properties = json_object_get(root, "properties");

    _legacyType.wall.tool_id = ObjectJsonHelpers::ParseCursor(
        ObjectJsonHelpers::GetString(properties, "cursor"), CURSOR_FENCE_DOWN);
    _legacyType.wall.height = json_integer_value(json_object_get(properties, "height"));
    _legacyType.wall.price = json_integer_value(json_object_get(properties, "price"));

    auto jScrollingMode = json_object_get(properties, "scrollingMode");
    _legacyType.wall.scrolling_mode = jScrollingMode != nullptr ? json_integer_value(jScrollingMode) : SCROLLING_MODE_NONE;

    SetPrimarySceneryGroup(ObjectJsonHelpers::GetString(json_object_get(properties, "sceneryGroup")));

    // Flags
    _legacyType.wall.flags = ObjectJsonHelpers::GetFlags<uint8_t>(
        properties,
        {
            { "hasPrimaryColour", WALL_SCENERY_HAS_PRIMARY_COLOUR },
            { "hasSecondaryColour", WALL_SCENERY_HAS_SECONDARY_COLOUR },
            { "hasTernaryColour", WALL_SCENERY_HAS_TERNARY_COLOUR },
            { "hasGlass", WALL_SCENERY_HAS_GLASS },
            { "isBanner", WALL_SCENERY_IS_BANNER },
            { "isDoor", WALL_SCENERY_IS_DOOR },
            { "isLongDoorAnimation", WALL_SCENERY_LONG_DOOR_ANIMATION },
        });
    _legacyType.wall.flags2 = ObjectJsonHelpers::GetFlags<uint8_t>(
        properties,
        {
            { "isOpaque", WALL_SCENERY_2_IS_OPAQUE },
            { "isAnimated", WALL_SCENERY_2_ANIMATED },
        });

    // HACK To avoid 'negated' properties in JSON, handle this separately until
    //      flag is inverted in this code base.
    if (!ObjectJsonHelpers::GetBoolean(properties, "isAllowedOnSlope", false))
    {
        _legacyType.wall.flags |= WALL_SCENERY_CANT_BUILD_ON_SLOPE;
    }

    // HACK WALL_SCENERY_HAS_PRIMARY_COLOUR actually means, has any colour but we simplify the
    //      JSON and handle this on load. We should change code base in future to reflect the JSON.
    if (!(_legacyType.wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR))
    {
        if ((_legacyType.wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
            || (_legacyType.wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR))
        {
            _legacyType.wall.flags |= WALL_SCENERY_HAS_PRIMARY_COLOUR;
            _legacyType.wall.flags2 |= WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR;
        }
    }

    // Door sound
    auto jDoorSound = json_object_get(properties, "doorSound");
    if (jDoorSound != nullptr)
    {
        auto doorSound = json_integer_value(jDoorSound);
        _legacyType.wall.flags2 |= (doorSound << WALL_SCENERY_2_DOOR_SOUND_SHIFT) & WALL_SCENERY_2_DOOR_SOUND_MASK;
    }

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
