// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#অন্তর্ভাব <utility>
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : সার্বজনিক UMemory {
সার্বজনিক:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ধ্রুব CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        প্রত্যাগম *this;
    }
    CharStringMap &operator=(ধ্রুব CharStringMap &other) = delete;

    ধ্রুব বর্ণ *get(ধ্রুব বর্ণ *চাবি) ধ্রুব { প্রত্যাগম static_cast<ধ্রুব বর্ণ *>(uhash_get(map, চাবি)); }
    শূন্য put(ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<বর্ণ *>(চাবি), const_cast<বর্ণ *>(মান_2), &errorCode);
    }

গুপ্ত:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
