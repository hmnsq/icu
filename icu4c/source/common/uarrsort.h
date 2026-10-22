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

#నిర్వచితం_కాకపోతే __UARRSORT_H__
#నిర్వచించు __UARRSORT_H__

#అన్తర్భావ "unicode/utypes.h"

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
రకనిర్వచనం int32_t U_CALLCONV
UComparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ_2, స్థిరరాశి శూన్య *కుడి_2);
U_CDECL_చివర

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
U_CAPI శూన్య U_EXPORT2
uprv_క్రమీకరించు_Array(శూన్య *array, int32_t నిడివి, int32_t అంశం_కొలత,
               UComparator *cmp, స్థిరరాశి శూన్య *context,
               UBool క్రమీకరించు_Stable, UError_సంకేతం *p_Error_సంకేతం);

/**
 * Convenience UComparator implementation for uint16_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ_2, స్థిరరాశి శూన్య *కుడి_2);

/**
 * Convenience UComparator implementation for int32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ_2, స్థిరరాశి శూన్య *కుడి_2);

/**
 * Convenience UComparator implementation for uint32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ_2, స్థిరరాశి శూన్య *కుడి_2);

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
uprv_stable_ద్విమాన_శోధన(అక్షరం *array, int32_t నిడివి, శూన్య *అంశం, int32_t అంశం_కొలత,
                        UComparator *cmp, స్థిరరాశి శూన్య *context);

#అయితే_ముగింపు
