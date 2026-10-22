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

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"

ਗਿਣਤੀ {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ਢੇਰ_ਵਸਤੂ_ਮਾਪ=200
};

ਸਥਿਰ constexpr int32_t ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(int32_t ਮਾਪ_ਵਿੱਚ_ਬਾਈਟਾਂ) {
    ਵਾਪਸ (ਮਾਪ_ਵਿੱਚ_ਬਾਈਟਾਂ + ਆਕਾਰ(std::max_align_t) - 1) / ਆਕਾਰ(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    (ਖਾਲੀ)context;
    ਵਾਪਸ (int32_t)*(ਅਚਲ uint16_t *)ਖੱਬੇ - (int32_t)*(ਅਚਲ uint16_t *)ਸੱਜੇ;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    (ਖਾਲੀ)context;
    ਵਾਪਸ *(ਅਚਲ int32_t *)ਖੱਬੇ - *(ਅਚਲ int32_t *)ਸੱਜੇ;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    (ਖਾਲੀ)context;
    uint32_t l=*(ਅਚਲ uint32_t *)ਖੱਬੇ, r=*(ਅਚਲ uint32_t *)ਸੱਜੇ;

    /* compare directly because (l-r) would overflow the int32_t result */
    ਜੇ(l<r) {
        ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ ਜੇ(l==r) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ /* l>r */ {
        ਵਾਪਸ 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ਦੁਆਧਾਰੀ_ਖੋਜ(ਅੱਖਰ *array, int32_t ਹੱਦਬੰਦੀ, ਖਾਲੀ *ਵਸਤੂ, int32_t ਵਸਤੂ_ਮਾਪ,
                        UComparator *cmp, ਅਚਲ ਖਾਲੀ *context) {
    int32_t ਸ਼ੁਰੂ=0;
    UBool ਮਿਲਿਆ=ਝੂਠ;

    /* Binary search until we get down to a tiny sub-array. */
    ਜਦੋਂ((ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ)>=MIN_QSORT) {
        int32_t i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        int32_t diff=cmp(context, ਵਸਤੂ, array+i*ਵਸਤੂ_ਮਾਪ);
        ਜੇ(diff==0) {
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
            ਮਿਲਿਆ=ਸੱਚ;
            ਸ਼ੁਰੂ=i+1;
        } ਨਹੀਂਤਾਂ ਜੇ(diff<0) {
            ਹੱਦਬੰਦੀ=i;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        int32_t diff=cmp(context, ਵਸਤੂ, array+ਸ਼ੁਰੂ*ਵਸਤੂ_ਮਾਪ);
        ਜੇ(diff==0) {
            ਮਿਲਿਆ=ਸੱਚ;
        } ਨਹੀਂਤਾਂ ਜੇ(diff<0) {
            ਰੋਕੋ;
        }
        ++ਸ਼ੁਰੂ;
    }
    ਵਾਪਸ ਮਿਲਿਆ ? (ਸ਼ੁਰੂ-1) : ~ਸ਼ੁਰੂ;
}

ਸਥਿਰ ਖਾਲੀ
do_Insertion_ਛਾਂਟੋ(ਅੱਖਰ *array, int32_t length, int32_t ਵਸਤੂ_ਮਾਪ,
                UComparator *cmp, ਅਚਲ ਖਾਲੀ *context, ਖਾਲੀ *pv) {
    int32_t j;

    ਲਈ(j=1; j<length; ++j) {
        ਅੱਖਰ *ਵਸਤੂ=array+j*ਵਸਤੂ_ਮਾਪ;
        int32_t insertion_ਬਿੰਦੂ=uprv_stable_ਦੁਆਧਾਰੀ_ਖੋਜ(array, j, ਵਸਤੂ, ਵਸਤੂ_ਮਾਪ, cmp, context);
        ਜੇ(insertion_ਬਿੰਦੂ<0) {
            insertion_ਬਿੰਦੂ=~insertion_ਬਿੰਦੂ;
        } ਨਹੀਂਤਾਂ {
            ++insertion_ਬਿੰਦੂ;  /* one past the last equal item */
        }
        ਜੇ(insertion_ਬਿੰਦੂ<j) {
            ਅੱਖਰ *dest=array+insertion_ਬਿੰਦੂ*ਵਸਤੂ_ਮਾਪ;
            uprv_memcpy(pv, ਵਸਤੂ, ਵਸਤੂ_ਮਾਪ);  /* v=array[j] */
            uprv_memmove(dest+ਵਸਤੂ_ਮਾਪ, dest, (j-insertion_ਬਿੰਦੂ)*(size_t)ਵਸਤੂ_ਮਾਪ);
            uprv_memcpy(dest, pv, ਵਸਤੂ_ਮਾਪ);  /* array[insertionPoint]=v */
        }
    }
}

ਸਥਿਰ ਖਾਲੀ
insertion_ਛਾਂਟੋ(ਅੱਖਰ *array, int32_t length, int32_t ਵਸਤੂ_ਮਾਪ,
              UComparator *cmp, ਅਚਲ ਖਾਲੀ *context, UError_ਕੋਡ *p_Error_ਕੋਡ) {

    icu::Maybe_ਢੇਰ_Array<std::max_align_t, ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਢੇਰ_ਵਸਤੂ_ਮਾਪ)> v;
    ਜੇ (ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਵਸਤੂ_ਮਾਪ) > v.get_ਸਮਰੱਥਾ() &&
            v.resize(ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਵਸਤੂ_ਮਾਪ)) == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    do_Insertion_ਛਾਂਟੋ(array, length, ਵਸਤੂ_ਮਾਪ, cmp, context, v.getAlias());
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
ਸਥਿਰ ਖਾਲੀ
sub_ਤੇਜ਼_ਛਾਂਟੋ(ਅੱਖਰ *array, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t ਵਸਤੂ_ਮਾਪ,
             UComparator *cmp, ਅਚਲ ਖਾਲੀ *context,
             ਖਾਲੀ *px, ਖਾਲੀ *pw) {
    int32_t ਖੱਬੇ, ਸੱਜੇ;

    /* start and left are inclusive, limit and right are exclusive */
    ਕਰੋ {
        ਜੇ((ਸ਼ੁਰੂ+MIN_QSORT)>=ਹੱਦਬੰਦੀ) {
            do_Insertion_ਛਾਂਟੋ(array+ਸ਼ੁਰੂ*ਵਸਤੂ_ਮਾਪ, ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ, ਵਸਤੂ_ਮਾਪ, cmp, context, px);
            ਰੋਕੋ;
        }

        ਖੱਬੇ=ਸ਼ੁਰੂ;
        ਸੱਜੇ=ਹੱਦਬੰਦੀ;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2)*ਵਸਤੂ_ਮਾਪ, ਵਸਤੂ_ਮਾਪ);

        ਕਰੋ {
            ਜਦੋਂ(/* array[left]<x */
                  cmp(context, array+ਖੱਬੇ*ਵਸਤੂ_ਮਾਪ, px)<0
            ) {
                ++ਖੱਬੇ;
            }
            ਜਦੋਂ(/* x<array[right-1] */
                  cmp(context, px, array+(ਸੱਜੇ-1)*ਵਸਤੂ_ਮਾਪ)<0
            ) {
                --ਸੱਜੇ;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ਜੇ(ਖੱਬੇ<ਸੱਜੇ) {
                --ਸੱਜੇ;

                ਜੇ(ਖੱਬੇ<ਸੱਜੇ) {
                    uprv_memcpy(pw, array+(size_t)ਖੱਬੇ*ਵਸਤੂ_ਮਾਪ, ਵਸਤੂ_ਮਾਪ);
                    uprv_memcpy(array+(size_t)ਖੱਬੇ*ਵਸਤੂ_ਮਾਪ, array+(size_t)ਸੱਜੇ*ਵਸਤੂ_ਮਾਪ, ਵਸਤੂ_ਮਾਪ);
                    uprv_memcpy(array+(size_t)ਸੱਜੇ*ਵਸਤੂ_ਮਾਪ, pw, ਵਸਤੂ_ਮਾਪ);
                }

                ++ਖੱਬੇ;
            }
        } ਜਦੋਂ(ਖੱਬੇ<ਸੱਜੇ);

        /* sort sub-arrays */
        ਜੇ((ਸੱਜੇ-ਸ਼ੁਰੂ)<(ਹੱਦਬੰਦੀ-ਖੱਬੇ)) {
            /* sort [start..right[ */
            ਜੇ(ਸ਼ੁਰੂ<(ਸੱਜੇ-1)) {
                sub_ਤੇਜ਼_ਛਾਂਟੋ(array, ਸ਼ੁਰੂ, ਸੱਜੇ, ਵਸਤੂ_ਮਾਪ, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            ਸ਼ੁਰੂ=ਖੱਬੇ;
        } ਨਹੀਂਤਾਂ {
            /* sort [left..limit[ */
            ਜੇ(ਖੱਬੇ<(ਹੱਦਬੰਦੀ-1)) {
                sub_ਤੇਜ਼_ਛਾਂਟੋ(array, ਖੱਬੇ, ਹੱਦਬੰਦੀ, ਵਸਤੂ_ਮਾਪ, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            ਹੱਦਬੰਦੀ=ਸੱਜੇ;
        }
    } ਜਦੋਂ(ਸ਼ੁਰੂ<(ਹੱਦਬੰਦੀ-1));
}

ਸਥਿਰ ਖਾਲੀ
ਤੇਜ਼_ਛਾਂਟੋ(ਅੱਖਰ *array, int32_t length, int32_t ਵਸਤੂ_ਮਾਪ,
            UComparator *cmp, ਅਚਲ ਖਾਲੀ *context, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ਢੇਰ_Array<std::max_align_t, ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਢੇਰ_ਵਸਤੂ_ਮਾਪ) * 2> xw;
    ਜੇ(ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਵਸਤੂ_ਮਾਪ)*2 > xw.get_ਸਮਰੱਥਾ() &&
            xw.resize(ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਵਸਤੂ_ਮਾਪ) * 2) == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    sub_ਤੇਜ਼_ਛਾਂਟੋ(array, 0, length, ਵਸਤੂ_ਮਾਪ, cmp, context,
                 xw.getAlias(), xw.getAlias() + ਮਾਪ_ਵਿੱਚ_Max_Align_Ts(ਵਸਤੂ_ਮਾਪ));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uprv_ਛਾਂਟੋ_Array(ਖਾਲੀ *array, int32_t length, int32_t ਵਸਤੂ_ਮਾਪ,
               UComparator *cmp, ਅਚਲ ਖਾਲੀ *context,
               UBool ਛਾਂਟੋ_Stable, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ((length>0 && array==ਖਾਲੀ_ਸੂਚਕ) || length<0 || ਵਸਤੂ_ਮਾਪ<=0 || cmp==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ(length<=1) {
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ ਜੇ(length<MIN_QSORT || ਛਾਂਟੋ_Stable) {
        insertion_ਛਾਂਟੋ((ਅੱਖਰ *)array, length, ਵਸਤੂ_ਮਾਪ, cmp, context, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਤੇਜ਼_ਛਾਂਟੋ((ਅੱਖਰ *)array, length, ਵਸਤੂ_ਮਾਪ, cmp, context, p_Error_ਕੋਡ);
    }
}
