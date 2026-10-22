// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#که_نه_تعریف_شوی __ULAYOUT_PROPS_H__
#تعریف_کړه __ULAYOUT_PROPS_H__

#ضمن "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#تعریف_کړه ULAYOUT_معلومات_NAME "ulayout"
#تعریف_کړه ULAYOUT_معلومات_TYPE "icu"

// data format "Layo"
#تعریف_کړه ULAYOUT_FMT_0 0x4c
#تعریف_کړه ULAYOUT_FMT_1 0x61
#تعریف_کړه ULAYOUT_FMT_2 0x79
#تعریف_کړه ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
تعداد {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_اوږدتیا,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_سر,
    ULAYOUT_IX_INSC_TRIE_سر,
    ULAYOUT_IX_VO_TRIE_سر,
    ULAYOUT_IX_RESERVED_سر,

    ULAYOUT_IX_TRIES_سر = 7,

    ULAYOUT_IX_MAX_ارزښتونه = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_شمېر = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_ښويدنه = 24;
constexpr int32_t ULAYOUT_MAX_INSC_ښويدنه = 16;
constexpr int32_t ULAYOUT_MAX_VO_ښويدنه = 8;

#د_که_پای  // __ULAYOUT_PROPS_H__
