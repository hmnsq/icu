// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#ଅନ୍ତର୍ଭାଵ <utility>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ଅଚଳ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ଫେରନ୍ତୁ *this;
    }
    CharStringMap &operator=(ଅଚଳ CharStringMap &other) = delete;

    ଅଚଳ ଅକ୍ଷର *get(ଅଚଳ ଅକ୍ଷର *ଚାବି) ଅଚଳ { ଫେରନ୍ତୁ static_cast<ଅଚଳ ଅକ୍ଷର *>(uhash_get(map, ଚାବି)); }
    ଶୂନ୍ୟ put(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<ଅକ୍ଷର *>(ଚାବି), const_cast<ଅକ୍ଷର *>(ମୂଲ୍ୟ_2), &errorCode);
    }

ବ୍ୟକ୍ତିଗତ:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
