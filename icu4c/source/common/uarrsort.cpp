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

#ضمن <cstddef>

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"

تعداد {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ڈھیر_شے_ناپ=200
};

ساكن constexpr int32_t ناپ_وچ_Max_Align_Ts(int32_t ناپ_وچ_بائٹاں) {
    أرجع (ناپ_وچ_بائٹاں + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    (فراغ)context;
    أرجع (int32_t)*(ثابت uint16_t *)کھبے - (int32_t)*(ثابت uint16_t *)سجے;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    (فراغ)context;
    أرجع *(ثابت int32_t *)کھبے - *(ثابت int32_t *)سجے;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)کھبے, r=*(ثابت uint32_t *)سجے;

    /* compare directly because (l-r) would overflow the int32_t result */
    إذا(l<r) {
        أرجع -1;
    } وإلا إذا(l==r) {
        أرجع 0;
    } وإلا /* l>r */ {
        أرجع 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ثنائی_تلاش(حرف *array, int32_t حد_بندی, فراغ *شے, int32_t شے_ناپ,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t شروع=0;
    UBool لبھیا=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    طالما((حد_بندی-شروع)>=MIN_QSORT) {
        int32_t i=(شروع+حد_بندی)/2;
        int32_t diff=cmp(context, شے, array+i*شے_ناپ);
        إذا(diff==0) {
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
            لبھیا=سچ;
            شروع=i+1;
        } وإلا إذا(diff<0) {
            حد_بندی=i;
        } وإلا {
            شروع=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    طالما(شروع<حد_بندی) {
        int32_t diff=cmp(context, شے, array+شروع*شے_ناپ);
        إذا(diff==0) {
            لبھیا=سچ;
        } وإلا إذا(diff<0) {
            اكسر;
        }
        ++شروع;
    }
    أرجع لبھیا ? (شروع-1) : ~شروع;
}

ساكن فراغ
do_Insertion_ترتیب_دیو(حرف *array, int32_t length, int32_t شے_ناپ,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *شے=array+j*شے_ناپ;
        int32_t insertion_نقطہ=uprv_stable_ثنائی_تلاش(array, j, شے, شے_ناپ, cmp, context);
        إذا(insertion_نقطہ<0) {
            insertion_نقطہ=~insertion_نقطہ;
        } وإلا {
            ++insertion_نقطہ;  /* one past the last equal item */
        }
        إذا(insertion_نقطہ<j) {
            حرف *dest=array+insertion_نقطہ*شے_ناپ;
            uprv_memcpy(pv, شے, شے_ناپ);  /* v=array[j] */
            uprv_memmove(dest+شے_ناپ, dest, (j-insertion_نقطہ)*(size_t)شے_ناپ);
            uprv_memcpy(dest, pv, شے_ناپ);  /* array[insertionPoint]=v */
        }
    }
}

ساكن فراغ
insertion_ترتیب_دیو(حرف *array, int32_t length, int32_t شے_ناپ,
              UComparator *cmp, ثابت فراغ *context, UError_کوڈ *p_Error_کوڈ) {

    icu::Maybe_ڈھیر_Array<std::max_align_t, ناپ_وچ_Max_Align_Ts(ڈھیر_شے_ناپ)> v;
    إذا (ناپ_وچ_Max_Align_Ts(شے_ناپ) > v.get_سمرتھا() &&
            v.resize(ناپ_وچ_Max_Align_Ts(شے_ناپ)) == خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    do_Insertion_ترتیب_دیو(array, length, شے_ناپ, cmp, context, v.getAlias());
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
ساكن فراغ
sub_تیز_ترتیب_دیو(حرف *array, int32_t شروع, int32_t حد_بندی, int32_t شے_ناپ,
             UComparator *cmp, ثابت فراغ *context,
             فراغ *px, فراغ *pw) {
    int32_t کھبے, سجے;

    /* start and left are inclusive, limit and right are exclusive */
    افعل {
        إذا((شروع+MIN_QSORT)>=حد_بندی) {
            do_Insertion_ترتیب_دیو(array+شروع*شے_ناپ, حد_بندی-شروع, شے_ناپ, cmp, context, px);
            اكسر;
        }

        کھبے=شروع;
        سجے=حد_بندی;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((شروع+حد_بندی)/2)*شے_ناپ, شے_ناپ);

        افعل {
            طالما(/* array[left]<x */
                  cmp(context, array+کھبے*شے_ناپ, px)<0
            ) {
                ++کھبے;
            }
            طالما(/* x<array[right-1] */
                  cmp(context, px, array+(سجے-1)*شے_ناپ)<0
            ) {
                --سجے;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            إذا(کھبے<سجے) {
                --سجے;

                إذا(کھبے<سجے) {
                    uprv_memcpy(pw, array+(size_t)کھبے*شے_ناپ, شے_ناپ);
                    uprv_memcpy(array+(size_t)کھبے*شے_ناپ, array+(size_t)سجے*شے_ناپ, شے_ناپ);
                    uprv_memcpy(array+(size_t)سجے*شے_ناپ, pw, شے_ناپ);
                }

                ++کھبے;
            }
        } طالما(کھبے<سجے);

        /* sort sub-arrays */
        إذا((سجے-شروع)<(حد_بندی-کھبے)) {
            /* sort [start..right[ */
            إذا(شروع<(سجے-1)) {
                sub_تیز_ترتیب_دیو(array, شروع, سجے, شے_ناپ, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            شروع=کھبے;
        } وإلا {
            /* sort [left..limit[ */
            إذا(کھبے<(حد_بندی-1)) {
                sub_تیز_ترتیب_دیو(array, کھبے, حد_بندی, شے_ناپ, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            حد_بندی=سجے;
        }
    } طالما(شروع<(حد_بندی-1));
}

ساكن فراغ
تیز_ترتیب_دیو(حرف *array, int32_t length, int32_t شے_ناپ,
            UComparator *cmp, ثابت فراغ *context, UError_کوڈ *p_Error_کوڈ) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ڈھیر_Array<std::max_align_t, ناپ_وچ_Max_Align_Ts(ڈھیر_شے_ناپ) * 2> xw;
    إذا(ناپ_وچ_Max_Align_Ts(شے_ناپ)*2 > xw.get_سمرتھا() &&
            xw.resize(ناپ_وچ_Max_Align_Ts(شے_ناپ) * 2) == خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    sub_تیز_ترتیب_دیو(array, 0, length, شے_ناپ, cmp, context,
                 xw.getAlias(), xw.getAlias() + ناپ_وچ_Max_Align_Ts(شے_ناپ));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI فراغ U_EXPORT2
uprv_ترتیب_دیو_Array(فراغ *array, int32_t length, int32_t شے_ناپ,
               UComparator *cmp, ثابت فراغ *context,
               UBool ترتیب_دیو_Stable, UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا((length>0 && array==خالی_سوچک) || length<0 || شے_ناپ<=0 || cmp==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(length<=1) {
        أرجع;
    } وإلا إذا(length<MIN_QSORT || ترتیب_دیو_Stable) {
        insertion_ترتیب_دیو((حرف *)array, length, شے_ناپ, cmp, context, p_Error_کوڈ);
    } وإلا {
        تیز_ترتیب_دیو((حرف *)array, length, شے_ناپ, cmp, context, p_Error_کوڈ);
    }
}
