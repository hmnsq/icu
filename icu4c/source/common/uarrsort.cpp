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

#अन्तर्भाव <cstddef>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uarrsort.h"

गणना {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    थाक_ITEM_SIZE=200
};

स्थिर constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    फर्काऊ (sizeInBytes + परिमाण(std::max_align_t) - 1) / परिमाण(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    (शून्य)context;
    फर्काऊ (int32_t)*(अचर uint16_t *)left - (int32_t)*(अचर uint16_t *)दायाँ;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    (शून्य)context;
    फर्काऊ *(अचर int32_t *)left - *(अचर int32_t *)दायाँ;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    (शून्य)context;
    uint32_t l=*(अचर uint32_t *)left, r=*(अचर uint32_t *)दायाँ;

    /* compare directly because (l-r) would overflow the int32_t result */
    यदि(l<r) {
        फर्काऊ -1;
    } अन्यथा यदि(l==r) {
        फर्काऊ 0;
    } अन्यथा /* l>r */ {
        फर्काऊ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_द्विआधारी_खोज(वर्ण *array, int32_t limit, शून्य *item, int32_t itemSize,
                        UComparator *cmp, अचर शून्य *context) {
    int32_t start=0;
    UBool found=असत्य;

    /* Binary search until we get down to a tiny sub-array. */
    जबसम्म((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        यदि(diff==0) {
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
            found=सत्य;
            start=i+1;
        } अन्यथा यदि(diff<0) {
            limit=i;
        } अन्यथा {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    जबसम्म(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        यदि(diff==0) {
            found=सत्य;
        } अन्यथा यदि(diff<0) {
            रोक;
        }
        ++start;
    }
    फर्काऊ found ? (start-1) : ~start;
}

स्थिर शून्य
doInsertionSort(वर्ण *array, int32_t length, int32_t itemSize,
                UComparator *cmp, अचर शून्य *context, शून्य *pv) {
    int32_t j;

    लागि(j=1; j<length; ++j) {
        वर्ण *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_द्विआधारी_खोज(array, j, item, itemSize, cmp, context);
        यदि(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } अन्यथा {
            ++insertionPoint;  /* one past the last equal item */
        }
        यदि(insertionPoint<j) {
            वर्ण *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(माप_प्रकार)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

स्थिर शून्य
insertionSort(वर्ण *array, int32_t length, int32_t itemSize,
              UComparator *cmp, अचर शून्य *context, UErrorCode *pErrorCode) {

    icu::Maybe_थाक_Array<std::max_align_t, sizeInMaxAlignTs(थाक_ITEM_SIZE)> v;
    यदि (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
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
स्थिर शून्य
subQuickSort(वर्ण *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, अचर शून्य *context,
             शून्य *px, शून्य *pw) {
    int32_t left, दायाँ;

    /* start and left are inclusive, limit and right are exclusive */
    गर {
        यदि((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            रोक;
        }

        left=start;
        दायाँ=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(माप_प्रकार)((start+limit)/2)*itemSize, itemSize);

        गर {
            जबसम्म(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            जबसम्म(/* x<array[right-1] */
                  cmp(context, px, array+(दायाँ-1)*itemSize)<0
            ) {
                --दायाँ;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            यदि(left<दायाँ) {
                --दायाँ;

                यदि(left<दायाँ) {
                    uprv_memcpy(pw, array+(माप_प्रकार)left*itemSize, itemSize);
                    uprv_memcpy(array+(माप_प्रकार)left*itemSize, array+(माप_प्रकार)दायाँ*itemSize, itemSize);
                    uprv_memcpy(array+(माप_प्रकार)दायाँ*itemSize, pw, itemSize);
                }

                ++left;
            }
        } जबसम्म(left<दायाँ);

        /* sort sub-arrays */
        यदि((दायाँ-start)<(limit-left)) {
            /* sort [start..right[ */
            यदि(start<(दायाँ-1)) {
                subQuickSort(array, start, दायाँ, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } अन्यथा {
            /* sort [left..limit[ */
            यदि(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=दायाँ;
        }
    } जबसम्म(start<(limit-1));
}

स्थिर शून्य
quickSort(वर्ण *array, int32_t length, int32_t itemSize,
            UComparator *cmp, अचर शून्य *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_थाक_Array<std::max_align_t, sizeInMaxAlignTs(थाक_ITEM_SIZE) * 2> xw;
    यदि(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI शून्य U_EXPORT2
uprv_sortArray(शून्य *array, int32_t length, int32_t itemSize,
               UComparator *cmp, अचर शून्य *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ;
    }
    यदि((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    यदि(length<=1) {
        फर्काऊ;
    } अन्यथा यदि(length<MIN_QSORT || sortStable) {
        insertionSort((वर्ण *)array, length, itemSize, cmp, context, pErrorCode);
    } अन्यथा {
        quickSort((वर्ण *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
