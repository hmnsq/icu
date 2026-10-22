// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CHARSTRMAP_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CHARSTRMAP_H__

#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        map = uhash_open_ಗಾತ್ರ(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             ಗಾತ್ರ_3, &error_ಸಂಕೇತ);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(ಸ್ಥಿರಾಂಕ CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &ಕ್ರಿಯಾಕಾರಕ=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }
    CharStringMap &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ CharStringMap &other) = delete;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(uhash_get(map, ಕೀಲಿ)); }
    ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        uhash_put(map, const_cast<ಅಕ್ಷರ *>(ಕೀಲಿ), const_cast<ಅಕ್ಷರ *>(ಮೌಲ್ಯ_2), &error_ಸಂಕೇತ);
    }

ಖಾಸಗಿ:
    UHashtable *map;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  //  __CHARSTRMAP_H__
