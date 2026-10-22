// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#اگر_غیر_تعریف_شدہ __CHARSTRMAP_H__
#تعریف_کرو __CHARSTRMAP_H__

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : عوامی UMemory {
عوامی:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t سائز_2, UError_رمز &error_رمز) {
        map = uhash_open_سائز(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             سائز_2, &error_رمز);
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
        واپس *this;
    }
    CharStringMap &عامل=(ثابت CharStringMap &other) = delete;

    ثابت حرف *get(ثابت حرف *کلید) ثابت { واپس static_cast<ثابت حرف *>(uhash_get(map, کلید)); }
    خالی put(ثابت حرف *کلید, ثابت حرف *قدر_2, UError_رمز &error_رمز) {
        uhash_put(map, const_cast<حرف *>(کلید), const_cast<حرف *>(قدر_2), &error_رمز);
    }

نجی:
    UHashtable *map;
};

U_NAMESPACE_انتہا

#اگر_ختم  //  __CHARSTRMAP_H__
