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

#ಅನ್ತರ್ಭಾವ <cstddef>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"

ಗಣನಾ {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ರಾಶಿ_ITEM_SIZE=200
};

ಸ್ಥಿರ constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    ಪ್ರತ್ಯಾಗಮ (sizeInBytes + ಪರಿಮಾಣ(std::max_align_t) - 1) / ಪರಿಮಾಣ(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    ಪ್ರತ್ಯಾಗಮ (int32_t)*(ಧ್ರುವ uint16_t *)left - (int32_t)*(ಧ್ರುವ uint16_t *)ಬಲ;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    ಪ್ರತ್ಯಾಗಮ *(ಧ್ರುವ int32_t *)left - *(ಧ್ರುವ int32_t *)ಬಲ;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    uint32_t l=*(ಧ್ರುವ uint32_t *)left, r=*(ಧ್ರುವ uint32_t *)ಬಲ;

    /* compare directly because (l-r) would overflow the int32_t result */
    ಯದಿ(l<r) {
        ಪ್ರತ್ಯಾಗಮ -1;
    } ಅನ್ಯಥಾ ಯದಿ(l==r) {
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ /* l>r */ {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ದ್ವಿಮಾನ_ಹುಡುಕು(ವರ್ಣ *array, int32_t limit, ಶೂನ್ಯ *item, int32_t itemSize,
                        UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context) {
    int32_t start=0;
    UBool found=ಅಸತ್ಯ;

    /* Binary search until we get down to a tiny sub-array. */
    ಯಾವತ್((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        ಯದಿ(diff==0) {
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
            found=ಸತ್ಯ;
            start=i+1;
        } ಅನ್ಯಥಾ ಯದಿ(diff<0) {
            limit=i;
        } ಅನ್ಯಥಾ {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ಯಾವತ್(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        ಯದಿ(diff==0) {
            found=ಸತ್ಯ;
        } ಅನ್ಯಥಾ ಯದಿ(diff<0) {
            ಭಙ್ಗ;
        }
        ++start;
    }
    ಪ್ರತ್ಯಾಗಮ found ? (start-1) : ~start;
}

ಸ್ಥಿರ ಶೂನ್ಯ
doInsertionSort(ವರ್ಣ *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context, ಶೂನ್ಯ *pv) {
    int32_t j;

    ಪ್ರತಿ(j=1; j<length; ++j) {
        ವರ್ಣ *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_ದ್ವಿಮಾನ_ಹುಡುಕು(array, j, item, itemSize, cmp, context);
        ಯದಿ(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } ಅನ್ಯಥಾ {
            ++insertionPoint;  /* one past the last equal item */
        }
        ಯದಿ(insertionPoint<j) {
            ವರ್ಣ *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ
insertionSort(ವರ್ಣ *array, int32_t length, int32_t itemSize,
              UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context, UErrorCode *pErrorCode) {

    icu::Maybe_ರಾಶಿ_Array<std::max_align_t, sizeInMaxAlignTs(ರಾಶಿ_ITEM_SIZE)> v;
    ಯದಿ (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
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
ಸ್ಥಿರ ಶೂನ್ಯ
subQuickSort(ವರ್ಣ *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context,
             ಶೂನ್ಯ *px, ಶೂನ್ಯ *pw) {
    int32_t left, ಬಲ;

    /* start and left are inclusive, limit and right are exclusive */
    ಕುರು {
        ಯದಿ((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            ಭಙ್ಗ;
        }

        left=start;
        ಬಲ=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        ಕುರು {
            ಯಾವತ್(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            ಯಾವತ್(/* x<array[right-1] */
                  cmp(context, px, array+(ಬಲ-1)*itemSize)<0
            ) {
                --ಬಲ;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ಯದಿ(left<ಬಲ) {
                --ಬಲ;

                ಯದಿ(left<ಬಲ) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)ಬಲ*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)ಬಲ*itemSize, pw, itemSize);
                }

                ++left;
            }
        } ಯಾವತ್(left<ಬಲ);

        /* sort sub-arrays */
        ಯದಿ((ಬಲ-start)<(limit-left)) {
            /* sort [start..right[ */
            ಯದಿ(start<(ಬಲ-1)) {
                subQuickSort(array, start, ಬಲ, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } ಅನ್ಯಥಾ {
            /* sort [left..limit[ */
            ಯದಿ(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=ಬಲ;
        }
    } ಯಾವತ್(start<(limit-1));
}

ಸ್ಥಿರ ಶೂನ್ಯ
quickSort(ವರ್ಣ *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ರಾಶಿ_Array<std::max_align_t, sizeInMaxAlignTs(ರಾಶಿ_ITEM_SIZE) * 2> xw;
    ಯದಿ(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_sortArray(ಶೂನ್ಯ *array, int32_t length, int32_t itemSize,
               UComparator *cmp, ಧ್ರುವ ಶೂನ್ಯ *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(length<=1) {
        ಪ್ರತ್ಯಾಗಮ;
    } ಅನ್ಯಥಾ ಯದಿ(length<MIN_QSORT || sortStable) {
        insertionSort((ವರ್ಣ *)array, length, itemSize, cmp, context, pErrorCode);
    } ಅನ್ಯಥಾ {
        quickSort((ವರ್ಣ *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
