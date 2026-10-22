// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#جيڪڏهن_وصف_نه_ٿيل __CHARSTRMAP_H__
#وصف_ڪريو __CHARSTRMAP_H__

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
    CharStringMap(int32_t سائيز_2, UError_ڪوڊ &error_ڪوڊ) {
        map = uhash_open_سائيز(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             سائيز_2, &error_ڪوڊ);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ثابت CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &عامل=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        أرجع *this;
    }
    CharStringMap &عامل=(ثابت CharStringMap &other) = delete;

    ثابت حرف *get(ثابت حرف *ڪنجي) ثابت { أرجع static_cast<ثابت حرف *>(uhash_get(map, ڪنجي)); }
    فراغ put(ثابت حرف *ڪنجي, ثابت حرف *قدر_2, UError_ڪوڊ &error_ڪوڊ) {
        uhash_put(map, const_cast<حرف *>(ڪنجي), const_cast<حرف *>(قدر_2), &error_ڪوڊ);
    }

خاص:
    UHashtable *map;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  //  __CHARSTRMAP_H__
