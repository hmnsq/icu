// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ULAYOUT_PROPS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __ULAYOUT_PROPS_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_ದತ್ತಾಂಶ_NAME "ulayout"
#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_ದತ್ತಾಂಶ_TYPE "icu"

// data format "Layo"
#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_FMT_0 0x4c
#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_FMT_1 0x61
#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_FMT_2 0x79
#ವ್ಯಾಖ್ಯಾನಿಸು ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
ಎಣಿಕೆ {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_ಉದ್ದಳತೆ,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_ತುದಿ,
    ULAYOUT_IX_INSC_TRIE_ತುದಿ,
    ULAYOUT_IX_VO_TRIE_ತುದಿ,
    ULAYOUT_IX_RESERVED_ತುದಿ,

    ULAYOUT_IX_TRIES_ತುದಿ = 7,

    ULAYOUT_IX_MAX_ಮೌಲ್ಯಗಳು = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_ಎಣಿಕೆ = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_ಸರಿಸು = 24;
constexpr int32_t ULAYOUT_MAX_INSC_ಸರಿಸು = 16;
constexpr int32_t ULAYOUT_MAX_VO_ಸರಿಸು = 8;

#ಆದರೆ_ಅಂತ್ಯ  // __ULAYOUT_PROPS_H__
