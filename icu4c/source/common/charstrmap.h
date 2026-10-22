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
    CharStringMap(अचर CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &चालक=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        फर्काऊ *this;
    }
    CharStringMap &चालक=(अचर CharStringMap &other) = delete;

    अचर वर्ण *get(अचर वर्ण *साँचो) अचर { फर्काऊ static_cast<अचर वर्ण *>(uhash_get(map, साँचो)); }
    शून्य put(अचर वर्ण *साँचो, अचर वर्ण *मान_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<वर्ण *>(साँचो), const_cast<वर्ण *>(मान_2), &errorCode);
    }

गुप्त:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
