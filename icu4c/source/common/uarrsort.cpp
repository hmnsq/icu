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

#شامل <cstddef>

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "uarrsort.h"

شمار {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    ڈھیر_شے_سائز=200
};

ساکن constexpr int32_t سائز_اندر_Max_Align_Ts(int32_t سائز_اندر_بائٹیں) {
    واپس (سائز_اندر_بائٹیں + حجم(std::max_align_t) - 1) / حجم(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    (خالی)context;
    واپس (int32_t)*(ثابت uint16_t *)بائیں - (int32_t)*(ثابت uint16_t *)دائیں;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    (خالی)context;
    واپس *(ثابت int32_t *)بائیں - *(ثابت int32_t *)دائیں;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    (خالی)context;
    uint32_t l=*(ثابت uint32_t *)بائیں, r=*(ثابت uint32_t *)دائیں;

    /* compare directly because (l-r) would overflow the int32_t result */
    اگر(l<r) {
        واپس -1;
    } ورنہ اگر(l==r) {
        واپس 0;
    } ورنہ /* l>r */ {
        واپس 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ثنائی_تلاش(حرف *array, int32_t حد_بندی, خالی *شے, int32_t شے_سائز,
                        UComparator *cmp, ثابت خالی *context) {
    int32_t آغاز=0;
    UBool ملا=جھوٹ;

    /* Binary search until we get down to a tiny sub-array. */
    جب((حد_بندی-آغاز)>=MIN_QSORT) {
        int32_t i=(آغاز+حد_بندی)/2;
        int32_t diff=cmp(context, شے, array+i*شے_سائز);
        اگر(diff==0) {
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
            ملا=سچ;
            آغاز=i+1;
        } ورنہ اگر(diff<0) {
            حد_بندی=i;
        } ورنہ {
            آغاز=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    جب(آغاز<حد_بندی) {
        int32_t diff=cmp(context, شے, array+آغاز*شے_سائز);
        اگر(diff==0) {
            ملا=سچ;
        } ورنہ اگر(diff<0) {
            توڑو;
        }
        ++آغاز;
    }
    واپس ملا ? (آغاز-1) : ~آغاز;
}

ساکن خالی
do_Insertion_ترتیب_دو(حرف *array, int32_t length, int32_t شے_سائز,
                UComparator *cmp, ثابت خالی *context, خالی *pv) {
    int32_t j;

    برائے(j=1; j<length; ++j) {
        حرف *شے=array+j*شے_سائز;
        int32_t insertion_نقطہ=uprv_stable_ثنائی_تلاش(array, j, شے, شے_سائز, cmp, context);
        اگر(insertion_نقطہ<0) {
            insertion_نقطہ=~insertion_نقطہ;
        } ورنہ {
            ++insertion_نقطہ;  /* one past the last equal item */
        }
        اگر(insertion_نقطہ<j) {
            حرف *dest=array+insertion_نقطہ*شے_سائز;
            uprv_memcpy(pv, شے, شے_سائز);  /* v=array[j] */
            uprv_memmove(dest+شے_سائز, dest, (j-insertion_نقطہ)*(size_t)شے_سائز);
            uprv_memcpy(dest, pv, شے_سائز);  /* array[insertionPoint]=v */
        }
    }
}

ساکن خالی
insertion_ترتیب_دو(حرف *array, int32_t length, int32_t شے_سائز,
              UComparator *cmp, ثابت خالی *context, UError_رمز *p_Error_رمز) {

    icu::Maybe_ڈھیر_Array<std::max_align_t, سائز_اندر_Max_Align_Ts(ڈھیر_شے_سائز)> v;
    اگر (سائز_اندر_Max_Align_Ts(شے_سائز) > v.get_استعداد() &&
            v.resize(سائز_اندر_Max_Align_Ts(شے_سائز)) == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    do_Insertion_ترتیب_دو(array, length, شے_سائز, cmp, context, v.getAlias());
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
ساکن خالی
sub_تیز_ترتیب_دو(حرف *array, int32_t آغاز, int32_t حد_بندی, int32_t شے_سائز,
             UComparator *cmp, ثابت خالی *context,
             خالی *px, خالی *pw) {
    int32_t بائیں, دائیں;

    /* start and left are inclusive, limit and right are exclusive */
    کرو {
        اگر((آغاز+MIN_QSORT)>=حد_بندی) {
            do_Insertion_ترتیب_دو(array+آغاز*شے_سائز, حد_بندی-آغاز, شے_سائز, cmp, context, px);
            توڑو;
        }

        بائیں=آغاز;
        دائیں=حد_بندی;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((آغاز+حد_بندی)/2)*شے_سائز, شے_سائز);

        کرو {
            جب(/* array[left]<x */
                  cmp(context, array+بائیں*شے_سائز, px)<0
            ) {
                ++بائیں;
            }
            جب(/* x<array[right-1] */
                  cmp(context, px, array+(دائیں-1)*شے_سائز)<0
            ) {
                --دائیں;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            اگر(بائیں<دائیں) {
                --دائیں;

                اگر(بائیں<دائیں) {
                    uprv_memcpy(pw, array+(size_t)بائیں*شے_سائز, شے_سائز);
                    uprv_memcpy(array+(size_t)بائیں*شے_سائز, array+(size_t)دائیں*شے_سائز, شے_سائز);
                    uprv_memcpy(array+(size_t)دائیں*شے_سائز, pw, شے_سائز);
                }

                ++بائیں;
            }
        } جب(بائیں<دائیں);

        /* sort sub-arrays */
        اگر((دائیں-آغاز)<(حد_بندی-بائیں)) {
            /* sort [start..right[ */
            اگر(آغاز<(دائیں-1)) {
                sub_تیز_ترتیب_دو(array, آغاز, دائیں, شے_سائز, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            آغاز=بائیں;
        } ورنہ {
            /* sort [left..limit[ */
            اگر(بائیں<(حد_بندی-1)) {
                sub_تیز_ترتیب_دو(array, بائیں, حد_بندی, شے_سائز, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            حد_بندی=دائیں;
        }
    } جب(آغاز<(حد_بندی-1));
}

ساکن خالی
تیز_ترتیب_دو(حرف *array, int32_t length, int32_t شے_سائز,
            UComparator *cmp, ثابت خالی *context, UError_رمز *p_Error_رمز) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_ڈھیر_Array<std::max_align_t, سائز_اندر_Max_Align_Ts(ڈھیر_شے_سائز) * 2> xw;
    اگر(سائز_اندر_Max_Align_Ts(شے_سائز)*2 > xw.get_استعداد() &&
            xw.resize(سائز_اندر_Max_Align_Ts(شے_سائز) * 2) == خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    sub_تیز_ترتیب_دو(array, 0, length, شے_سائز, cmp, context,
                 xw.getAlias(), xw.getAlias() + سائز_اندر_Max_Align_Ts(شے_سائز));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI خالی U_EXPORT2
uprv_ترتیب_دو_Array(خالی *array, int32_t length, int32_t شے_سائز,
               UComparator *cmp, ثابت خالی *context,
               UBool ترتیب_دو_Stable, UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر((length>0 && array==خالی_اشاریہ) || length<0 || شے_سائز<=0 || cmp==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر(length<=1) {
        واپس;
    } ورنہ اگر(length<MIN_QSORT || ترتیب_دو_Stable) {
        insertion_ترتیب_دو((حرف *)array, length, شے_سائز, cmp, context, p_Error_رمز);
    } ورنہ {
        تیز_ترتیب_دو((حرف *)array, length, شے_سائز, cmp, context, p_Error_رمز);
    }
}
