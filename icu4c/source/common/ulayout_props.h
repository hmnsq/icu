// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ULAYOUT_PROPS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __ULAYOUT_PROPS_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_ਅੰਕੜੇ_NAME "ulayout"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_ਅੰਕੜੇ_TYPE "icu"

// data format "Layo"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_FMT_0 0x4c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_FMT_1 0x61
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_FMT_2 0x79
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
ਗਿਣਤੀ {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_ਲੰਮਾਈ,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_ਸਿਖਰ,
    ULAYOUT_IX_INSC_TRIE_ਸਿਖਰ,
    ULAYOUT_IX_VO_TRIE_ਸਿਖਰ,
    ULAYOUT_IX_RESERVED_ਸਿਖਰ,

    ULAYOUT_IX_TRIES_ਸਿਖਰ = 7,

    ULAYOUT_IX_MAX_ਮੁੱਲਾਂ = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_ਗਿਣਤੀ = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_ਖਿਸਕਾਓ = 24;
constexpr int32_t ULAYOUT_MAX_INSC_ਖਿਸਕਾਓ = 16;
constexpr int32_t ULAYOUT_MAX_VO_ਖਿਸਕਾਓ = 8;

#ਜੇ_ਅੰਤ  // __ULAYOUT_PROPS_H__
