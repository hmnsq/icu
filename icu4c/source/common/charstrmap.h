// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#include <utility>
#include "unicode/utypes.h"
#include "unicode/uobject.h"
#include "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : public UMemory {
public:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(const CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ສົ່ງຄືນ *this;
    }
    CharStringMap &operator=(const CharStringMap &other) = delete;

    const var_na *get(const var_na *key) const { ສົ່ງຄືນ static_cast<const var_na *>(uhash_get(map, key)); }
    void put(const var_na *key, const var_na *value, UErrorCode &errorCode) {
        uhash_put(map, const_cast<var_na *>(key), const_cast<var_na *>(value), &errorCode);
    }

private:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
