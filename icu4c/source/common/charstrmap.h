// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ಧ್ರುವ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ಪ್ರತ್ಯಾಗಮ *this;
    }
    CharStringMap &operator=(ಧ್ರುವ CharStringMap &other) = delete;

    ಧ್ರುವ ವರ್ಣ *get(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ ವರ್ಣ *>(uhash_get(map, ಕೀಲಿ)); }
    ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<ವರ್ಣ *>(ಕೀಲಿ), const_cast<ವರ್ಣ *>(ಮೌಲ್ಯ_2), &errorCode);
    }

ಗುಪ್ತ:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
