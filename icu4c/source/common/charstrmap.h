// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#अन्तर्भाव <utility>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : सार्वजनिक UMemory {
सार्वजनिक:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(स्थिरांक CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &चालक=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        परत *this;
    }
    CharStringMap &चालक=(स्थिरांक CharStringMap &other) = delete;

    स्थिरांक अक्षर *get(स्थिरांक अक्षर *किल्ली) स्थिरांक { परत static_cast<स्थिरांक अक्षर *>(uhash_get(map, किल्ली)); }
    शून्य put(स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<अक्षर *>(किल्ली), const_cast<अक्षर *>(मूल्य_2), &errorCode);
    }

खाजगी:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
