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
    राशिः_ITEM_SIZE=200
};

स्थिर constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    प्रत्यागम (sizeInBytes + परिमाण(std::max_align_t) - 1) / परिमाण(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    (शून्य)context;
    प्रत्यागम (int32_t)*(ध्रुव uint16_t *)left - (int32_t)*(ध्रुव uint16_t *)दक्षिणः;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    (शून्य)context;
    प्रत्यागम *(ध्रुव int32_t *)left - *(ध्रुव int32_t *)दक्षिणः;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    (शून्य)context;
    uint32_t l=*(ध्रुव uint32_t *)left, r=*(ध्रुव uint32_t *)दक्षिणः;

    /* compare directly because (l-r) would overflow the int32_t result */
    जइ(l<r) {
        प्रत्यागम -1;
    } अण्णहा जइ(l==r) {
        प्रत्यागम 0;
    } अण्णहा /* l>r */ {
        प्रत्यागम 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_द्विमान_अन्वेषणम्(वर्ण *array, int32_t limit, शून्य *item, int32_t itemSize,
                        UComparator *cmp, ध्रुव शून्य *context) {
    int32_t start=0;
    UBool found=मिच्छ;

    /* Binary search until we get down to a tiny sub-array. */
    जाव((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        जइ(diff==0) {
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
            found=सच्च;
            start=i+1;
        } अण्णहा जइ(diff<0) {
            limit=i;
        } अण्णहा {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    जाव(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        जइ(diff==0) {
            found=सच्च;
        } अण्णहा जइ(diff<0) {
            भङ्ग;
        }
        ++start;
    }
    प्रत्यागम found ? (start-1) : ~start;
}

स्थिर शून्य
doInsertionSort(वर्ण *array, int32_t length, int32_t itemSize,
                UComparator *cmp, ध्रुव शून्य *context, शून्य *pv) {
    int32_t j;

    प्रति(j=1; j<length; ++j) {
        वर्ण *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_द्विमान_अन्वेषणम्(array, j, item, itemSize, cmp, context);
        जइ(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } अण्णहा {
            ++insertionPoint;  /* one past the last equal item */
        }
        जइ(insertionPoint<j) {
            वर्ण *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

स्थिर शून्य
insertionSort(वर्ण *array, int32_t length, int32_t itemSize,
              UComparator *cmp, ध्रुव शून्य *context, UErrorCode *pErrorCode) {

    icu::Maybe_राशिः_Array<std::max_align_t, sizeInMaxAlignTs(राशिः_ITEM_SIZE)> v;
    जइ (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
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
             UComparator *cmp, ध्रुव शून्य *context,
             शून्य *px, शून्य *pw) {
    int32_t left, दक्षिणः;

    /* start and left are inclusive, limit and right are exclusive */
    करेहि {
        जइ((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            भङ्ग;
        }

        left=start;
        दक्षिणः=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        करेहि {
            जाव(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            जाव(/* x<array[right-1] */
                  cmp(context, px, array+(दक्षिणः-1)*itemSize)<0
            ) {
                --दक्षिणः;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            जइ(left<दक्षिणः) {
                --दक्षिणः;

                जइ(left<दक्षिणः) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)दक्षिणः*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)दक्षिणः*itemSize, pw, itemSize);
                }

                ++left;
            }
        } जाव(left<दक्षिणः);

        /* sort sub-arrays */
        जइ((दक्षिणः-start)<(limit-left)) {
            /* sort [start..right[ */
            जइ(start<(दक्षिणः-1)) {
                subQuickSort(array, start, दक्षिणः, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } अण्णहा {
            /* sort [left..limit[ */
            जइ(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=दक्षिणः;
        }
    } जाव(start<(limit-1));
}

स्थिर शून्य
quickSort(वर्ण *array, int32_t length, int32_t itemSize,
            UComparator *cmp, ध्रुव शून्य *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_राशिः_Array<std::max_align_t, sizeInMaxAlignTs(राशिः_ITEM_SIZE) * 2> xw;
    जइ(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
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
               UComparator *cmp, ध्रुव शून्य *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    जइ((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    जइ(length<=1) {
        प्रत्यागम;
    } अण्णहा जइ(length<MIN_QSORT || sortStable) {
        insertionSort((वर्ण *)array, length, itemSize, cmp, context, pErrorCode);
    } अण्णहा {
        quickSort((वर्ण *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
