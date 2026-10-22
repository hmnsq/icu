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

#اگر_تعریف_نشده __UPROPSVEC_H__
#تعریف_کن __UPROPSVEC_H__

#ضمن "unicode/utypes.h"
#ضمن "utrie.h"
#ضمن "utrie2.h"

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
بنية UPropsVectors;
تعريفنوع بنية UPropsVectors UPropsVectors;

/*
 * Special pseudo code points for storing the initialValue and the errorValue,
 * which are used to initialize a UTrie2 or similar.
 */
#تعریف_کن UPVEC_اولی_SPECIAL_CP 0x110000
#تعریف_کن UPVEC_INITIAL_مقدار_CP 0x110000
#تعریف_کن UPVEC_ERROR_مقدار_CP 0x110001
#تعریف_کن UPVEC_MAX_CP 0x110001

/*
 * Special pseudo code point used in upvec_compact() signalling the end of
 * delivering special values and the beginning of delivering real ones.
 * Stable value, unlike UPVEC_MAX_CP which might grow over time.
 */
#تعریف_کن UPVEC_شروع_REAL_مقادیر_CP 0x200000

/*
 * Open a UPropsVectors object.
 * @param columns Number of value integers (uint32_t) per row.
 */
U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_کد *p_Error_کد);

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv);

/*
 * In rows for code points [start..end], select the column,
 * reset the mask bits and set the value bits (ANDed with the mask).
 *
 * Will set U_NO_WRITE_PERMISSION if called after upvec_compact().
 */
U_CAPI فراغ U_EXPORT2
upvec_مجموعه_مقدار(UPropsVectors *pv,
               UChar32 شروع, UChar32 انتها,
               int32_t ستونی,
               uint32_t مقدار_2, uint32_t ماسک,
               UError_کد *p_Error_کد);

/*
 * Logically const but must not be used on the same pv concurrently!
 * Always returns 0 if called after upvec_compact().
 */
U_CAPI uint32_t U_EXPORT2
upvec_get_مقدار(ثابت UPropsVectors *pv, UChar32 c, int32_t ستونی);

/*
 * pRangeStart and pRangeEnd can be NULL.
 * @return NULL if rowIndex out of range and for illegal arguments,
 *         or if called after upvec_compact()
 */
U_CAPI uint32_t * U_EXPORT2
upvec_get_ردیف(ثابت UPropsVectors *pv, int32_t ردیف_شاخص,
             UChar32 *p_Range_شروع, UChar32 *p_Range_انتها);

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
تعريفنوع فراغ U_CALLCONV
UPVecCompactHandler(فراغ *context,
                    UChar32 شروع, UChar32 انتها,
                    int32_t ردیف_شاخص, uint32_t *ردیف, int32_t columns,
                    UError_کد *p_Error_کد);

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UError_کد *p_Error_کد);

/*
 * Get the vectors array after calling upvec_compact().
 * The caller must not modify nor release the returned array.
 * Returns NULL if called before upvec_compact().
 */
U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_ردیفها, int32_t *pColumns);

/*
 * Get a clone of the vectors array after calling upvec_compact().
 * The caller owns the returned array and must uprv_free() it.
 * Returns NULL if called before upvec_compact().
 */
U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_ردیفها, int32_t *pColumns, UError_کد *p_Error_کد);

/*
 * Call upvec_compact(), create a 16-bit UTrie2 with indexes into the compacted
 * vectors array, and freeze the trie.
 */
U_CAPI UTrie2 * U_EXPORT2
upvec_compact_مقصد_UTrie2_With_ردیف_Indexes(UPropsVectors *pv, UError_کد *p_Error_کد);

بنية UPVec_مقصد_UTrie2_Context {
    UTrie2 *trie;
    int32_t initial_مقدار;
    int32_t error_مقدار;
    int32_t max_مقدار;
};
تعريفنوع بنية UPVec_مقصد_UTrie2_Context UPVec_مقصد_UTrie2_Context;

/* context=UPVecToUTrie2Context, creates the trie and stores the rowIndex values */
U_CAPI فراغ U_CALLCONV
upvec_compact_مقصد_UTrie2_Handler(فراغ *context,
                             UChar32 شروع, UChar32 انتها,
                             int32_t ردیف_شاخص, uint32_t *ردیف, int32_t columns,
                             UError_کد *p_Error_کد);

U_CDECL_انتها

#پایان_اگر
