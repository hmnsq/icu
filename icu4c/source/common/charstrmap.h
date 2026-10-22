// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#അന്തര്ഭാവ <utility>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : പൊതു UMemory {
പൊതു:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(സ്ഥിരം CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        തിരികെ *this;
    }
    CharStringMap &operator=(സ്ഥിരം CharStringMap &other) = delete;

    സ്ഥിരം അക്ഷരം *get(സ്ഥിരം അക്ഷരം *താക്കോൽ) സ്ഥിരം { തിരികെ static_cast<സ്ഥിരം അക്ഷരം *>(uhash_get(map, താക്കോൽ)); }
    ശൂന്യം put(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<അക്ഷരം *>(താക്കോൽ), const_cast<അക്ഷരം *>(മൂല്യം_2), &errorCode);
    }

സ്വകാര്യ:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
