// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#ضمن <utility>
#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : عام UMemory {
عام:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ثابت CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        أرجع *this;
    }
    CharStringMap &operator=(ثابت CharStringMap &other) = delete;

    ثابت حرف *get(ثابت حرف *ئاچقۇچ) ثابت { أرجع static_cast<ثابت حرف *>(uhash_get(map, ئاچقۇچ)); }
    فراغ put(ثابت حرف *ئاچقۇچ, ثابت حرف *قىممەت_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<حرف *>(ئاچقۇچ), const_cast<حرف *>(قىممەت_2), &errorCode);
    }

خاص:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
