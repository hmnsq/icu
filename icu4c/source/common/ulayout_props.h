// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __ULAYOUT_PROPS_H__
#وصف_ڪريو __ULAYOUT_PROPS_H__

#ضمن "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#وصف_ڪريو ULAYOUT_معلومات_NAME "ulayout"
#وصف_ڪريو ULAYOUT_معلومات_TYPE "icu"

// data format "Layo"
#وصف_ڪريو ULAYOUT_FMT_0 0x4c
#وصف_ڪريو ULAYOUT_FMT_1 0x61
#وصف_ڪريو ULAYOUT_FMT_2 0x79
#وصف_ڪريو ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
تعداد {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_ڊيگهائي,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_چوٽي,
    ULAYOUT_IX_INSC_TRIE_چوٽي,
    ULAYOUT_IX_VO_TRIE_چوٽي,
    ULAYOUT_IX_RESERVED_چوٽي,

    ULAYOUT_IX_TRIES_چوٽي = 7,

    ULAYOUT_IX_MAX_قدرون = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_ڳڻپ = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_سرڪايو = 24;
constexpr int32_t ULAYOUT_MAX_INSC_سرڪايو = 16;
constexpr int32_t ULAYOUT_MAX_VO_سرڪايو = 8;

#جيڪڏهن_پورو  // __ULAYOUT_PROPS_H__
