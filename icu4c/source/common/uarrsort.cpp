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

#அந்தர்பாவ <cstddef>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uarrsort.h"

எண்ணிக்கை {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    அடுக்கு_ITEM_SIZE=200
};

நிலையான constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    திரும்பு (sizeInBytes + அளவு(std::max_align_t) - 1) / அளவு(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    (வெறுமை)context;
    திரும்பு (int32_t)*(மாறிலி uint16_t *)left - (int32_t)*(மாறிலி uint16_t *)வலது;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    (வெறுமை)context;
    திரும்பு *(மாறிலி int32_t *)left - *(மாறிலி int32_t *)வலது;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    (வெறுமை)context;
    uint32_t l=*(மாறிலி uint32_t *)left, r=*(மாறிலி uint32_t *)வலது;

    /* compare directly because (l-r) would overflow the int32_t result */
    எனில்(l<r) {
        திரும்பு -1;
    } இல்லையெனில் எனில்(l==r) {
        திரும்பு 0;
    } இல்லையெனில் /* l>r */ {
        திரும்பு 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_இருமம்_தேடல்(எழுத்து *array, int32_t limit, வெறுமை *item, int32_t itemSize,
                        UComparator *cmp, மாறிலி வெறுமை *context) {
    int32_t start=0;
    UBool found=பொய்;

    /* Binary search until we get down to a tiny sub-array. */
    போது((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        எனில்(diff==0) {
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
            found=உண்மை;
            start=i+1;
        } இல்லையெனில் எனில்(diff<0) {
            limit=i;
        } இல்லையெனில் {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    போது(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        எனில்(diff==0) {
            found=உண்மை;
        } இல்லையெனில் எனில்(diff<0) {
            நிறுத்து;
        }
        ++start;
    }
    திரும்பு found ? (start-1) : ~start;
}

நிலையான வெறுமை
doInsertionSort(எழுத்து *array, int32_t length, int32_t itemSize,
                UComparator *cmp, மாறிலி வெறுமை *context, வெறுமை *pv) {
    int32_t j;

    க்கு(j=1; j<length; ++j) {
        எழுத்து *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_இருமம்_தேடல்(array, j, item, itemSize, cmp, context);
        எனில்(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } இல்லையெனில் {
            ++insertionPoint;  /* one past the last equal item */
        }
        எனில்(insertionPoint<j) {
            எழுத்து *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

நிலையான வெறுமை
insertionSort(எழுத்து *array, int32_t length, int32_t itemSize,
              UComparator *cmp, மாறிலி வெறுமை *context, UErrorCode *pErrorCode) {

    icu::Maybe_அடுக்கு_Array<std::max_align_t, sizeInMaxAlignTs(அடுக்கு_ITEM_SIZE)> v;
    எனில் (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
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
நிலையான வெறுமை
subQuickSort(எழுத்து *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, மாறிலி வெறுமை *context,
             வெறுமை *px, வெறுமை *pw) {
    int32_t left, வலது;

    /* start and left are inclusive, limit and right are exclusive */
    செய் {
        எனில்((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            நிறுத்து;
        }

        left=start;
        வலது=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        செய் {
            போது(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            போது(/* x<array[right-1] */
                  cmp(context, px, array+(வலது-1)*itemSize)<0
            ) {
                --வலது;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            எனில்(left<வலது) {
                --வலது;

                எனில்(left<வலது) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)வலது*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)வலது*itemSize, pw, itemSize);
                }

                ++left;
            }
        } போது(left<வலது);

        /* sort sub-arrays */
        எனில்((வலது-start)<(limit-left)) {
            /* sort [start..right[ */
            எனில்(start<(வலது-1)) {
                subQuickSort(array, start, வலது, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } இல்லையெனில் {
            /* sort [left..limit[ */
            எனில்(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=வலது;
        }
    } போது(start<(limit-1));
}

நிலையான வெறுமை
quickSort(எழுத்து *array, int32_t length, int32_t itemSize,
            UComparator *cmp, மாறிலி வெறுமை *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_அடுக்கு_Array<std::max_align_t, sizeInMaxAlignTs(அடுக்கு_ITEM_SIZE) * 2> xw;
    எனில்(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI வெறுமை U_EXPORT2
uprv_sortArray(வெறுமை *array, int32_t length, int32_t itemSize,
               UComparator *cmp, மாறிலி வெறுமை *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    எனில்((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    எனில்(length<=1) {
        திரும்பு;
    } இல்லையெனில் எனில்(length<MIN_QSORT || sortStable) {
        insertionSort((எழுத்து *)array, length, itemSize, cmp, context, pErrorCode);
    } இல்லையெனில் {
        quickSort((எழுத்து *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
