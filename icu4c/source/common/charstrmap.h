// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : عمومی UMemory {
عمومی:
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
        بازگشت *this;
    }
    CharStringMap &operator=(ثابت CharStringMap &other) = delete;

    ثابت نویسه *get(ثابت نویسه *کلید) ثابت { بازگشت static_cast<ثابت نویسه *>(uhash_get(map, کلید)); }
    تهی put(ثابت نویسه *کلید, ثابت نویسه *مقدار_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<نویسه *>(کلید), const_cast<نویسه *>(مقدار_2), &errorCode);
    }

خصوصی:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
