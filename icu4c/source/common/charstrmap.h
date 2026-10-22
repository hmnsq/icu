// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#यदि_अपरिभाषितम् __CHARSTRMAP_H__
#परिभाषय __CHARSTRMAP_H__

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
    CharStringMap(int32_t आकार_2, UError_संहिता &error_संहिता) {
        map = uhash_open_आकार(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             आकार_2, &error_संहिता);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ध्रुव CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &संक्रियकः=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        प्रत्यागम *this;
    }
    CharStringMap &संक्रियकः=(ध्रुव CharStringMap &other) = delete;

    ध्रुव वर्ण *get(ध्रुव वर्ण *कुञ्चिका) ध्रुव { प्रत्यागम static_cast<ध्रुव वर्ण *>(uhash_get(map, कुञ्चिका)); }
    शून्य put(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्_2, UError_संहिता &error_संहिता) {
        uhash_put(map, const_cast<वर्ण *>(कुञ्चिका), const_cast<वर्ण *>(मानम्_2), &error_संहिता);
    }

गुप्त:
    UHashtable *map;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  //  __CHARSTRMAP_H__
