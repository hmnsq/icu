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
    پشته_قلم_اندازه=200
};

ساكن constexpr int32_t اندازه_درون_Max_Align_Ts(int32_t اندازه_درون_بایتها) {
    أرجع (اندازه_درون_بایتها + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    (فراغ)context;
    أرجع (int32_t)*(ثابت uint16_t *)چپ - (int32_t)*(ثابت uint16_t *)راست;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    (فراغ)context;
    أرجع *(ثابت int32_t *)چپ - *(ثابت int32_t *)راست;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    (فراغ)context;
    uint32_t l=*(ثابت uint32_t *)چپ, r=*(ثابت uint32_t *)راست;

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
uprv_stable_دودویی_جستجو(حرف *array, int32_t حد, فراغ *قلم, int32_t قلم_اندازه,
                        UComparator *cmp, ثابت فراغ *context) {
    int32_t شروع=0;
    UBool یافت=خطأ;

    /* Binary search until we get down to a tiny sub-array. */
    طالما((حد-شروع)>=MIN_QSORT) {
        int32_t i=(شروع+حد)/2;
        int32_t diff=cmp(context, قلم, array+i*قلم_اندازه);
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
            یافت=درست;
            شروع=i+1;
        } وإلا إذا(diff<0) {
            حد=i;
        } وإلا {
            شروع=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    طالما(شروع<حد) {
        int32_t diff=cmp(context, قلم, array+شروع*قلم_اندازه);
        إذا(diff==0) {
            یافت=درست;
        } وإلا إذا(diff<0) {
            اكسر;
        }
        ++شروع;
    }
    أرجع یافت ? (شروع-1) : ~شروع;
}

ساكن فراغ
do_Insertion_مرتب_سازی(حرف *array, int32_t length, int32_t قلم_اندازه,
                UComparator *cmp, ثابت فراغ *context, فراغ *pv) {
    int32_t j;

    لكل(j=1; j<length; ++j) {
        حرف *قلم=array+j*قلم_اندازه;
        int32_t insertion_نقطه=uprv_stable_دودویی_جستجو(array, j, قلم, قلم_اندازه, cmp, context);
        إذا(insertion_نقطه<0) {
            insertion_نقطه=~insertion_نقطه;
        } وإلا {
            ++insertion_نقطه;  /* one past the last equal item */
        }
        إذا(insertion_نقطه<j) {
            حرف *dest=array+insertion_نقطه*قلم_اندازه;
            uprv_memcpy(pv, قلم, قلم_اندازه);  /* v=array[j] */
            uprv_memmove(dest+قلم_اندازه, dest, (j-insertion_نقطه)*(size_t)قلم_اندازه);
            uprv_memcpy(dest, pv, قلم_اندازه);  /* array[insertionPoint]=v */
        }
    }
}

ساكن فراغ
insertion_مرتب_سازی(حرف *array, int32_t length, int32_t قلم_اندازه,
              UComparator *cmp, ثابت فراغ *context, UError_کد *p_Error_کد) {

    icu::Maybe_پشته_Array<std::max_align_t, اندازه_درون_Max_Align_Ts(پشته_قلم_اندازه)> v;
    إذا (اندازه_درون_Max_Align_Ts(قلم_اندازه) > v.get_ظرفیت() &&
            v.resize(اندازه_درون_Max_Align_Ts(قلم_اندازه)) == اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    do_Insertion_مرتب_سازی(array, length, قلم_اندازه, cmp, context, v.getAlias());
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
sub_سریع_مرتب_سازی(حرف *array, int32_t شروع, int32_t حد, int32_t قلم_اندازه,
             UComparator *cmp, ثابت فراغ *context,
             فراغ *px, فراغ *pw) {
    int32_t چپ, راست;

    /* start and left are inclusive, limit and right are exclusive */
    افعل {
        إذا((شروع+MIN_QSORT)>=حد) {
            do_Insertion_مرتب_سازی(array+شروع*قلم_اندازه, حد-شروع, قلم_اندازه, cmp, context, px);
            اكسر;
        }

        چپ=شروع;
        راست=حد;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((شروع+حد)/2)*قلم_اندازه, قلم_اندازه);

        افعل {
            طالما(/* array[left]<x */
                  cmp(context, array+چپ*قلم_اندازه, px)<0
            ) {
                ++چپ;
            }
            طالما(/* x<array[right-1] */
                  cmp(context, px, array+(راست-1)*قلم_اندازه)<0
            ) {
                --راست;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            إذا(چپ<راست) {
                --راست;

                إذا(چپ<راست) {
                    uprv_memcpy(pw, array+(size_t)چپ*قلم_اندازه, قلم_اندازه);
                    uprv_memcpy(array+(size_t)چپ*قلم_اندازه, array+(size_t)راست*قلم_اندازه, قلم_اندازه);
                    uprv_memcpy(array+(size_t)راست*قلم_اندازه, pw, قلم_اندازه);
                }

                ++چپ;
            }
        } طالما(چپ<راست);

        /* sort sub-arrays */
        إذا((راست-شروع)<(حد-چپ)) {
            /* sort [start..right[ */
            إذا(شروع<(راست-1)) {
                sub_سریع_مرتب_سازی(array, شروع, راست, قلم_اندازه, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            شروع=چپ;
        } وإلا {
            /* sort [left..limit[ */
            إذا(چپ<(حد-1)) {
                sub_سریع_مرتب_سازی(array, چپ, حد, قلم_اندازه, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            حد=راست;
        }
    } طالما(شروع<(حد-1));
}

ساكن فراغ
سریع_مرتب_سازی(حرف *array, int32_t length, int32_t قلم_اندازه,
            UComparator *cmp, ثابت فراغ *context, UError_کد *p_Error_کد) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_پشته_Array<std::max_align_t, اندازه_درون_Max_Align_Ts(پشته_قلم_اندازه) * 2> xw;
    إذا(اندازه_درون_Max_Align_Ts(قلم_اندازه)*2 > xw.get_ظرفیت() &&
            xw.resize(اندازه_درون_Max_Align_Ts(قلم_اندازه) * 2) == اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    sub_سریع_مرتب_سازی(array, 0, length, قلم_اندازه, cmp, context,
                 xw.getAlias(), xw.getAlias() + اندازه_درون_Max_Align_Ts(قلم_اندازه));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI فراغ U_EXPORT2
uprv_مرتب_سازی_Array(فراغ *array, int32_t length, int32_t قلم_اندازه,
               UComparator *cmp, ثابت فراغ *context,
               UBool مرتب_سازی_Stable, UError_کد *p_Error_کد) {
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا((length>0 && array==اشاره_تهی) || length<0 || قلم_اندازه<=0 || cmp==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    إذا(length<=1) {
        أرجع;
    } وإلا إذا(length<MIN_QSORT || مرتب_سازی_Stable) {
        insertion_مرتب_سازی((حرف *)array, length, قلم_اندازه, cmp, context, p_Error_کد);
    } وإلا {
        سریع_مرتب_سازی((حرف *)array, length, قلم_اندازه, cmp, context, p_Error_کد);
    }
}
