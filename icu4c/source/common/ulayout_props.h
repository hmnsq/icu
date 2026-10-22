// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#अगर_अघोषित __ULAYOUT_PROPS_H__
#घोषणा __ULAYOUT_PROPS_H__

#समावेश "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#घोषणा ULAYOUT_तथ्य_NAME "ulayout"
#घोषणा ULAYOUT_तथ्य_TYPE "icu"

// data format "Layo"
#घोषणा ULAYOUT_FMT_0 0x4c
#घोषणा ULAYOUT_FMT_1 0x61
#घोषणा ULAYOUT_FMT_2 0x79
#घोषणा ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
क्रमागत {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_लंबाई,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_शीर्ष,
    ULAYOUT_IX_INSC_TRIE_शीर्ष,
    ULAYOUT_IX_VO_TRIE_शीर्ष,
    ULAYOUT_IX_RESERVED_शीर्ष,

    ULAYOUT_IX_TRIES_शीर्ष = 7,

    ULAYOUT_IX_MAX_मूल्यावली = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_गिनती = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_SHIFT = 24;
constexpr int32_t ULAYOUT_MAX_INSC_SHIFT = 16;
constexpr int32_t ULAYOUT_MAX_VO_SHIFT = 8;

#पूर्ण_अगर  // __ULAYOUT_PROPS_H__
