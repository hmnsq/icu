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
    राशिः_ITEM_आकार=200
};

स्थिर constexpr int32_t आकार_अन्तर्_Max_Align_Ts(int32_t आकार_अन्तर्_अष्टकानि) {
    प्रत्यागम (आकार_अन्तर्_अष्टकानि + परिमाण(std::max_align_t) - 1) / परिमाण(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    (शून्य)context;
    प्रत्यागम (int32_t)*(ध्रुव uint16_t *)वामतः - (int32_t)*(ध्रुव uint16_t *)दक्षिणतः;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    (शून्य)context;
    प्रत्यागम *(ध्रुव int32_t *)वामतः - *(ध्रुव int32_t *)दक्षिणतः;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    (शून्य)context;
    uint32_t l=*(ध्रुव uint32_t *)वामतः, r=*(ध्रुव uint32_t *)दक्षिणतः;

    /* compare directly because (l-r) would overflow the int32_t result */
    यदि(l<r) {
        प्रत्यागम -1;
    } अन्यथा यदि(l==r) {
        प्रत्यागम 0;
    } अन्यथा /* l>r */ {
        प्रत्यागम 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_द्विमान_अन्वेषणम्(वर्ण *array, int32_t सीमा, शून्य *item, int32_t item_आकार,
                        UComparator *cmp, ध्रुव शून्य *context) {
    int32_t start=0;
    UBool प्राप्तम्=असत्य;

    /* Binary search until we get down to a tiny sub-array. */
    यावत्((सीमा-start)>=MIN_QSORT) {
        int32_t i=(start+सीमा)/2;
        int32_t diff=cmp(context, item, array+i*item_आकार);
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
            प्राप्तम्=सत्य;
            start=i+1;
        } अन्यथा यदि(diff<0) {
            सीमा=i;
        } अन्यथा {
            start=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    यावत्(start<सीमा) {
        int32_t diff=cmp(context, item, array+start*item_आकार);
        यदि(diff==0) {
            प्राप्तम्=सत्य;
        } अन्यथा यदि(diff<0) {
            भङ्ग;
        }
        ++start;
    }
    प्रत्यागम प्राप्तम् ? (start-1) : ~start;
}

स्थिर शून्य
do_Insertion_क्रमय(वर्ण *array, int32_t length, int32_t item_आकार,
                UComparator *cmp, ध्रुव शून्य *context, शून्य *pv) {
    int32_t j;

    प्रति(j=1; j<length; ++j) {
        वर्ण *item=array+j*item_आकार;
        int32_t insertionPoint=uprv_stable_द्विमान_अन्वेषणम्(array, j, item, item_आकार, cmp, context);
        यदि(insertionPoint<0) {
            insertionPoint=~insertionPoint;
        } अन्यथा {
            ++insertionPoint;  /* one past the last equal item */
        }
        यदि(insertionPoint<j) {
            वर्ण *dest=array+insertionPoint*item_आकार;
            uprv_memcpy(pv, item, item_आकार);  /* v=array[j] */
            uprv_memmove(dest+item_आकार, dest, (j-insertionPoint)*(size_t)item_आकार);
            uprv_memcpy(dest, pv, item_आकार);  /* array[insertionPoint]=v */
        }
    }
}

स्थिर शून्य
insertion_क्रमय(वर्ण *array, int32_t length, int32_t item_आकार,
              UComparator *cmp, ध्रुव शून्य *context, UError_संहिता *p_Error_संहिता) {

    icu::Maybe_राशिः_Array<std::max_align_t, आकार_अन्तर्_Max_Align_Ts(राशिः_ITEM_आकार)> v;
    यदि (आकार_अन्तर्_Max_Align_Ts(item_आकार) > v.get_क्षमता() &&
            v.resize(आकार_अन्तर्_Max_Align_Ts(item_आकार)) == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    do_Insertion_क्रमय(array, length, item_आकार, cmp, context, v.getAlias());
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
sub_शीघ्र_क्रमय(वर्ण *array, int32_t start, int32_t सीमा, int32_t item_आकार,
             UComparator *cmp, ध्रुव शून्य *context,
             शून्य *px, शून्य *pw) {
    int32_t वामतः, दक्षिणतः;

    /* start and left are inclusive, limit and right are exclusive */
    कुरु {
        यदि((start+MIN_QSORT)>=सीमा) {
            do_Insertion_क्रमय(array+start*item_आकार, सीमा-start, item_आकार, cmp, context, px);
            भङ्ग;
        }

        वामतः=start;
        दक्षिणतः=सीमा;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((start+सीमा)/2)*item_आकार, item_आकार);

        कुरु {
            यावत्(/* array[left]<x */
                  cmp(context, array+वामतः*item_आकार, px)<0
            ) {
                ++वामतः;
            }
            यावत्(/* x<array[right-1] */
                  cmp(context, px, array+(दक्षिणतः-1)*item_आकार)<0
            ) {
                --दक्षिणतः;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            यदि(वामतः<दक्षिणतः) {
                --दक्षिणतः;

                यदि(वामतः<दक्षिणतः) {
                    uprv_memcpy(pw, array+(size_t)वामतः*item_आकार, item_आकार);
                    uprv_memcpy(array+(size_t)वामतः*item_आकार, array+(size_t)दक्षिणतः*item_आकार, item_आकार);
                    uprv_memcpy(array+(size_t)दक्षिणतः*item_आकार, pw, item_आकार);
                }

                ++वामतः;
            }
        } यावत्(वामतः<दक्षिणतः);

        /* sort sub-arrays */
        यदि((दक्षिणतः-start)<(सीमा-वामतः)) {
            /* sort [start..right[ */
            यदि(start<(दक्षिणतः-1)) {
                sub_शीघ्र_क्रमय(array, start, दक्षिणतः, item_आकार, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            start=वामतः;
        } अन्यथा {
            /* sort [left..limit[ */
            यदि(वामतः<(सीमा-1)) {
                sub_शीघ्र_क्रमय(array, वामतः, सीमा, item_आकार, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            सीमा=दक्षिणतः;
        }
    } यावत्(start<(सीमा-1));
}

स्थिर शून्य
शीघ्र_क्रमय(वर्ण *array, int32_t length, int32_t item_आकार,
            UComparator *cmp, ध्रुव शून्य *context, UError_संहिता *p_Error_संहिता) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_राशिः_Array<std::max_align_t, आकार_अन्तर्_Max_Align_Ts(राशिः_ITEM_आकार) * 2> xw;
    यदि(आकार_अन्तर्_Max_Align_Ts(item_आकार)*2 > xw.get_क्षमता() &&
            xw.resize(आकार_अन्तर्_Max_Align_Ts(item_आकार) * 2) == शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    sub_शीघ्र_क्रमय(array, 0, length, item_आकार, cmp, context,
                 xw.getAlias(), xw.getAlias() + आकार_अन्तर्_Max_Align_Ts(item_आकार));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI शून्य U_EXPORT2
uprv_क्रमय_Array(शून्य *array, int32_t length, int32_t item_आकार,
               UComparator *cmp, ध्रुव शून्य *context,
               UBool क्रमय_Stable, UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि((length>0 && array==शून्यसूचकः) || length<0 || item_आकार<=0 || cmp==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि(length<=1) {
        प्रत्यागम;
    } अन्यथा यदि(length<MIN_QSORT || क्रमय_Stable) {
        insertion_क्रमय((वर्ण *)array, length, item_आकार, cmp, context, p_Error_संहिता);
    } अन्यथा {
        शीघ्र_क्रमय((वर्ण *)array, length, item_आकार, cmp, context, p_Error_संहिता);
    }
}
