/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StringTable.h"

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/LanguagePack.h"
#include "../localisation/LocalisationService.h"
#include "Object.h"

#include <algorithm>

static constexpr const uint8_t RCT2ToOpenRCT2LanguageId[] = {
    LANGUAGE_ENGLISH_UK,
    LANGUAGE_ENGLISH_US,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN,
    LANGUAGE_SPANISH,
    LANGUAGE_ITALIAN,
    LANGUAGE_DUTCH,
    LANGUAGE_SWEDISH,
    LANGUAGE_JAPANESE,
    LANGUAGE_KOREAN,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_CHINESE_TRADITIONAL,
    LANGUAGE_UNDEFINED,
    LANGUAGE_PORTUGUESE_BR,
};

static bool StringIsBlank(const utf8* str)
{
    for (auto ch = str; *ch != '\0'; ch++)
    {
        if (!isblank(static_cast<uint8_t>(*ch)))
        {
            return false;
        }
    }
    return true;
}

void StringTable::Read(IReadObjectContext* context, OpenRCT2::IStream* stream, uint8_t id)
{
    try
    {
        RCT2LanguageId rct2LanguageId;
        while ((rct2LanguageId = static_cast<RCT2LanguageId>(stream->ReadValue<uint8_t>())) != RCT2_LANGUAGE_ID_END)
        {
            uint8_t languageId = (rct2LanguageId <= RCT2_LANGUAGE_ID_PORTUGUESE) ? RCT2ToOpenRCT2LanguageId[rct2LanguageId]
                                                                                 : static_cast<uint8_t>(LANGUAGE_UNDEFINED);
            std::string stringAsWin1252 = stream->ReadStdString();
            auto stringAsUtf8 = rct2_to_utf8(stringAsWin1252, rct2LanguageId);

            if (!StringIsBlank(stringAsUtf8.data()))
            {
                stringAsUtf8 = String::Trim(stringAsUtf8);
                StringTableEntry entry{};
                entry.Id = id;
                entry.LanguageId = languageId;
                entry.Text = stringAsUtf8;
                _strings.push_back(entry);
            }
        }
    }
    catch (const std::exception&)
    {
        context->LogError(OBJECT_ERROR_BAD_STRING_TABLE, "Bad string table.");
        throw;
    }
    Sort();
}

std::string StringTable::GetString(uint8_t id) const
{
    for (auto& string : _strings)
    {
        if (string.Id == id)
        {
            return string.Text;
        }
    }
    return std::string();
}

std::string StringTable::GetString(uint8_t language, uint8_t id) const
{
    for (auto& string : _strings)
    {
        if (string.LanguageId == language && string.Id == id)
        {
            return string.Text;
        }
    }
    return std::string();
}

void StringTable::SetString(uint8_t id, uint8_t language, const std::string& text)
{
    StringTableEntry entry;
    entry.Id = id;
    entry.LanguageId = language;
    entry.Text = text;
    _strings.push_back(entry);
}

void StringTable::Sort()
{
    auto targetLanguage = LocalisationService_GetCurrentLanguage();
    std::sort(_strings.begin(), _strings.end(), [targetLanguage](const StringTableEntry& a, const StringTableEntry& b) -> bool {
        if (a.Id == b.Id)
        {
            if (a.LanguageId == b.LanguageId)
            {
                return String::Compare(a.Text, b.Text, true) < 0;
            }

            if (a.LanguageId == targetLanguage)
            {
                return true;
            }
            if (b.LanguageId == targetLanguage)
            {
                return false;
            }

            if (a.LanguageId == LANGUAGE_ENGLISH_UK)
            {
                return true;
            }
            if (b.LanguageId == LANGUAGE_ENGLISH_UK)
            {
                return false;
            }

            return a.LanguageId < b.LanguageId;
        }
        return a.Id < b.Id;
    });
}
