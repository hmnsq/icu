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
    ډېری_توکی_اندازه=200
};

ساكن constexpr int32_t اندازه_دننه_Max_Align_Ts(int32_t اندازه_دننه_بایټونه) {
    أرجع (اندازه_دننه_بایټونه + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    (فراغ)context;
    أرجع (int32_t)*(ثابت uint16_t *)کیڼ - (int32_t)*(ثابت uint16_t *)ښی;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    (فراغ)context;
    أرجع *(ثابت int32_t *)کیڼ - *(ثابت int32_t *)ښی;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)کیڼ, r=*(ثابت uint32_t *)ښی;

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
uprv_stable_دوګونی_پلټنه(حرف *array, int32_t حد, فراغ *توکی, int32_t توکی_اندازه,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t پیل=0;
    UBool وموندل=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    طالما((حد-پیل)>=MIN_QSORT) {
        int32_t i=(پیل+حد)/2;
        int32_t diff=cmp(context, توکی, array+i*توکی_اندازه);
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
            وموندل=ریښتیا;
            پیل=i+1;
        } وإلا إذا(diff<0) {
            حد=i;
        } وإلا {
            پیل=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    طالما(پیل<حد) {
        int32_t diff=cmp(context, توکی, array+پیل*توکی_اندازه);
        إذا(diff==0) {
            وموندل=ریښتیا;
        } وإلا إذا(diff<0) {
            اكسر;
        }
        ++پیل;
    }
    أرجع وموندل ? (پیل-1) : ~پیل;
}

ساكن فراغ
do_Insertion_ترتیبول(حرف *array, int32_t length, int32_t توکی_اندازه,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *توکی=array+j*توکی_اندازه;
        int32_t insertion_ټکی=uprv_stable_دوګونی_پلټنه(array, j, توکی, توکی_اندازه, cmp, context);
        إذا(insertion_ټکی<0) {
            insertion_ټکی=~insertion_ټکی;
        } وإلا {
            ++insertion_ټکی;  /* one past the last equal item */
        }
        إذا(insertion_ټکی<j) {
            حرف *dest=array+insertion_ټکی*توکی_اندازه;
            uprv_memcpy(pv, توکی, توکی_اندازه);  /* v=array[j] */
            uprv_memmove(dest+توکی_اندازه, dest, (j-insertion_ټکی)*(size_t)توکی_اندازه);
            uprv_memcpy(dest, pv, توکی_اندازه);  /* array[insertionPoint]=v */
        }
    }
}

ساكن فراغ
insertion_ترتیبول(حرف *array, int32_t length, int32_t توکی_اندازه,
              UComparator *cmp, ثابت فراغ *context, UError_کوډ *p_Error_کوډ) {

    icu::Maybe_ډېری_Array<std::max_align_t, اندازه_دننه_Max_Align_Ts(ډېری_توکی_اندازه)> v;
    إذا (اندازه_دننه_Max_Align_Ts(توکی_اندازه) > v.get_ظرفیت() &&
            v.resize(اندازه_دننه_Max_Align_Ts(توکی_اندازه)) == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }

    do_Insertion_ترتیبول(array, length, توکی_اندازه, cmp, context, v.getAlias());
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
sub_چټک_ترتیبول(حرف *array, int32_t پیل, int32_t حد, int32_t توکی_اندازه,
             UComparator *cmp, ثابت فراغ *context,
             فراغ *px, فراغ *pw) {
    int32_t کیڼ, ښی;

    /* start and left are inclusive, limit and right are exclusive */
    افعل {
        إذا((پیل+MIN_QSORT)>=حد) {
            do_Insertion_ترتیبول(array+پیل*توکی_اندازه, حد-پیل, توکی_اندازه, cmp, context, px);
            اكسر;
        }

        کیڼ=پیل;
        ښی=حد;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((پیل+حد)/2)*توکی_اندازه, توکی_اندازه);

        افعل {
            طالما(/* array[left]<x */
                  cmp(context, array+کیڼ*توکی_اندازه, px)<0
            ) {
                ++کیڼ;
            }
            طالما(/* x<array[right-1] */
                  cmp(context, px, array+(ښی-1)*توکی_اندازه)<0
            ) {
                --ښی;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            إذا(کیڼ<ښی) {
                --ښی;

                إذا(کیڼ<ښی) {
                    uprv_memcpy(pw, array+(size_t)کیڼ*توکی_اندازه, توکی_اندازه);
                    uprv_memcpy(array+(size_t)کیڼ*توکی_اندازه, array+(size_t)ښی*توکی_اندازه, توکی_اندازه);
                    uprv_memcpy(array+(size_t)ښی*توکی_اندازه, pw, توکی_اندازه);
                }

                ++کیڼ;
            }
        } طالما(کیڼ<ښی);

        /* sort sub-arrays */
        إذا((ښی-پیل)<(حد-کیڼ)) {
            /* sort [start..right[ */
            إذا(پیل<(ښی-1)) {
                sub_چټک_ترتیبول(array, پیل, ښی, توکی_اندازه, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            پیل=کیڼ;
        } وإلا {
            /* sort [left..limit[ */
            إذا(کیڼ<(حد-1)) {
                sub_چټک_ترتیبول(array, کیڼ, حد, توکی_اندازه, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            حد=ښی;
        }
    } طالما(پیل<(حد-1));
}

ساكن فراغ
چټک_ترتیبول(حرف *array, int32_t length, int32_t توکی_اندازه,
            UComparator *cmp, ثابت فراغ *context, UError_کوډ *p_Error_کوډ) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ډېری_Array<std::max_align_t, اندازه_دننه_Max_Align_Ts(ډېری_توکی_اندازه) * 2> xw;
    إذا(اندازه_دننه_Max_Align_Ts(توکی_اندازه)*2 > xw.get_ظرفیت() &&
            xw.resize(اندازه_دننه_Max_Align_Ts(توکی_اندازه) * 2) == تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع;
    }

    sub_چټک_ترتیبول(array, 0, length, توکی_اندازه, cmp, context,
                 xw.getAlias(), xw.getAlias() + اندازه_دننه_Max_Align_Ts(توکی_اندازه));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI فراغ U_EXPORT2
uprv_ترتیبول_Array(فراغ *array, int32_t length, int32_t توکی_اندازه,
               UComparator *cmp, ثابت فراغ *context,
               UBool ترتیبول_Stable, UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا((length>0 && array==تش_اشاره) || length<0 || توکی_اندازه<=0 || cmp==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(length<=1) {
        أرجع;
    } وإلا إذا(length<MIN_QSORT || ترتیبول_Stable) {
        insertion_ترتیبول((حرف *)array, length, توکی_اندازه, cmp, context, p_Error_کوډ);
    } وإلا {
        چټک_ترتیبول((حرف *)array, length, توکی_اندازه, cmp, context, p_Error_کوډ);
    }
}
