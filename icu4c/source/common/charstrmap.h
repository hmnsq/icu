// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#অন্তর্ভাৱ <utility>
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ধ্রুৱ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ঘূৰাই *this;
    }
    CharStringMap &operator=(ধ্রুৱ CharStringMap &other) = delete;

    ধ্রুৱ ৱর্ণ *get(ধ্রুৱ ৱর্ণ *চাবি) ধ্রুৱ { ঘূৰাই static_cast<ধ্রুৱ ৱর্ণ *>(uhash_get(map, চাবি)); }
    শূন্য put(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<ৱর্ণ *>(চাবি), const_cast<ৱর্ণ *>(মান_2), &errorCode);
    }

গুপ্ত:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
