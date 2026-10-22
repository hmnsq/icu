// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#અન્તર્ભાવ <utility>
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : જાહેર UMemory {
જાહેર:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(અચલ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        પાછા *this;
    }
    CharStringMap &operator=(અચલ CharStringMap &other) = delete;

    અચલ અક્ષર *get(અચલ અક્ષર *ચાવી) અચલ { પાછા static_cast<અચલ અક્ષર *>(uhash_get(map, ચાવી)); }
    શૂન્ય put(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<અક્ષર *>(ચાવી), const_cast<અક્ષર *>(કિંમત_2), &errorCode);
    }

ખાનગી:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
