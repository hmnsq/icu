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

#ଅନ୍ତର୍ଭାଵ <cstddef>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"

ଗଣନା {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ଗଦା_ITEM_SIZE=200
};

ସ୍ଥିର constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    ଫେରନ୍ତୁ (sizeInBytes + ଆକାର(std::max_align_t) - 1) / ଆକାର(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    (ଶୂନ୍ୟ)context;
    ଫେରନ୍ତୁ (int32_t)*(ଅଚଳ uint16_t *)left - (int32_t)*(ଅଚଳ uint16_t *)ଡାହାଣ;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    (ଶୂନ୍ୟ)context;
    ଫେରନ୍ତୁ *(ଅଚଳ int32_t *)left - *(ଅଚଳ int32_t *)ଡାହାଣ;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    (ଶୂନ୍ୟ)context;
    uint32_t l=*(ଅଚଳ uint32_t *)left, r=*(ଅଚଳ uint32_t *)ଡାହାଣ;

    /* compare directly because (l-r) would overflow the int32_t result */
    ଯଦି(l<r) {
        ଫେରନ୍ତୁ -1;
    } ଅନ୍ୟଥା ଯଦି(l==r) {
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା /* l>r */ {
        ଫେରନ୍ତୁ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ଦ୍ୱିମିକ_ସନ୍ଧାନ(ଅକ୍ଷର *array, int32_t limit, ଶୂନ୍ୟ *item, int32_t itemSize,
                        UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context) {
    int32_t start=0;
    UBool found=ମିଥ୍ୟା;

    /* Binary search until we get down to a tiny sub-array. */
    ଯେତେବେଳେ((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        ଯଦି(diff==0) {
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
            found=ସତ୍ୟ;
            start=i+1;
        } ଅନ୍ୟଥା ଯଦି(diff<0) {
            limit=i;
        } ଅନ୍ୟଥା {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ଯେତେବେଳେ(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        ଯଦି(diff==0) {
            found=ସତ୍ୟ;
        } ଅନ୍ୟଥା ଯଦି(diff<0) {
            ଭାଙ୍ଗ;
        }
        ++start;
    }
    ଫେରନ୍ତୁ found ? (start-1) : ~start;
}

ସ୍ଥିର ଶୂନ୍ୟ
doInsertionSort(ଅକ୍ଷର *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context, ଶୂନ୍ୟ *pv) {
    int32_t j;

    ପାଇଁ(j=1; j<length; ++j) {
        ଅକ୍ଷର *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_ଦ୍ୱିମିକ_ସନ୍ଧାନ(array, j, item, itemSize, cmp, context);
        ଯଦି(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } ଅନ୍ୟଥା {
            ++insertionPoint;  /* one past the last equal item */
        }
        ଯଦି(insertionPoint<j) {
            ଅକ୍ଷର *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ
insertionSort(ଅକ୍ଷର *array, int32_t length, int32_t itemSize,
              UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context, UErrorCode *pErrorCode) {

    icu::Maybe_ଗଦା_Array<std::max_align_t, sizeInMaxAlignTs(ଗଦା_ITEM_SIZE)> v;
    ଯଦି (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
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
ସ୍ଥିର ଶୂନ୍ୟ
subQuickSort(ଅକ୍ଷର *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context,
             ଶୂନ୍ୟ *px, ଶୂନ୍ୟ *pw) {
    int32_t left, ଡାହାଣ;

    /* start and left are inclusive, limit and right are exclusive */
    କର {
        ଯଦି((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            ଭାଙ୍ଗ;
        }

        left=start;
        ଡାହାଣ=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        କର {
            ଯେତେବେଳେ(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            ଯେତେବେଳେ(/* x<array[right-1] */
                  cmp(context, px, array+(ଡାହାଣ-1)*itemSize)<0
            ) {
                --ଡାହାଣ;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ଯଦି(left<ଡାହାଣ) {
                --ଡାହାଣ;

                ଯଦି(left<ଡାହାଣ) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)ଡାହାଣ*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)ଡାହାଣ*itemSize, pw, itemSize);
                }

                ++left;
            }
        } ଯେତେବେଳେ(left<ଡାହାଣ);

        /* sort sub-arrays */
        ଯଦି((ଡାହାଣ-start)<(limit-left)) {
            /* sort [start..right[ */
            ଯଦି(start<(ଡାହାଣ-1)) {
                subQuickSort(array, start, ଡାହାଣ, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } ଅନ୍ୟଥା {
            /* sort [left..limit[ */
            ଯଦି(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=ଡାହାଣ;
        }
    } ଯେତେବେଳେ(start<(limit-1));
}

ସ୍ଥିର ଶୂନ୍ୟ
quickSort(ଅକ୍ଷର *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ଗଦା_Array<std::max_align_t, sizeInMaxAlignTs(ଗଦା_ITEM_SIZE) * 2> xw;
    ଯଦି(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
uprv_sortArray(ଶୂନ୍ୟ *array, int32_t length, int32_t itemSize,
               UComparator *cmp, ଅଚଳ ଶୂନ୍ୟ *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଯଦି(length<=1) {
        ଫେରନ୍ତୁ;
    } ଅନ୍ୟଥା ଯଦି(length<MIN_QSORT || sortStable) {
        insertionSort((ଅକ୍ଷର *)array, length, itemSize, cmp, context, pErrorCode);
    } ଅନ୍ୟଥା {
        quickSort((ଅକ୍ଷର *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
