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
    ڍير_شي_سائيز=200
};

ساكن constexpr int32_t سائيز_اندريون_Max_Align_Ts(int32_t سائيز_اندريون_بائيٽون) {
    أرجع (سائيز_اندريون_بائيٽون + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    (فراغ)context;
    أرجع (int32_t)*(ثابت uint16_t *)کاٻي - (int32_t)*(ثابت uint16_t *)ساڄي;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    (فراغ)context;
    أرجع *(ثابت int32_t *)کاٻي - *(ثابت int32_t *)ساڄي;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)کاٻي, r=*(ثابت uint32_t *)ساڄي;

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
uprv_stable_ٻٽو_ڳولا(حرف *array, int32_t حد_بندي, فراغ *شي, int32_t شي_سائيز,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t شروعات=0;
    UBool مليو=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    طالما((حد_بندي-شروعات)>=MIN_QSORT) {
        int32_t i=(شروعات+حد_بندي)/2;
        int32_t diff=cmp(context, شي, array+i*شي_سائيز);
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
            مليو=سچ;
            شروعات=i+1;
        } وإلا إذا(diff<0) {
            حد_بندي=i;
        } وإلا {
            شروعات=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    طالما(شروعات<حد_بندي) {
        int32_t diff=cmp(context, شي, array+شروعات*شي_سائيز);
        إذا(diff==0) {
            مليو=سچ;
        } وإلا إذا(diff<0) {
            اكسر;
        }
        ++شروعات;
    }
    أرجع مليو ? (شروعات-1) : ~شروعات;
}

ساكن فراغ
do_Insertion_ترتيب_ڏيو(حرف *array, int32_t length, int32_t شي_سائيز,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *شي=array+j*شي_سائيز;
        int32_t insertion_نقطو=uprv_stable_ٻٽو_ڳولا(array, j, شي, شي_سائيز, cmp, context);
        إذا(insertion_نقطو<0) {
            insertion_نقطو=~insertion_نقطو;
        } وإلا {
            ++insertion_نقطو;  /* one past the last equal item */
        }
        إذا(insertion_نقطو<j) {
            حرف *dest=array+insertion_نقطو*شي_سائيز;
            uprv_memcpy(pv, شي, شي_سائيز);  /* v=array[j] */
            uprv_memmove(dest+شي_سائيز, dest, (j-insertion_نقطو)*(size_t)شي_سائيز);
            uprv_memcpy(dest, pv, شي_سائيز);  /* array[insertionPoint]=v */
        }
    }
}

ساكن فراغ
insertion_ترتيب_ڏيو(حرف *array, int32_t length, int32_t شي_سائيز,
              UComparator *cmp, ثابت فراغ *context, UError_ڪوڊ *p_Error_ڪوڊ) {

    icu::Maybe_ڍير_Array<std::max_align_t, سائيز_اندريون_Max_Align_Ts(ڍير_شي_سائيز)> v;
    إذا (سائيز_اندريون_Max_Align_Ts(شي_سائيز) > v.get_صلاحيت() &&
            v.resize(سائيز_اندريون_Max_Align_Ts(شي_سائيز)) == خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    do_Insertion_ترتيب_ڏيو(array, length, شي_سائيز, cmp, context, v.getAlias());
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
sub_تيز_ترتيب_ڏيو(حرف *array, int32_t شروعات, int32_t حد_بندي, int32_t شي_سائيز,
             UComparator *cmp, ثابت فراغ *context,
             فراغ *px, فراغ *pw) {
    int32_t کاٻي, ساڄي;

    /* start and left are inclusive, limit and right are exclusive */
    افعل {
        إذا((شروعات+MIN_QSORT)>=حد_بندي) {
            do_Insertion_ترتيب_ڏيو(array+شروعات*شي_سائيز, حد_بندي-شروعات, شي_سائيز, cmp, context, px);
            اكسر;
        }

        کاٻي=شروعات;
        ساڄي=حد_بندي;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((شروعات+حد_بندي)/2)*شي_سائيز, شي_سائيز);

        افعل {
            طالما(/* array[left]<x */
                  cmp(context, array+کاٻي*شي_سائيز, px)<0
            ) {
                ++کاٻي;
            }
            طالما(/* x<array[right-1] */
                  cmp(context, px, array+(ساڄي-1)*شي_سائيز)<0
            ) {
                --ساڄي;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            إذا(کاٻي<ساڄي) {
                --ساڄي;

                إذا(کاٻي<ساڄي) {
                    uprv_memcpy(pw, array+(size_t)کاٻي*شي_سائيز, شي_سائيز);
                    uprv_memcpy(array+(size_t)کاٻي*شي_سائيز, array+(size_t)ساڄي*شي_سائيز, شي_سائيز);
                    uprv_memcpy(array+(size_t)ساڄي*شي_سائيز, pw, شي_سائيز);
                }

                ++کاٻي;
            }
        } طالما(کاٻي<ساڄي);

        /* sort sub-arrays */
        إذا((ساڄي-شروعات)<(حد_بندي-کاٻي)) {
            /* sort [start..right[ */
            إذا(شروعات<(ساڄي-1)) {
                sub_تيز_ترتيب_ڏيو(array, شروعات, ساڄي, شي_سائيز, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            شروعات=کاٻي;
        } وإلا {
            /* sort [left..limit[ */
            إذا(کاٻي<(حد_بندي-1)) {
                sub_تيز_ترتيب_ڏيو(array, کاٻي, حد_بندي, شي_سائيز, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            حد_بندي=ساڄي;
        }
    } طالما(شروعات<(حد_بندي-1));
}

ساكن فراغ
تيز_ترتيب_ڏيو(حرف *array, int32_t length, int32_t شي_سائيز,
            UComparator *cmp, ثابت فراغ *context, UError_ڪوڊ *p_Error_ڪوڊ) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ڍير_Array<std::max_align_t, سائيز_اندريون_Max_Align_Ts(ڍير_شي_سائيز) * 2> xw;
    إذا(سائيز_اندريون_Max_Align_Ts(شي_سائيز)*2 > xw.get_صلاحيت() &&
            xw.resize(سائيز_اندريون_Max_Align_Ts(شي_سائيز) * 2) == خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    sub_تيز_ترتيب_ڏيو(array, 0, length, شي_سائيز, cmp, context,
                 xw.getAlias(), xw.getAlias() + سائيز_اندريون_Max_Align_Ts(شي_سائيز));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI فراغ U_EXPORT2
uprv_ترتيب_ڏيو_Array(فراغ *array, int32_t length, int32_t شي_سائيز,
               UComparator *cmp, ثابت فراغ *context,
               UBool ترتيب_ڏيو_Stable, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا((length>0 && array==خالي_اشارو) || length<0 || شي_سائيز<=0 || cmp==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    إذا(length<=1) {
        أرجع;
    } وإلا إذا(length<MIN_QSORT || ترتيب_ڏيو_Stable) {
        insertion_ترتيب_ڏيو((حرف *)array, length, شي_سائيز, cmp, context, p_Error_ڪوڊ);
    } وإلا {
        تيز_ترتيب_ڏيو((حرف *)array, length, شي_سائيز, cmp, context, p_Error_ڪوڊ);
    }
}
