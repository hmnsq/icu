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

#অন্তর্ভাব <cstddef>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uarrsort.h"

গণনা {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    স্তূপ_ITEM_আকার=200
};

স্থির constexpr int32_t আকার_In_Max_Align_Ts(int32_t আকার_In_Bytes) {
    ফেরত (আকার_In_Bytes + আকার(std::max_align_t) - 1) / আকার(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    (শূন্য)context;
    ফেরত (int32_t)*(ধ্রুবক uint16_t *)left - (int32_t)*(ধ্রুবক uint16_t *)ডান;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    (শূন্য)context;
    ফেরত *(ধ্রুবক int32_t *)left - *(ধ্রুবক int32_t *)ডান;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    (শূন্য)context;
    uint32_t l=*(ধ্রুবক uint32_t *)left, r=*(ধ্রুবক uint32_t *)ডান;

    /* compare directly because (l-r) would overflow the int32_t result */
    যদি(l<r) {
        ফেরত -1;
    } অন্যথা যদি(l==r) {
        ফেরত 0;
    } অন্যথা /* l>r */ {
        ফেরত 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_দ্বিমিক_অনুসন্ধান(অক্ষর *array, int32_t limit, শূন্য *item, int32_t item_আকার,
                        UComparator *cmp, ধ্রুবক শূন্য *context) {
    int32_t start=0;
    UBool found=মিথ্যা;

    /* Binary search until we get down to a tiny sub-array. */
    যখন((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*item_আকার);
        যদি(diff==0) {
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
            found=সত্য;
            start=i+1;
        } অন্যথা যদি(diff<0) {
            limit=i;
        } অন্যথা {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    যখন(start<limit) {
        int32_t diff=cmp(context, item, array+start*item_আকার);
        যদি(diff==0) {
            found=সত্য;
        } অন্যথা যদি(diff<0) {
            বিরতি;
        }
        ++start;
    }
    ফেরত found ? (start-1) : ~start;
}

স্থির শূন্য
doInsertionSort(অক্ষর *array, int32_t length, int32_t item_আকার,
                UComparator *cmp, ধ্রুবক শূন্য *context, শূন্য *pv) {
    int32_t j;

    জন্য(j=1; j<length; ++j) {
        অক্ষর *item=array+j*item_আকার;
        int32_t insertionPoint=uprv_stable_দ্বিমিক_অনুসন্ধান(array, j, item, item_আকার, cmp, context);
        যদি(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } অন্যথা {
            ++insertionPoint;  /* one past the last equal item */
        }
        যদি(insertionPoint<j) {
            অক্ষর *dest=array+insertionPoint*item_আকার;
            uprv_memcpy(pv, item, item_আকার);  /* v=array[j] */
            uprv_memmove(dest+item_আকার, dest, (j-insertionPoint)*(size_t)item_আকার);
            uprv_memcpy(dest, pv, item_আকার);  /* array[insertionPoint]=v */
        }
    }
}

স্থির শূন্য
insertionSort(অক্ষর *array, int32_t length, int32_t item_আকার,
              UComparator *cmp, ধ্রুবক শূন্য *context, UErrorCode *pErrorCode) {

    icu::Maybe_স্তূপ_Array<std::max_align_t, আকার_In_Max_Align_Ts(স্তূপ_ITEM_আকার)> v;
    যদি (আকার_In_Max_Align_Ts(item_আকার) > v.getCapacity() &&
            v.resize(আকার_In_Max_Align_Ts(item_আকার)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    doInsertionSort(array, length, item_আকার, cmp, context, v.getAlias());
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
স্থির শূন্য
subQuickSort(অক্ষর *array, int32_t start, int32_t limit, int32_t item_আকার,
             UComparator *cmp, ধ্রুবক শূন্য *context,
             শূন্য *px, শূন্য *pw) {
    int32_t left, ডান;

    /* start and left are inclusive, limit and right are exclusive */
    করো {
        যদি((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*item_আকার, limit-start, item_আকার, cmp, context, px);
            বিরতি;
        }

        left=start;
        ডান=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*item_আকার, item_আকার);

        করো {
            যখন(/* array[left]<x */
                  cmp(context, array+left*item_আকার, px)<0
            ) {
                ++left;
            }
            যখন(/* x<array[right-1] */
                  cmp(context, px, array+(ডান-1)*item_আকার)<0
            ) {
                --ডান;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            যদি(left<ডান) {
                --ডান;

                যদি(left<ডান) {
                    uprv_memcpy(pw, array+(size_t)left*item_আকার, item_আকার);
                    uprv_memcpy(array+(size_t)left*item_আকার, array+(size_t)ডান*item_আকার, item_আকার);
                    uprv_memcpy(array+(size_t)ডান*item_আকার, pw, item_আকার);
                }

                ++left;
            }
        } যখন(left<ডান);

        /* sort sub-arrays */
        যদি((ডান-start)<(limit-left)) {
            /* sort [start..right[ */
            যদি(start<(ডান-1)) {
                subQuickSort(array, start, ডান, item_আকার, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } অন্যথা {
            /* sort [left..limit[ */
            যদি(left<(limit-1)) {
                subQuickSort(array, left, limit, item_আকার, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=ডান;
        }
    } যখন(start<(limit-1));
}

স্থির শূন্য
quickSort(অক্ষর *array, int32_t length, int32_t item_আকার,
            UComparator *cmp, ধ্রুবক শূন্য *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_স্তূপ_Array<std::max_align_t, আকার_In_Max_Align_Ts(স্তূপ_ITEM_আকার) * 2> xw;
    যদি(আকার_In_Max_Align_Ts(item_আকার)*2 > xw.getCapacity() &&
            xw.resize(আকার_In_Max_Align_Ts(item_আকার) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    subQuickSort(array, 0, length, item_আকার, cmp, context,
                 xw.getAlias(), xw.getAlias() + আকার_In_Max_Align_Ts(item_আকার));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI শূন্য U_EXPORT2
uprv_sortArray(শূন্য *array, int32_t length, int32_t item_আকার,
               UComparator *cmp, ধ্রুবক শূন্য *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি((length>0 && array==nullptr) || length<0 || item_আকার<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    যদি(length<=1) {
        ফেরত;
    } অন্যথা যদি(length<MIN_QSORT || sortStable) {
        insertionSort((অক্ষর *)array, length, item_আকার, cmp, context, pErrorCode);
    } অন্যথা {
        quickSort((অক্ষর *)array, length, item_আকার, cmp, context, pErrorCode);
    }
}
