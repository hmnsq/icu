// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ulayout_props.h
// created: 2019feb12 Markus W. Scherer

#నిర్వచితం_కాకపోతే __ULAYOUT_PROPS_H__
#నిర్వచించు __ULAYOUT_PROPS_H__

#అన్తర్భావ "unicode/utypes.h"

// file definitions ------------------------------------------------------------

#నిర్వచించు ULAYOUT_దత్తాంశం_NAME "ulayout"
#నిర్వచించు ULAYOUT_దత్తాంశం_TYPE "icu"

// data format "Layo"
#నిర్వచించు ULAYOUT_FMT_0 0x4c
#నిర్వచించు ULAYOUT_FMT_1 0x61
#నిర్వచించు ULAYOUT_FMT_2 0x79
#నిర్వచించు ULAYOUT_FMT_3 0x6f

// indexes into indexes[]
గణన {
    // Element 0 stores the length of the indexes[] array.
    ULAYOUT_IX_INDEXES_నిడివి,
    // Elements 1..7 store the tops of consecutive code point tries.
    // No trie is stored if the difference between two of these is less than 16.
    ULAYOUT_IX_INPC_TRIE_శిఖరం,
    ULAYOUT_IX_INSC_TRIE_శిఖరం,
    ULAYOUT_IX_VO_TRIE_శిఖరం,
    ULAYOUT_IX_RESERVED_శిఖరం,

    ULAYOUT_IX_TRIES_శిఖరం = 7,

    ULAYOUT_IX_MAX_విలువలు = 9,

    // Length of indexes[]. Multiple of 4 to 16-align the tries.
    ULAYOUT_IX_లెక్క = 12
};

constexpr int32_t ULAYOUT_MAX_INPC_జరుపు = 24;
constexpr int32_t ULAYOUT_MAX_INSC_జరుపు = 16;
constexpr int32_t ULAYOUT_MAX_VO_జరుపు = 8;

#అయితే_ముగింపు  // __ULAYOUT_PROPS_H__
