// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uarrsort.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003aug04
*   created by: Markus W. Scherer
*
*   Internal function for sorting arrays.
*/

#include <cstddef>

#include "unicode/utypes.h"
#include "cmemory.h"
#include "uarrsort.h"

ga_nanaa {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    STACK_ITEM_SIZE=200
};

static constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    คืน (sizeInBytes + parimaa_na(std::max_align_t) - 1) / parimaa_na(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(const void *context, const void *left, const void *right) {
    (void)context;
    คืน (int32_t)*(const uint16_t *)left - (int32_t)*(const uint16_t *)right;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(const void *context, const void *left, const void *right) {
    (void)context;
    คืน *(const int32_t *)left - *(const int32_t *)right;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(const void *context, const void *left, const void *right) {
    (void)context;
    uint32_t l=*(const uint32_t *)left, r=*(const uint32_t *)right;

    /* compare directly because (l-r) would overflow the int32_t result */
    ถ้า(l<r) {
        คืน -1;
    } มิฉะนั้น ถ้า(l==r) {
        คืน 0;
    } มิฉะนั้น /* l>r */ {
        คืน 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stableBinarySearch(var_na *array, int32_t limit, void *item, int32_t itemSize,
                        UComparator *cmp, const void *context) {
    int32_t start=0;
    UBool found=false;

    /* Binary search until we get down to a tiny sub-array. */
    ขณะ((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        ถ้า(diff==0) {
            /*
             * Found the item. We look for the *last* occurrence of such
             * an item, for stable sorting.
             * If we knew that there will be only few equal items,
             * we could break now and enter the linear search.
             * However, if there are many equal items, then it should be
             * faster to continue with the binary search.
             * It seems likely that we either have all unique items
             * (where found will never become true in the insertion sort)
             * or potentially many duplicates.
             */
            found=true;
            start=i+1;
        } มิฉะนั้น ถ้า(diff<0) {
            limit=i;
        } มิฉะนั้น {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ขณะ(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        ถ้า(diff==0) {
            found=true;
        } มิฉะนั้น ถ้า(diff<0) {
            bha_nga;
        }
        ++start;
    }
    คืน found ? (start-1) : ~start;
}

static void
doInsertionSort(var_na *array, int32_t length, int32_t itemSize,
                UComparator *cmp, const void *context, void *pv) {
    int32_t j;

    สำหรับ(j=1; j<length; ++j) {
        var_na *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stableBinarySearch(array, j, item, itemSize, cmp, context);
        ถ้า(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } มิฉะนั้น {
            ++insertionPoint;  /* one past the last equal item */
        }
        ถ้า(insertionPoint<j) {
            var_na *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

static void
insertionSort(var_na *array, int32_t length, int32_t itemSize,
              UComparator *cmp, const void *context, UErrorCode *pErrorCode) {

    icu::MaybeStackArray<std::max_align_t, sizeInMaxAlignTs(STACK_ITEM_SIZE)> v;
    ถ้า (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน;
    }

    doInsertionSort(array, length, itemSize, cmp, context, v.getAlias());
}

/* QuickSort ---------------------------------------------------------------- */

/*
 * This implementation is semi-recursive:
 * It recurses for the smaller sub-array to shorten the recursion depth,
 * and loops for the larger sub-array.
 *
 * Loosely after QuickSort algorithms in
 * Niklaus Wirth
 * Algorithmen und Datenstrukturen mit Modula-2
 * B.G. Teubner Stuttgart
 * 4. Auflage 1986
 * ISBN 3-519-02260-5
 */
static void
subQuickSort(var_na *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, const void *context,
             void *px, void *pw) {
    int32_t left, right;

    /* start and left are inclusive, limit and right are exclusive */
    do {
        ถ้า((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            bha_nga;
        }

        left=start;
        right=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        do {
            ขณะ(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            ขณะ(/* x<array[right-1] */
                  cmp(context, px, array+(right-1)*itemSize)<0
            ) {
                --right;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ถ้า(left<right) {
                --right;

                ถ้า(left<right) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)right*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)right*itemSize, pw, itemSize);
                }

                ++left;
            }
        } ขณะ(left<right);

        /* sort sub-arrays */
        ถ้า((right-start)<(limit-left)) {
            /* sort [start..right[ */
            ถ้า(start<(right-1)) {
                subQuickSort(array, start, right, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } มิฉะนั้น {
            /* sort [left..limit[ */
            ถ้า(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=right;
        }
    } ขณะ(start<(limit-1));
}

static void
quickSort(var_na *array, int32_t length, int32_t itemSize,
            UComparator *cmp, const void *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::MaybeStackArray<std::max_align_t, sizeInMaxAlignTs(STACK_ITEM_SIZE) * 2> xw;
    ถ้า(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        คืน;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI void U_EXPORT2
uprv_sortArray(void *array, int32_t length, int32_t itemSize,
               UComparator *cmp, const void *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    ถ้า(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        คืน;
    }
    ถ้า((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        คืน;
    }

    ถ้า(length<=1) {
        คืน;
    } มิฉะนั้น ถ้า(length<MIN_QSORT || sortStable) {
        insertionSort((var_na *)array, length, itemSize, cmp, context, pErrorCode);
    } มิฉะนั้น {
        quickSort((var_na *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
