// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  propsvec.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb22
*   created by: Markus W. Scherer
*
*   Store bits (Unicode character properties) in bit set vectors.
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UPROPSVEC_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UPROPSVEC_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"

U_CDECL_BEGIN

/**
 * Unicode Properties Vectors associated with code point ranges.
 *
 * Rows of uint32_t integers in a contiguous array store
 * the range limits and the properties vectors.
 *
 * Logically, each row has a certain number of uint32_t values,
 * which is set via the upvec_open() "columns" parameter.
 *
 * Internally, two additional columns are stored.
 * In each internal row,
 * row[0] contains the start code point and
 * row[1] contains the limit code point,
 * which is the start of the next range.
 *
 * Initially, there is only one "normal" row for
 * range [0..0x110000[ with values 0.
 * There are additional rows for special purposes, see UPVEC_FIRST_SPECIAL_CP.
 *
 * It would be possible to store only one range boundary per row,
 * but self-contained rows allow to later sort them by contents.
 */
ರಚನೆ UPropsVectors;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UPropsVectors UPropsVectors;

/*
 * Special pseudo code points for storing the initialValue and the errorValue,
 * which are used to initialize a UTrie2 or similar.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_ಮೊದಲ_SPECIAL_CP 0x110000
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_INITIAL_ಮೌಲ್ಯ_CP 0x110000
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_ERROR_ಮೌಲ್ಯ_CP 0x110001
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_MAX_CP 0x110001

/*
 * Special pseudo code point used in upvec_compact() signalling the end of
 * delivering special values and the beginning of delivering real ones.
 * Stable value, unlike UPVEC_MAX_CP which might grow over time.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_ಆರಂಭ_REAL_ಮೌಲ್ಯಗಳು_CP 0x200000

/*
 * Open a UPropsVectors object.
 * @param columns Number of value integers (uint32_t) per row.
 */
U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_close(UPropsVectors *pv);

/*
 * In rows for code points [start..end], select the column,
 * reset the mask bits and set the value bits (ANDed with the mask).
 *
 * Will set U_NO_WRITE_PERMISSION if called after upvec_compact().
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_ಗಣ_ಮೌಲ್ಯ(UPropsVectors *pv,
               UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
               int32_t ಸ್ತಂಭ,
               uint32_t ಮೌಲ್ಯ_2, uint32_t ಮುಖವಾಡ,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/*
 * Logically const but must not be used on the same pv concurrently!
 * Always returns 0 if called after upvec_compact().
 */
U_CAPI uint32_t U_EXPORT2
upvec_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, UChar32 c, int32_t ಸ್ತಂಭ);

/*
 * pRangeStart and pRangeEnd can be NULL.
 * @return NULL if rowIndex out of range and for illegal arguments,
 *         or if called after upvec_compact()
 */
U_CAPI uint32_t * U_EXPORT2
upvec_get_ಅಡ್ಡಸಾಲು(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, int32_t ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ,
             UChar32 *p_Range_ಆರಂಭ, UChar32 *p_Range_ಕೊನೆ);

/*
 * Compact the vectors:
 * - modify the memory
 * - keep only unique vectors
 * - store them contiguously from the beginning of the memory
 * - for each (non-unique) row, call the handler function
 *
 * The handler's rowIndex is the index of the row in the compacted
 * memory block.
 * (Therefore, it starts at 0 increases in increments of the columns value.)
 *
 * In a first phase, only special values are delivered (each exactly once),
 * with start==end both equalling a special pseudo code point.
 * Then the handler is called once more with start==end==UPVEC_START_REAL_VALUES_CP
 * where rowIndex is the length of the compacted array,
 * and the row is arbitrary (but not NULL).
 * Then, in the second phase, the handler is called for each row of real values.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
UPVecCompactHandler(ಶೂನ್ಯ *context,
                    UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                    int32_t ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ, uint32_t *ಅಡ್ಡಸಾಲು, int32_t columns,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ಶೂನ್ಯ *context, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/*
 * Get the vectors array after calling upvec_compact().
 * The caller must not modify nor release the returned array.
 * Returns NULL if called before upvec_compact().
 */
U_CAPI ಸ್ಥಿರಾಂಕ uint32_t * U_EXPORT2
upvec_getArray(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, int32_t *p_ಅಡ್ಡಸಾಲುಗಳು, int32_t *pColumns);

/*
 * Get a clone of the vectors array after calling upvec_compact().
 * The caller owns the returned array and must uprv_free() it.
 * Returns NULL if called before upvec_compact().
 */
U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ಸ್ಥಿರಾಂಕ UPropsVectors *pv,
                 int32_t *p_ಅಡ್ಡಸಾಲುಗಳು, int32_t *pColumns, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/*
 * Call upvec_compact(), create a 16-bit UTrie2 with indexes into the compacted
 * vectors array, and freeze the trie.
 */
U_CAPI UTrie2 * U_EXPORT2
upvec_compact_ಗಮ್ಯ_UTrie2_With_ಅಡ್ಡಸಾಲು_Indexes(UPropsVectors *pv, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ರಚನೆ UPVec_ಗಮ್ಯ_UTrie2_Context {
    UTrie2 *trie;
    int32_t initial_ಮೌಲ್ಯ;
    int32_t error_ಮೌಲ್ಯ;
    int32_t max_ಮೌಲ್ಯ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UPVec_ಗಮ್ಯ_UTrie2_Context UPVec_ಗಮ್ಯ_UTrie2_Context;

/* context=UPVecToUTrie2Context, creates the trie and stores the rowIndex values */
U_CAPI ಶೂನ್ಯ U_CALLCONV
upvec_compact_ಗಮ್ಯ_UTrie2_Handler(ಶೂನ್ಯ *context,
                             UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                             int32_t ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ, uint32_t *ಅಡ್ಡಸಾಲು, int32_t columns,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CDECL_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
