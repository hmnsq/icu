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

#ಅನ್ತರ್ಭಾವ <cstddef>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"

ಎಣಿಕೆ {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ರಾಶಿ_ಅಂಶ_ಗಾತ್ರ=200
};

ಸ್ಥಿರ constexpr int32_t ಗಾತ್ರ_ಒಳ_Max_Align_Ts(int32_t ಗಾತ್ರ_ಒಳ_ಬೈಟುಗಳು) {
    ಹಿಂತಿರುಗಿ (ಗಾತ್ರ_ಒಳ_ಬೈಟುಗಳು + ಗಾತ್ರ(std::max_align_t) - 1) / ಗಾತ್ರ(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    ಹಿಂತಿರುಗಿ (int32_t)*(ಸ್ಥಿರಾಂಕ uint16_t *)ಎಡ - (int32_t)*(ಸ್ಥಿರಾಂಕ uint16_t *)ಬಲ;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    ಹಿಂತಿರುಗಿ *(ಸ್ಥಿರಾಂಕ int32_t *)ಎಡ - *(ಸ್ಥಿರಾಂಕ int32_t *)ಬಲ;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    (ಶೂನ್ಯ)context;
    uint32_t l=*(ಸ್ಥಿರಾಂಕ uint32_t *)ಎಡ, r=*(ಸ್ಥಿರಾಂಕ uint32_t *)ಬಲ;

    /* compare directly because (l-r) would overflow the int32_t result */
    ಆದರೆ(l<r) {
        ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(l==r) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ /* l>r */ {
        ಹಿಂತಿರುಗಿ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ದ್ವಿಮಾನ_ಹುಡುಕು(ಅಕ್ಷರ *array, int32_t ಎಲ್ಲೆ, ಶೂನ್ಯ *ಅಂಶ, int32_t ಅಂಶ_ಗಾತ್ರ,
                        UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    int32_t ಆರಂಭ=0;
    UBool ಸಿಕ್ಕಿತು=ಸುಳ್ಳು;

    /* Binary search until we get down to a tiny sub-array. */
    ಆಗ((ಎಲ್ಲೆ-ಆರಂಭ)>=MIN_QSORT) {
        int32_t i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        int32_t diff=cmp(context, ಅಂಶ, array+i*ಅಂಶ_ಗಾತ್ರ);
        ಆದರೆ(diff==0) {
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
            ಸಿಕ್ಕಿತು=ನಿಜ;
            ಆರಂಭ=i+1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff<0) {
            ಎಲ್ಲೆ=i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        int32_t diff=cmp(context, ಅಂಶ, array+ಆರಂಭ*ಅಂಶ_ಗಾತ್ರ);
        ಆದರೆ(diff==0) {
            ಸಿಕ್ಕಿತು=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff<0) {
            ನಿಲ್ಲಿಸು;
        }
        ++ಆರಂಭ;
    }
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು ? (ಆರಂಭ-1) : ~ಆರಂಭ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
do_Insertion_ವಿಂಗಡಿಸು(ಅಕ್ಷರ *array, int32_t length, int32_t ಅಂಶ_ಗಾತ್ರ,
                UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಶೂನ್ಯ *pv) {
    int32_t j;

    ಗಾಗಿ(j=1; j<length; ++j) {
        ಅಕ್ಷರ *ಅಂಶ=array+j*ಅಂಶ_ಗಾತ್ರ;
        int32_t insertion_ಬಿಂದು=uprv_stable_ದ್ವಿಮಾನ_ಹುಡುಕು(array, j, ಅಂಶ, ಅಂಶ_ಗಾತ್ರ, cmp, context);
        ಆದರೆ(insertion_ಬಿಂದು<0) {
            insertion_ಬಿಂದು=~insertion_ಬಿಂದು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++insertion_ಬಿಂದು;  /* one past the last equal item */
        }
        ಆದರೆ(insertion_ಬಿಂದು<j) {
            ಅಕ್ಷರ *dest=array+insertion_ಬಿಂದು*ಅಂಶ_ಗಾತ್ರ;
            uprv_memcpy(pv, ಅಂಶ, ಅಂಶ_ಗಾತ್ರ);  /* v=array[j] */
            uprv_memmove(dest+ಅಂಶ_ಗಾತ್ರ, dest, (j-insertion_ಬಿಂದು)*(size_t)ಅಂಶ_ಗಾತ್ರ);
            uprv_memcpy(dest, pv, ಅಂಶ_ಗಾತ್ರ);  /* array[insertionPoint]=v */
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ
insertion_ವಿಂಗಡಿಸು(ಅಕ್ಷರ *array, int32_t length, int32_t ಅಂಶ_ಗಾತ್ರ,
              UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {

    icu::Maybe_ರಾಶಿ_Array<std::max_align_t, ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ರಾಶಿ_ಅಂಶ_ಗಾತ್ರ)> v;
    ಆದರೆ (ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ಅಂಶ_ಗಾತ್ರ) > v.get_ಸಾಮರ್ಥ್ಯ() &&
            v.resize(ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ಅಂಶ_ಗಾತ್ರ)) == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    do_Insertion_ವಿಂಗಡಿಸು(array, length, ಅಂಶ_ಗಾತ್ರ, cmp, context, v.getAlias());
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
ಸ್ಥಿರ ಶೂನ್ಯ
sub_ಶೀಘ್ರ_ವಿಂಗಡಿಸು(ಅಕ್ಷರ *array, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t ಅಂಶ_ಗಾತ್ರ,
             UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
             ಶೂನ್ಯ *px, ಶೂನ್ಯ *pw) {
    int32_t ಎಡ, ಬಲ;

    /* start and left are inclusive, limit and right are exclusive */
    ಮಾಡು {
        ಆದರೆ((ಆರಂಭ+MIN_QSORT)>=ಎಲ್ಲೆ) {
            do_Insertion_ವಿಂಗಡಿಸು(array+ಆರಂಭ*ಅಂಶ_ಗಾತ್ರ, ಎಲ್ಲೆ-ಆರಂಭ, ಅಂಶ_ಗಾತ್ರ, cmp, context, px);
            ನಿಲ್ಲಿಸು;
        }

        ಎಡ=ಆರಂಭ;
        ಬಲ=ಎಲ್ಲೆ;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((ಆರಂಭ+ಎಲ್ಲೆ)/2)*ಅಂಶ_ಗಾತ್ರ, ಅಂಶ_ಗಾತ್ರ);

        ಮಾಡು {
            ಆಗ(/* array[left]<x */
                  cmp(context, array+ಎಡ*ಅಂಶ_ಗಾತ್ರ, px)<0
            ) {
                ++ಎಡ;
            }
            ಆಗ(/* x<array[right-1] */
                  cmp(context, px, array+(ಬಲ-1)*ಅಂಶ_ಗಾತ್ರ)<0
            ) {
                --ಬಲ;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ಆದರೆ(ಎಡ<ಬಲ) {
                --ಬಲ;

                ಆದರೆ(ಎಡ<ಬಲ) {
                    uprv_memcpy(pw, array+(size_t)ಎಡ*ಅಂಶ_ಗಾತ್ರ, ಅಂಶ_ಗಾತ್ರ);
                    uprv_memcpy(array+(size_t)ಎಡ*ಅಂಶ_ಗಾತ್ರ, array+(size_t)ಬಲ*ಅಂಶ_ಗಾತ್ರ, ಅಂಶ_ಗಾತ್ರ);
                    uprv_memcpy(array+(size_t)ಬಲ*ಅಂಶ_ಗಾತ್ರ, pw, ಅಂಶ_ಗಾತ್ರ);
                }

                ++ಎಡ;
            }
        } ಆಗ(ಎಡ<ಬಲ);

        /* sort sub-arrays */
        ಆದರೆ((ಬಲ-ಆರಂಭ)<(ಎಲ್ಲೆ-ಎಡ)) {
            /* sort [start..right[ */
            ಆದರೆ(ಆರಂಭ<(ಬಲ-1)) {
                sub_ಶೀಘ್ರ_ವಿಂಗಡಿಸು(array, ಆರಂಭ, ಬಲ, ಅಂಶ_ಗಾತ್ರ, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            ಆರಂಭ=ಎಡ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* sort [left..limit[ */
            ಆದರೆ(ಎಡ<(ಎಲ್ಲೆ-1)) {
                sub_ಶೀಘ್ರ_ವಿಂಗಡಿಸು(array, ಎಡ, ಎಲ್ಲೆ, ಅಂಶ_ಗಾತ್ರ, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            ಎಲ್ಲೆ=ಬಲ;
        }
    } ಆಗ(ಆರಂಭ<(ಎಲ್ಲೆ-1));
}

ಸ್ಥಿರ ಶೂನ್ಯ
ಶೀಘ್ರ_ವಿಂಗಡಿಸು(ಅಕ್ಷರ *array, int32_t length, int32_t ಅಂಶ_ಗಾತ್ರ,
            UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ರಾಶಿ_Array<std::max_align_t, ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ರಾಶಿ_ಅಂಶ_ಗಾತ್ರ) * 2> xw;
    ಆದರೆ(ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ಅಂಶ_ಗಾತ್ರ)*2 > xw.get_ಸಾಮರ್ಥ್ಯ() &&
            xw.resize(ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ಅಂಶ_ಗಾತ್ರ) * 2) == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    sub_ಶೀಘ್ರ_ವಿಂಗಡಿಸು(array, 0, length, ಅಂಶ_ಗಾತ್ರ, cmp, context,
                 xw.getAlias(), xw.getAlias() + ಗಾತ್ರ_ಒಳ_Max_Align_Ts(ಅಂಶ_ಗಾತ್ರ));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_ವಿಂಗಡಿಸು_Array(ಶೂನ್ಯ *array, int32_t length, int32_t ಅಂಶ_ಗಾತ್ರ,
               UComparator *cmp, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
               UBool ವಿಂಗಡಿಸು_Stable, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ((length>0 && array==ಶೂನ್ಯಸೂಚಕ) || length<0 || ಅಂಶ_ಗಾತ್ರ<=0 || cmp==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(length<=1) {
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length<MIN_QSORT || ವಿಂಗಡಿಸು_Stable) {
        insertion_ವಿಂಗಡಿಸು((ಅಕ್ಷರ *)array, length, ಅಂಶ_ಗಾತ್ರ, cmp, context, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಶೀಘ್ರ_ವಿಂಗಡಿಸು((ಅಕ್ಷರ *)array, length, ಅಂಶ_ಗಾತ್ರ, cmp, context, p_Error_ಸಂಕೇತ);
    }
}
