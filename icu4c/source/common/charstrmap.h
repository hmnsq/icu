// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#که_نه_تعریف_شوی __CHARSTRMAP_H__
#تعریف_کړه __CHARSTRMAP_H__

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
    CharStringMap(int32_t اندازه_2, UError_کوډ &error_کوډ) {
        map = uhash_open_اندازه(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             اندازه_2, &error_کوډ);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ثابت CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &چلوونکی=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        أرجع *this;
    }
    CharStringMap &چلوونکی=(ثابت CharStringMap &other) = delete;

    ثابت حرف *get(ثابت حرف *کلی) ثابت { أرجع static_cast<ثابت حرف *>(uhash_get(map, کلی)); }
    فراغ put(ثابت حرف *کلی, ثابت حرف *ارزښت_2, UError_کوډ &error_کوډ) {
        uhash_put(map, const_cast<حرف *>(کلی), const_cast<حرف *>(ارزښت_2), &error_کوډ);
    }

خاص:
    UHashtable *map;
};

U_NAMESPACE_پای_ټکی

#د_که_پای  //  __CHARSTRMAP_H__
