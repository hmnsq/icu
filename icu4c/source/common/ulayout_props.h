// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __ULAYOUT_PROPS_H__
#تعریف_کرو __ULAYOUT_PROPS_H__

#شامل "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#تعریف_کرو ULAYOUT_ڈیٹا_NAME "ulayout"
#تعریف_کرو ULAYOUT_ڈیٹا_TYPE "icu"

// data format "Layo"
#تعریف_کرو ULAYOUT_FMT_0 0x4c
#تعریف_کرو ULAYOUT_FMT_1 0x61
#تعریف_کرو ULAYOUT_FMT_2 0x79
#تعریف_کرو ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
شمار {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_لمبائی,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_چوٹی,
    ULAYOUT_IX_INSC_TRIE_چوٹی,
    ULAYOUT_IX_VO_TRIE_چوٹی,
    ULAYOUT_IX_RESERVED_چوٹی,

    ULAYOUT_IX_TRIES_چوٹی = 7,

    ULAYOUT_IX_MAX_اقدار = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_گنتی = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_سرکاؤ = 24;
constexpr int32_t ULAYOUT_MAX_INSC_سرکاؤ = 16;
constexpr int32_t ULAYOUT_MAX_VO_سرکاؤ = 8;

#اگر_ختم  // __ULAYOUT_PROPS_H__
