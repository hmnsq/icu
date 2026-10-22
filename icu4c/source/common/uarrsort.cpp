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
    مكدس_ITEM_SIZE=200
};

ساكن constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    إرجاع (sizeInBytes + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *يمين) {
    (فراغ)context;
    إرجاع (int32_t)*(ثابت uint16_t *)left - (int32_t)*(ثابت uint16_t *)يمين;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *يمين) {
    (فراغ)context;
    إرجاع *(ثابت int32_t *)left - *(ثابت int32_t *)يمين;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *يمين) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)left, r=*(ثابت uint32_t *)يمين;

    /* compare directly because (l-r) would overflow the int32_t result */
    إذا(l<r) {
        إرجاع -1;
    } وإلا إذا(l==r) {
        إرجاع 0;
    } وإلا /* l>r */ {
        إرجاع 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ثنائي_بحث(حرف *array, int32_t limit, فراغ *item, int32_t itemSize,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t start=0;
    UBool found=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    بينما((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        إذا(diff==0) {
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
        } وإلا إذا(diff<0) {
            limit=i;
        } وإلا {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    بينما(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        إذا(diff==0) {
            found=true;
        } وإلا إذا(diff<0) {
            اكسر;
        }
        ++start;
    }
    إرجاع found ? (start-1) : ~start;
}

ساكن فراغ
doInsertionSort(حرف *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_ثنائي_بحث(array, j, item, itemSize, cmp, context);
        إذا(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } وإلا {
            ++insertionPoint;  /* one past the last equal item */
        }
        إذا(insertionPoint<j) {
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

    icu::Maybe_مكدس_Array<std::max_align_t, sizeInMaxAlignTs(مكدس_ITEM_SIZE)> v;
    إذا (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        إرجاع;
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
    int32_t left, يمين;

    /* start and left are inclusive, limit and right are exclusive */
    افعل {
        إذا((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            اكسر;
        }

        left=start;
        يمين=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        افعل {
            بينما(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            بينما(/* x<array[right-1] */
                  cmp(context, px, array+(يمين-1)*itemSize)<0
            ) {
                --يمين;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            إذا(left<يمين) {
                --يمين;

                إذا(left<يمين) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)يمين*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)يمين*itemSize, pw, itemSize);
                }

                ++left;
            }
        } بينما(left<يمين);

        /* sort sub-arrays */
        إذا((يمين-start)<(limit-left)) {
            /* sort [start..right[ */
            إذا(start<(يمين-1)) {
                subQuickSort(array, start, يمين, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } وإلا {
            /* sort [left..limit[ */
            إذا(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=يمين;
        }
    } بينما(start<(limit-1));
}

ساكن فراغ
quickSort(حرف *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ثابت فراغ *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_مكدس_Array<std::max_align_t, sizeInMaxAlignTs(مكدس_ITEM_SIZE) * 2> xw;
    إذا(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        إرجاع;
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
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        إرجاع;
    }
    إذا((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع;
    }

    إذا(length<=1) {
        إرجاع;
    } وإلا إذا(length<MIN_QSORT || sortStable) {
        insertionSort((حرف *)array, length, itemSize, cmp, context, pErrorCode);
    } وإلا {
        quickSort((حرف *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
