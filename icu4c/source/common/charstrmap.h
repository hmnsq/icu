// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CHARSTRMAP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CHARSTRMAP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t ਮਾਪ_2, UError_ਕੋਡ &error_ਕੋਡ) {
        map = uhash_open_ਮਾਪ(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             ਮਾਪ_2, &error_ਕੋਡ);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ਅਚਲ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &ਸੰਚਾਲਕ=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ਵਾਪਸ *this;
    }
    CharStringMap &ਸੰਚਾਲਕ=(ਅਚਲ CharStringMap &other) = delete;

    ਅਚਲ ਅੱਖਰ *get(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ) ਅਚਲ { ਵਾਪਸ static_cast<ਅਚਲ ਅੱਖਰ *>(uhash_get(map, ਕੁੰਜੀ)); }
    ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_2, UError_ਕੋਡ &error_ਕੋਡ) {
        uhash_put(map, const_cast<ਅੱਖਰ *>(ਕੁੰਜੀ), const_cast<ਅੱਖਰ *>(ਮੁੱਲ_2), &error_ਕੋਡ);
    }

ਨਿਜੀ:
    UHashtable *map;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  //  __CHARSTRMAP_H__
