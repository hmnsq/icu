// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#اگر_تعریف_نشده __CHARSTRMAP_H__
#تعریف_کن __CHARSTRMAP_H__

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
    CharStringMap(int32_t اندازه_2, UError_کد &error_کد) {
        map = uhash_open_اندازه(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             اندازه_2, &error_کد);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ثابت CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &عملگر=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        أرجع *this;
    }
    CharStringMap &عملگر=(ثابت CharStringMap &other) = delete;

    ثابت حرف *get(ثابت حرف *کلید) ثابت { أرجع static_cast<ثابت حرف *>(uhash_get(map, کلید)); }
    فراغ put(ثابت حرف *کلید, ثابت حرف *مقدار_2, UError_کد &error_کد) {
        uhash_put(map, const_cast<حرف *>(کلید), const_cast<حرف *>(مقدار_2), &error_کد);
    }

خاص:
    UHashtable *map;
};

U_NAMESPACE_انتها

#پایان_اگر  //  __CHARSTRMAP_H__
