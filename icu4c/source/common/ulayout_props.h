// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#यदि_अपरिभाषितम् __ULAYOUT_PROPS_H__
#परिभाषय __ULAYOUT_PROPS_H__

#अन्तर्भाव "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#परिभाषय ULAYOUT_दत्त_NAME "ulayout"
#परिभाषय ULAYOUT_दत्त_TYPE "icu"

// data format "Layo"
#परिभाषय ULAYOUT_FMT_0 0x4c
#परिभाषय ULAYOUT_FMT_1 0x61
#परिभाषय ULAYOUT_FMT_2 0x79
#परिभाषय ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
गणना {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_दीर्घता,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_शिखरम्,
    ULAYOUT_IX_INSC_TRIE_शिखरम्,
    ULAYOUT_IX_VO_TRIE_शिखरम्,
    ULAYOUT_IX_RESERVED_शिखरम्,

    ULAYOUT_IX_TRIES_शिखरम् = 7,

    ULAYOUT_IX_MAX_मानानि = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_गणनम् = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_सरणम् = 24;
constexpr int32_t ULAYOUT_MAX_INSC_सरणम् = 16;
constexpr int32_t ULAYOUT_MAX_VO_सरणम् = 8;

#यदि_अन्तः  // __ULAYOUT_PROPS_H__
