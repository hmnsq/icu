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

#شامل <cstddef>

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "uarrsort.h"

شمارش {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    پشته_ITEM_SIZE=200
};

ایستا constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    بازگشت (sizeInBytes + اندازه(std::max_align_t) - 1) / اندازه(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    (تهی)context;
    بازگشت (int32_t)*(ثابت uint16_t *)left - (int32_t)*(ثابت uint16_t *)راست;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    (تهی)context;
    بازگشت *(ثابت int32_t *)left - *(ثابت int32_t *)راست;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    (تهی)context;
    uint32_t l=*(ثابت uint32_t *)left, r=*(ثابت uint32_t *)راست;

    /* compare directly because (l-r) would overflow the int32_t result */
    اگر(l<r) {
        بازگشت -1;
    } وگرنه اگر(l==r) {
        بازگشت 0;
    } وگرنه /* l>r */ {
        بازگشت 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_دودویی_جستجو(نویسه *array, int32_t limit, تهی *item, int32_t itemSize,
                        UComparator *cmp, ثابت تهی *context) {
    int32_t start=0;
    UBool found=نادرست;

    /* Binary search until we get down to a tiny sub-array. */
    وقتی((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        اگر(diff==0) {
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
            found=درست;
            start=i+1;
        } وگرنه اگر(diff<0) {
            limit=i;
        } وگرنه {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    وقتی(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        اگر(diff==0) {
            found=درست;
        } وگرنه اگر(diff<0) {
            بشکن;
        }
        ++start;
    }
    بازگشت found ? (start-1) : ~start;
}

ایستا تهی
doInsertionSort(نویسه *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ثابت تهی *context, تهی *pv) {
    int32_t j;

    برای(j=1; j<length; ++j) {
        نویسه *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_دودویی_جستجو(array, j, item, itemSize, cmp, context);
        اگر(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } وگرنه {
            ++insertionPoint;  /* one past the last equal item */
        }
        اگر(insertionPoint<j) {
            نویسه *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

ایستا تهی
insertionSort(نویسه *array, int32_t length, int32_t itemSize,
              UComparator *cmp, ثابت تهی *context, UErrorCode *pErrorCode) {

    icu::Maybe_پشته_Array<std::max_align_t, sizeInMaxAlignTs(پشته_ITEM_SIZE)> v;
    اگر (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
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
ایستا تهی
subQuickSort(نویسه *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, ثابت تهی *context,
             تهی *px, تهی *pw) {
    int32_t left, راست;

    /* start and left are inclusive, limit and right are exclusive */
    انجامبده {
        اگر((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            بشکن;
        }

        left=start;
        راست=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        انجامبده {
            وقتی(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            وقتی(/* x<array[right-1] */
                  cmp(context, px, array+(راست-1)*itemSize)<0
            ) {
                --راست;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            اگر(left<راست) {
                --راست;

                اگر(left<راست) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)راست*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)راست*itemSize, pw, itemSize);
                }

                ++left;
            }
        } وقتی(left<راست);

        /* sort sub-arrays */
        اگر((راست-start)<(limit-left)) {
            /* sort [start..right[ */
            اگر(start<(راست-1)) {
                subQuickSort(array, start, راست, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } وگرنه {
            /* sort [left..limit[ */
            اگر(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=راست;
        }
    } وقتی(start<(limit-1));
}

ایستا تهی
quickSort(نویسه *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ثابت تهی *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_پشته_Array<std::max_align_t, sizeInMaxAlignTs(پشته_ITEM_SIZE) * 2> xw;
    اگر(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI تهی U_EXPORT2
uprv_sortArray(تهی *array, int32_t length, int32_t itemSize,
               UComparator *cmp, ثابت تهی *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت;
    }
    اگر((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    اگر(length<=1) {
        بازگشت;
    } وگرنه اگر(length<MIN_QSORT || sortStable) {
        insertionSort((نویسه *)array, length, itemSize, cmp, context, pErrorCode);
    } وگرنه {
        quickSort((نویسه *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
