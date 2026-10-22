// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#جے_بنا_تعریف __CHARSTRMAP_H__
#تعریف_کرو __CHARSTRMAP_H__

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
    CharStringMap(int32_t ناپ_2, UError_کوڈ &error_کوڈ) {
        map = uhash_open_ناپ(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             ناپ_2, &error_کوڈ);
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

    ثابت حرف *get(ثابت حرف *کلید) ثابت { أرجع static_cast<ثابت حرف *>(uhash_get(map, کلید)); }
    فراغ put(ثابت حرف *کلید, ثابت حرف *قدر_2, UError_کوڈ &error_کوڈ) {
        uhash_put(map, const_cast<حرف *>(کلید), const_cast<حرف *>(قدر_2), &error_کوڈ);
    }

خاص:
    UHashtable *map;
};

U_NAMESPACE_سرا

#جے_انت  //  __CHARSTRMAP_H__
