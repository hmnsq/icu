// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uarrsort.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003aug04
*   created by: Markus W. Scherer
*
*   Internal function for sorting arrays.
*/

#جے_بنا_تعریف __UARRSORT_H__
#تعریف_کرو __UARRSORT_H__

#ضمن "unicode/utypes.h"

U_CDECL_BEGIN
/**
 * Function type for comparing two items as part of sorting an array or similar.
 * Callback function for uprv_sortArray().
 *
 * @param context Application-specific pointer, passed through by uprv_sortArray().
 * @param left    Pointer to the "left" item.
 * @param right   Pointer to the "right" item.
 * @return 32-bit signed integer comparison result:
 *                <0 if left<right
 *               ==0 if left==right
 *                >0 if left>right
 *
 * @internal
 */
تعريفنوع int32_t U_CALLCONV
UComparator(ثابت فراغ *context, ثابت فراغ *بایاں, ثابت فراغ *دایاں);
U_CDECL_سرا

/**
 * Array sorting function.
 * Uses a UComparator for comparing array items to each other, and simple
 * memory copying to move items.
 *
 * @param array      The array to be sorted.
 * @param length     The number of items in the array.
 * @param itemSize   The size in bytes of each array item.
 * @param cmp        UComparator function used to compare two items each.
 * @param context    Application-specific pointer, passed through to the UComparator.
 * @param sortStable If true, a stable sorting algorithm must be used.
 * @param pErrorCode ICU in/out UErrorCode parameter.
 *
 * @internal
 */
U_CAPI فراغ U_EXPORT2
uprv_ترتیب_دیو_Array(فراغ *array, int32_t لمائی, int32_t شے_ناپ,
               UComparator *cmp, ثابت فراغ *context,
               UBool ترتیب_دیو_Stable, UError_کوڈ *p_Error_کوڈ);

/**
 * Convenience UComparator implementation for uint16_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *بایاں, ثابت فراغ *دایاں);

/**
 * Convenience UComparator implementation for int32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *بایاں, ثابت فراغ *دایاں);

/**
 * Convenience UComparator implementation for uint32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *بایاں, ثابت فراغ *دایاں);

/**
 * Much like Java Collections.binarySearch(list, key, comparator).
 *
 * Except: Java documents "If the list contains multiple elements equal to
 * the specified object, there is no guarantee which one will be found."
 *
 * This version here will return the largest index of any equal item,
 * for use in stable sorting.
 *
 * @return the index>=0 where the item was found:
 *         the largest such index, if multiple, for stable sorting;
 *         or the index<0 for inserting the item at ~index in sorted order
 */
U_CAPI int32_t U_EXPORT2
uprv_stable_ثنائی_تلاش(حرف *array, int32_t لمائی, فراغ *شے, int32_t شے_ناپ,
                        UComparator *cmp, ثابت فراغ *context);

#جے_انت
