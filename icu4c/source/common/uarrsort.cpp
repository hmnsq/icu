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

#અન્તર્ભાવ <cstddef>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uarrsort.h"

ગણના {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ઢગલો_ITEM_SIZE=200
};

સ્થિર constexpr int32_t sizeInMaxAlignTs(int32_t sizeInBytes) {
    પાછા (sizeInBytes + કદ(std::max_align_t) - 1) / કદ(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    (શૂન્ય)context;
    પાછા (int32_t)*(અચલ uint16_t *)left - (int32_t)*(અચલ uint16_t *)જમણું;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    (શૂન્ય)context;
    પાછા *(અચલ int32_t *)left - *(અચલ int32_t *)જમણું;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    (શૂન્ય)context;
    uint32_t l=*(અચલ uint32_t *)left, r=*(અચલ uint32_t *)જમણું;

    /* compare directly because (l-r) would overflow the int32_t result */
    જો(l<r) {
        પાછા -1;
    } નહીંતર જો(l==r) {
        પાછા 0;
    } નહીંતર /* l>r */ {
        પાછા 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_દ્વિઅંકી_શોધ(અક્ષર *array, int32_t limit, શૂન્ય *item, int32_t itemSize,
                        UComparator *cmp, અચલ શૂન્ય *context) {
    int32_t start=0;
    UBool found=ખોટું;

    /* Binary search until we get down to a tiny sub-array. */
    જ્યારે((limit-start)>=MIN_QSORT) {
        int32_t i=(start+limit)/2;
        int32_t diff=cmp(context, item, array+i*itemSize);
        જો(diff==0) {
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
            found=સાચું;
            start=i+1;
        } નહીંતર જો(diff<0) {
            limit=i;
        } નહીંતર {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    જ્યારે(start<limit) {
        int32_t diff=cmp(context, item, array+start*itemSize);
        જો(diff==0) {
            found=સાચું;
        } નહીંતર જો(diff<0) {
            તોડો;
        }
        ++start;
    }
    પાછા found ? (start-1) : ~start;
}

સ્થિર શૂન્ય
doInsertionSort(અક્ષર *array, int32_t length, int32_t itemSize,
                UComparator *cmp, અચલ શૂન્ય *context, શૂન્ય *pv) {
    int32_t j;

    માટે(j=1; j<length; ++j) {
        અક્ષર *item=array+j*itemSize;
        int32_t insertionPoint=uprv_stable_દ્વિઅંકી_શોધ(array, j, item, itemSize, cmp, context);
        જો(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } નહીંતર {
            ++insertionPoint;  /* one past the last equal item */
        }
        જો(insertionPoint<j) {
            અક્ષર *dest=array+insertionPoint*itemSize;
            uprv_memcpy(pv, item, itemSize);  /* v=array[j] */
            uprv_memmove(dest+itemSize, dest, (j-insertionPoint)*(size_t)itemSize);
            uprv_memcpy(dest, pv, itemSize);  /* array[insertionPoint]=v */
        }
    }
}

સ્થિર શૂન્ય
insertionSort(અક્ષર *array, int32_t length, int32_t itemSize,
              UComparator *cmp, અચલ શૂન્ય *context, UErrorCode *pErrorCode) {

    icu::Maybe_ઢગલો_Array<std::max_align_t, sizeInMaxAlignTs(ઢગલો_ITEM_SIZE)> v;
    જો (sizeInMaxAlignTs(itemSize) > v.getCapacity() &&
            v.resize(sizeInMaxAlignTs(itemSize)) == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
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
સ્થિર શૂન્ય
subQuickSort(અક્ષર *array, int32_t start, int32_t limit, int32_t itemSize,
             UComparator *cmp, અચલ શૂન્ય *context,
             શૂન્ય *px, શૂન્ય *pw) {
    int32_t left, જમણું;

    /* start and left are inclusive, limit and right are exclusive */
    કરો {
        જો((start+MIN_QSORT)>=limit) {
            doInsertionSort(array+start*itemSize, limit-start, itemSize, cmp, context, px);
            તોડો;
        }

        left=start;
        જમણું=limit;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+limit)/2)*itemSize, itemSize);

        કરો {
            જ્યારે(/* array[left]<x */
                  cmp(context, array+left*itemSize, px)<0
            ) {
                ++left;
            }
            જ્યારે(/* x<array[right-1] */
                  cmp(context, px, array+(જમણું-1)*itemSize)<0
            ) {
                --જમણું;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            જો(left<જમણું) {
                --જમણું;

                જો(left<જમણું) {
                    uprv_memcpy(pw, array+(size_t)left*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)left*itemSize, array+(size_t)જમણું*itemSize, itemSize);
                    uprv_memcpy(array+(size_t)જમણું*itemSize, pw, itemSize);
                }

                ++left;
            }
        } જ્યારે(left<જમણું);

        /* sort sub-arrays */
        જો((જમણું-start)<(limit-left)) {
            /* sort [start..right[ */
            જો(start<(જમણું-1)) {
                subQuickSort(array, start, જમણું, itemSize, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=left;
        } નહીંતર {
            /* sort [left..limit[ */
            જો(left<(limit-1)) {
                subQuickSort(array, left, limit, itemSize, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            limit=જમણું;
        }
    } જ્યારે(start<(limit-1));
}

સ્થિર શૂન્ય
quickSort(અક્ષર *array, int32_t length, int32_t itemSize,
            UComparator *cmp, અચલ શૂન્ય *context, UErrorCode *pErrorCode) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ઢગલો_Array<std::max_align_t, sizeInMaxAlignTs(ઢગલો_ITEM_SIZE) * 2> xw;
    જો(sizeInMaxAlignTs(itemSize)*2 > xw.getCapacity() &&
            xw.resize(sizeInMaxAlignTs(itemSize) * 2) == nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    subQuickSort(array, 0, length, itemSize, cmp, context,
                 xw.getAlias(), xw.getAlias() + sizeInMaxAlignTs(itemSize));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI શૂન્ય U_EXPORT2
uprv_sortArray(શૂન્ય *array, int32_t length, int32_t itemSize,
               UComparator *cmp, અચલ શૂન્ય *context,
               UBool sortStable, UErrorCode *pErrorCode) {
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    જો((length>0 && array==nullptr) || length<0 || itemSize<=0 || cmp==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    જો(length<=1) {
        પાછા;
    } નહીંતર જો(length<MIN_QSORT || sortStable) {
        insertionSort((અક્ષર *)array, length, itemSize, cmp, context, pErrorCode);
    } નહીંતર {
        quickSort((અક્ષર *)array, length, itemSize, cmp, context, pErrorCode);
    }
}
