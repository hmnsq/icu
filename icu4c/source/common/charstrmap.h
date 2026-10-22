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
class CharStringMap final : সর্বজনীন UMemory {
সর্বজনীন:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t আকার_3, UErrorCode &errorCode) {
        map = uhash_open_আকার(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             আকার_3, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ধ্রুবক CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ফেরত *this;
    }
    CharStringMap &operator=(ধ্রুবক CharStringMap &other) = delete;

    ধ্রুবক অক্ষর *get(ধ্রুবক অক্ষর *চাবি) ধ্রুবক { ফেরত static_cast<ধ্রুবক অক্ষর *>(uhash_get(map, চাবি)); }
    শূন্য put(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<অক্ষর *>(চাবি), const_cast<অক্ষর *>(মান_2), &errorCode);
    }

ব্যক্তিগত:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
