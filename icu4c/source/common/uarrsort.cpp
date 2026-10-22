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

#ضمن <cstddef>

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"

تعداد {
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

ساكن constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    أرجع (sizeInBytes + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    (فراغ)context;
    أرجع (int32_t)*(ثابت uint16_t *)left - (int32_t)*(ثابت uint16_t *)right;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    (فراغ)context;
    أرجع *(ثابت int32_t *)left - *(ثابت int32_t *)right;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)left, r=*(ثابت uint32_t *)right;

    /* compare directly because (l-r) would overflow the int32_t result */
    𐡄𐡍(l<r) {
        أرجع -1;
    } وإلا 𐡄𐡍(l==r) {
        أرجع 0;
    } وإلا /* l>r */ {
        أرجع 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stableBinarySearch(حرف *array, int32_t limit, فراغ *item, int32_t itemSize,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t start=0;
    UBool found=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    طالما((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        𐡄𐡍(diff==0) {
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
            found=𐡒𐡔𐡈;
            start=i+1;
        } وإلا 𐡄𐡍(diff<0) {
            limit=i;
        } وإلا {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    طالما(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        𐡄𐡍(diff==0) {
            found=𐡒𐡔𐡈;
        } وإلا 𐡄𐡍(diff<0) {
            اكسر;
        }
        ++start;
    }
    أرجع found ? (start-1) : ~start;
}

ساكن فراغ
doInsertionSort(حرف *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stableBinarySearch(array, j, item, itemSize, cmp, context);
        𐡄𐡍(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } وإلا {
            ++insertionPoint;  /* one past the last equal item */
        }
        𐡄𐡍(insertionPoint<j) {
            حرف *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

ساكن فراغ
insertionSort(حرف *array, int32_t length, int32_t itemSize,
              UComparator *cmp, ثابت فراغ *context, UErrorCode *pErrorCode) {

    icu::MaybeStackArray<std::max_align_t, sizeInMaxAlignTs(STACK_ITEM_SIZE)> v;
    𐡄𐡍 (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
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
ساكن فراغ
subQuickSort(حرف *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, ثابت فراغ *context,
             فراغ *px, فراغ *pw) {
    int32_t left, right;

    /* start and left are inclusive, limit and right are exclusive */
    𐡏𐡁𐡃 {
        𐡄𐡍((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            اكسر;
        }

        left=start;
        right=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        𐡏𐡁𐡃 {
            طالما(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            طالما(/* x<array[right-1] */
                  cmp(context, px, array+(right-1)*itemSize)<0
            ) {
                --right;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            𐡄𐡍(left<right) {
                --right;

                𐡄𐡍(left<right) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)right*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)right*itemSize, pw, itemSize);
                }

                ++left;
            }
        } طالما(left<right);

        /* sort sub-arrays */
        𐡄𐡍((right-start)<(limit-left)) {
            /* sort [start..right[ */
            𐡄𐡍(start<(right-1)) {
                subQuickSort(array, start, right, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } وإلا {
            /* sort [left..limit[ */
            𐡄𐡍(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=right;
        }
    } طالما(start<(limit-1));
}

ساكن فراغ
quickSort(حرف *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ثابت فراغ *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::MaybeStackArray<std::max_align_t, sizeInMaxAlignTs(STACK_ITEM_SIZE) * 2> xw;
    𐡄𐡍(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI فراغ U_EXPORT2
uprv_sortArray(فراغ *array, int32_t length, int32_t itemSize,
               UComparator *cmp, ثابت فراغ *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    𐡄𐡍(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    𐡄𐡍((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    𐡄𐡍(length<=1) {
        أرجع;
    } وإلا 𐡄𐡍(length<MIN_QSORT || sortStable) {
        insertionSort((حرف *)array, length, itemSize, cmp, context, pErrorCode);
    } وإلا {
        quickSort((حرف *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
