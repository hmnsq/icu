// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __CHARSTRMAP_H__
#define __CHARSTRMAP_H__

#அந்தர்பாவ <utility>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : பொது UMemory {
பொது:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t size, UErrorCode &errorCode) {
        map = uhash_openSize(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             size, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(மாறிலி CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &operator=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        திரும்பு *this;
    }
    CharStringMap &operator=(மாறிலி CharStringMap &other) = delete;

    மாறிலி எழுத்து *get(மாறிலி எழுத்து *சாவி) மாறிலி { திரும்பு static_cast<மாறிலி எழுத்து *>(uhash_get(map, சாவி)); }
    வெறுமை put(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<எழுத்து *>(சாவி), const_cast<எழுத்து *>(மதிப்பு_2), &errorCode);
    }

தனிப்பட்ட:
    UHashtable *map;
};

U_NAMESPACE_END

#endif  //  __CHARSTRMAP_H__
