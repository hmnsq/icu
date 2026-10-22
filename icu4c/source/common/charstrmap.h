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
    CharStringMap(ध्रुव CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        पटिनिवत्त *this;
    }
    CharStringMap &operator=(ध्रुव CharStringMap &other) = delete;

    ध्रुव वर्ण *get(ध्रुव वर्ण *कुञ्चिका) ध्रुव { पटिनिवत्त static_cast<ध्रुव वर्ण *>(uhash_get(map, कुञ्चिका)); }
    शून्य put(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<वर्ण *>(कुञ्चिका), const_cast<वर्ण *>(मानम्_2), &errorCode);
    }

गुप्त:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
