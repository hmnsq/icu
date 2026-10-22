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

#അന്തര്ഭാവ <cstddef>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uarrsort.h"

എണ്ണൽ {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    അട്ടി_ITEM_SIZE=200
};

സ്ഥിതം constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    തിരികെ (sizeInBytes + വലുപ്പം(std::max_align_t) - 1) / വലുപ്പം(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    (ശൂന്യം)context;
    തിരികെ (int32_t)*(സ്ഥിരം uint16_t *)left - (int32_t)*(സ്ഥിരം uint16_t *)വലത്;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    (ശൂന്യം)context;
    തിരികെ *(സ്ഥിരം int32_t *)left - *(സ്ഥിരം int32_t *)വലത്;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    (ശൂന്യം)context;
    uint32_t l=*(സ്ഥിരം uint32_t *)left, r=*(സ്ഥിരം uint32_t *)വലത്;

    /* compare directly because (l-r) would overflow the int32_t result */
    എങ്കിൽ(l<r) {
        തിരികെ -1;
    } അല്ലെങ്കിൽ എങ്കിൽ(l==r) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ /* l>r */ {
        തിരികെ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ദ്വിമാന_തിരയൽ(അക്ഷരം *array, int32_t limit, ശൂന്യം *item, int32_t itemSize,
                        UComparator *cmp, സ്ഥിരം ശൂന്യം *context) {
    int32_t start=0;
    UBool found=അസത്യം;

    /* Binary search until we get down to a tiny sub-array. */
    ആകുമ്പോൾ((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        എങ്കിൽ(diff==0) {
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
            found=സത്യം;
            start=i+1;
        } അല്ലെങ്കിൽ എങ്കിൽ(diff<0) {
            limit=i;
        } അല്ലെങ്കിൽ {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ആകുമ്പോൾ(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        എങ്കിൽ(diff==0) {
            found=സത്യം;
        } അല്ലെങ്കിൽ എങ്കിൽ(diff<0) {
            നിർത്തുക;
        }
        ++start;
    }
    തിരികെ found ? (start-1) : ~start;
}

സ്ഥിതം ശൂന്യം
doInsertionSort(അക്ഷരം *array, int32_t length, int32_t itemSize,
                UComparator *cmp, സ്ഥിരം ശൂന്യം *context, ശൂന്യം *pv) {
    int32_t j;

    വേണ്ടി(j=1; j<length; ++j) {
        അക്ഷരം *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_ദ്വിമാന_തിരയൽ(array, j, item, itemSize, cmp, context);
        എങ്കിൽ(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } അല്ലെങ്കിൽ {
            ++insertionPoint;  /* one past the last equal item */
        }
        എങ്കിൽ(insertionPoint<j) {
            അക്ഷരം *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

സ്ഥിതം ശൂന്യം
insertionSort(അക്ഷരം *array, int32_t length, int32_t itemSize,
              UComparator *cmp, സ്ഥിരം ശൂന്യം *context, UErrorCode *pErrorCode) {

    icu::Maybe_അട്ടി_Array<std::max_align_t, sizeInMaxAlignTs(അട്ടി_ITEM_SIZE)> v;
    എങ്കിൽ (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
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
സ്ഥിതം ശൂന്യം
subQuickSort(അക്ഷരം *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, സ്ഥിരം ശൂന്യം *context,
             ശൂന്യം *px, ശൂന്യം *pw) {
    int32_t left, വലത്;

    /* start and left are inclusive, limit and right are exclusive */
    ചെയ്യുക {
        എങ്കിൽ((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            നിർത്തുക;
        }

        left=start;
        വലത്=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        ചെയ്യുക {
            ആകുമ്പോൾ(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            ആകുമ്പോൾ(/* x<array[right-1] */
                  cmp(context, px, array+(വലത്-1)*itemSize)<0
            ) {
                --വലത്;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            എങ്കിൽ(left<വലത്) {
                --വലത്;

                എങ്കിൽ(left<വലത്) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)വലത്*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)വലത്*itemSize, pw, itemSize);
                }

                ++left;
            }
        } ആകുമ്പോൾ(left<വലത്);

        /* sort sub-arrays */
        എങ്കിൽ((വലത്-start)<(limit-left)) {
            /* sort [start..right[ */
            എങ്കിൽ(start<(വലത്-1)) {
                subQuickSort(array, start, വലത്, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } അല്ലെങ്കിൽ {
            /* sort [left..limit[ */
            എങ്കിൽ(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=വലത്;
        }
    } ആകുമ്പോൾ(start<(limit-1));
}

സ്ഥിതം ശൂന്യം
quickSort(അക്ഷരം *array, int32_t length, int32_t itemSize,
            UComparator *cmp, സ്ഥിരം ശൂന്യം *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_അട്ടി_Array<std::max_align_t, sizeInMaxAlignTs(അട്ടി_ITEM_SIZE) * 2> xw;
    എങ്കിൽ(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI ശൂന്യം U_EXPORT2
uprv_sortArray(ശൂന്യം *array, int32_t length, int32_t itemSize,
               UComparator *cmp, സ്ഥിരം ശൂന്യം *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    എങ്കിൽ(length<=1) {
        തിരികെ;
    } അല്ലെങ്കിൽ എങ്കിൽ(length<MIN_QSORT || sortStable) {
        insertionSort((അക്ഷരം *)array, length, itemSize, cmp, context, pErrorCode);
    } അല്ലെങ്കിൽ {
        quickSort((അക്ഷരം *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
