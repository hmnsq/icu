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

#समावेश <cstddef>

#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "uarrsort.h"

क्रमागत {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ढेर_ITEM_आकार=200
};

अटल constexpr int32_t आकार_In_Max_Align_Ts(int32_t आकार_In_Bytes) {
    वापस (आकार_In_Bytes + माप(std::max_align_t) - 1) / माप(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    (व्योम)context;
    वापस (int32_t)*(स्थिर uint16_t *)बाँए - (int32_t)*(स्थिर uint16_t *)दाँए;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    (व्योम)context;
    वापस *(स्थिर int32_t *)बाँए - *(स्थिर int32_t *)दाँए;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    (व्योम)context;
    uint32_t l=*(स्थिर uint32_t *)बाँए, r=*(स्थिर uint32_t *)दाँए;

    /* compare directly because (l-r) would overflow the int32_t result */
    अगर(l<r) {
        वापस -1;
    } अन्यथा अगर(l==r) {
        वापस 0;
    } अन्यथा /* l>r */ {
        वापस 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_द्विआधारी_खोज(अक्षर *array, int32_t limit, व्योम *item, int32_t item_आकार,
                        UComparator *cmp, स्थिर व्योम *context) {
    int32_t start=0;
    UBool found=false;

    /* Binary search until we get down to a tiny sub-array. */
    जबतक((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*item_आकार);
        अगर(diff==0) {
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
        } अन्यथा अगर(diff<0) {
            limit=i;
        } अन्यथा {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    जबतक(start<limit) {
        int32_t diff=cmp(context, item, array+start*item_आकार);
        अगर(diff==0) {
            found=true;
        } अन्यथा अगर(diff<0) {
            अवरोध;
        }
        ++start;
    }
    वापस found ? (start-1) : ~start;
}

अटल व्योम
doInsertionSort(अक्षर *array, int32_t length, int32_t item_आकार,
                UComparator *cmp, स्थिर व्योम *context, व्योम *pv) {
    int32_t j;

    क्रम(j=1; j<length; ++j) {
        अक्षर *item=array+j*item_आकार;
        int32_t insertionPoint=uprv_stable_द्विआधारी_खोज(array, j, item, item_आकार, cmp, context);
        अगर(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } अन्यथा {
            ++insertionPoint;  /* one past the last equal item */
        }
        अगर(insertionPoint<j) {
            अक्षर *dest=array+insertionPoint*item_आकार;
            uprv_memcpy(pv, item, item_आकार);  /* v=array[j] */
            uprv_memmove(dest+item_आकार, dest, (j-insertionPoint)*(माप_प्रकार)item_आकार);
            uprv_memcpy(dest, pv, item_आकार);  /* array[insertionPoint]=v */
        }
    }
}

अटल व्योम
insertionSort(अक्षर *array, int32_t length, int32_t item_आकार,
              UComparator *cmp, स्थिर व्योम *context, UErrorCode *pErrorCode) {

    icu::Maybe_ढेर_Array<std::max_align_t, आकार_In_Max_Align_Ts(ढेर_ITEM_आकार)> v;
    अगर (आकार_In_Max_Align_Ts(item_आकार) > v.getCapacity() &&
            v.resize(आकार_In_Max_Align_Ts(item_आकार)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    doInsertionSort(array, length, item_आकार, cmp, context, v.getAlias());
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
अटल व्योम
subQuickSort(अक्षर *array, int32_t start, int32_t limit, int32_t item_आकार,
             UComparator *cmp, स्थिर व्योम *context,
             व्योम *px, व्योम *pw) {
    int32_t बाँए, दाँए;

    /* start and left are inclusive, limit and right are exclusive */
    करो {
        अगर((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*item_आकार, limit-start, item_आकार, cmp, context, px);
            अवरोध;
        }

        बाँए=start;
        दाँए=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(माप_प्रकार)((start+limit)/2)*item_आकार, item_आकार);

        करो {
            जबतक(/* array[left]<x */
                  cmp(context, array+बाँए*item_आकार, px)<0
            ) {
                ++बाँए;
            }
            जबतक(/* x<array[right-1] */
                  cmp(context, px, array+(दाँए-1)*item_आकार)<0
            ) {
                --दाँए;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            अगर(बाँए<दाँए) {
                --दाँए;

                अगर(बाँए<दाँए) {
                    uprv_memcpy(pw, array+(माप_प्रकार)बाँए*item_आकार, item_आकार);
                    uprv_memcpy(array+(माप_प्रकार)बाँए*item_आकार, array+(माप_प्रकार)दाँए*item_आकार, item_आकार);
                    uprv_memcpy(array+(माप_प्रकार)दाँए*item_आकार, pw, item_आकार);
                }

                ++बाँए;
            }
        } जबतक(बाँए<दाँए);

        /* sort sub-arrays */
        अगर((दाँए-start)<(limit-बाँए)) {
            /* sort [start..right[ */
            अगर(start<(दाँए-1)) {
                subQuickSort(array, start, दाँए, item_आकार, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=बाँए;
        } अन्यथा {
            /* sort [left..limit[ */
            अगर(बाँए<(limit-1)) {
                subQuickSort(array, बाँए, limit, item_आकार, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=दाँए;
        }
    } जबतक(start<(limit-1));
}

अटल व्योम
quickSort(अक्षर *array, int32_t length, int32_t item_आकार,
            UComparator *cmp, स्थिर व्योम *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ढेर_Array<std::max_align_t, आकार_In_Max_Align_Ts(ढेर_ITEM_आकार) * 2> xw;
    अगर(आकार_In_Max_Align_Ts(item_आकार)*2 > xw.getCapacity() &&
            xw.resize(आकार_In_Max_Align_Ts(item_आकार) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    subQuickSort(array, 0, length, item_आकार, cmp, context,
                 xw.getAlias(), xw.getAlias() + आकार_In_Max_Align_Ts(item_आकार));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI व्योम U_EXPORT2
uprv_sortArray(व्योम *array, int32_t length, int32_t item_आकार,
               UComparator *cmp, स्थिर व्योम *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर((length>0 && array==nullptr) || length<0 || item_आकार<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर(length<=1) {
        वापस;
    } अन्यथा अगर(length<MIN_QSORT || sortStable) {
        insertionSort((अक्षर *)array, length, item_आकार, cmp, context, pErrorCode);
    } अन्यथा {
        quickSort((अक्षर *)array, length, item_आकार, cmp, context, pErrorCode);
    }
}
