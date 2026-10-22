// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  propsvec.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb22
*   created by: Markus W. Scherer
*
*   Store bits (Unicode character properties) in bit set vectors.
*/

#شامل <stdlib.h>
#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "utrie.h"
#شامل "utrie2.h"
#شامل "uarrsort.h"
#شامل "propsvec.h"
#شامل "uassert.h"

ساخت UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_صفیں;
    int32_t صفیں;
    int32_t پچھلا_صف;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#تعریف_کرو UPVEC_INITIAL_صفیں (1<<12)
#تعریف_کرو UPVEC_MEDIUM_صفیں ((int32_t)1<<16)
#تعریف_کرو UPVEC_MAX_صفیں (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_رمز *p_Error_رمز) {
    UPropsVectors *pv;
    uint32_t *v, *صف;
    uint32_t cp;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(columns<1) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_صفیں*columns*4);
    اگر(pv==خالی_اشاریہ || v==خالی_اشاریہ) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_صفیں=UPVEC_INITIAL_صفیں;
    pv->صفیں=2+(UPVEC_MAX_CP-UPVEC_پہلا_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    صف=pv->v;
    uprv_memset(صف, 0, pv->صفیں*columns*4);
    صف[0]=0;
    صف[1]=0x110000;
    صف+=columns;
    برائے(cp=UPVEC_پہلا_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        صف[0]=cp;
        صف[1]=cp+1;
        صف+=columns;
    }
    واپس pv;
}

U_CAPI خالی U_EXPORT2
upvec_close(UPropsVectors *pv) {
    اگر(pv!=خالی_اشاریہ) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساکن uint32_t *
ڈھونڈو_صف(UPropsVectors *pv, UChar32 range_آغاز) {
    uint32_t *صف;
    int32_t columns, i, آغاز, حد_بندی, پچھلا_صف;

    columns=pv->columns;
    حد_بندی=pv->صفیں;
    پچھلا_صف=pv->پچھلا_صف;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    صف=pv->v+پچھلا_صف*columns;
    اگر (range_آغاز >= static_cast<UChar32>(صف[0])) {
        اگر (range_آغاز < static_cast<UChar32>(صف[1])) {
            /* same row as last seen */
            واپس صف;
        } ورنہ اگر (range_آغاز < static_cast<UChar32>((صف += columns)[1])) {
            /* next row after the last one */
            pv->پچھلا_صف=پچھلا_صف+1;
            واپس صف;
        } ورنہ اگر (range_آغاز < static_cast<UChar32>((صف += columns)[1])) {
            /* second row after the last one */
            pv->پچھلا_صف=پچھلا_صف+2;
            واپس صف;
        } ورنہ اگر ((range_آغاز - static_cast<UChar32>(صف[1])) < 10) {
            /* we are close, continue looping */
            پچھلا_صف+=2;
            کرو {
                ++پچھلا_صف;
                صف+=columns;
            } جب (range_آغاز >= static_cast<UChar32>(صف[1]));
            pv->پچھلا_صف=پچھلا_صف;
            واپس صف;
        }
    } ورنہ اگر (range_آغاز < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->پچھلا_صف=0;
        واپس pv->v;
    }

    /* do a binary search for the start of the range */
    آغاز=0;
    جب(آغاز<حد_بندی-1) {
        i=(آغاز+حد_بندی)/2;
        صف=pv->v+i*columns;
        اگر (range_آغاز < static_cast<UChar32>(صف[0])) {
            حد_بندی=i;
        } ورنہ اگر (range_آغاز < static_cast<UChar32>(صف[1])) {
            pv->پچھلا_صف=i;
            واپس صف;
        } ورنہ {
            آغاز=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->پچھلا_صف=آغاز;
    واپس pv->v+آغاز*columns;
}

U_CAPI خالی U_EXPORT2
upvec_سیٹ_قدر(UPropsVectors *pv,
               UChar32 آغاز, UChar32 انتہا,
               int32_t عمود,
               uint32_t value, uint32_t mask,
               UError_رمز *p_Error_رمز) {
    uint32_t *پہلا_صف, *آخری_صف;
    int32_t columns;
    UChar32 حد_بندی;
    UBool بانٹو_پہلا_صف, بانٹو_آخری_صف;

    /* argument checking */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر( pv==خالی_اشاریہ ||
        آغاز<0 || آغاز>انتہا || انتہا>UPVEC_MAX_CP ||
        عمود<0 || عمود>=(pv->columns-2)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر(pv->isCompacted) {
        *p_Error_رمز=U_NO_WRITE_PERMISSION;
        واپس;
    }
    حد_بندی=انتہا+1;

    /* initialize */
    columns=pv->columns;
    عمود+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    پہلا_صف=ڈھونڈو_صف(pv, آغاز);
    آخری_صف=ڈھونڈو_صف(pv, انتہا);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    بانٹو_پہلا_صف = آغاز != static_cast<UChar32>(پہلا_صف[0]) && value != (پہلا_صف[عمود] & mask);
    بانٹو_آخری_صف = حد_بندی != static_cast<UChar32>(آخری_صف[1]) && value != (آخری_صف[عمود] & mask);

    /* split first/last rows if necessary */
    اگر(بانٹو_پہلا_صف || بانٹو_آخری_صف) {
        int32_t count, صفیں;

        صفیں=pv->صفیں;
        اگر((صفیں+بانٹو_پہلا_صف+بانٹو_آخری_صف)>pv->max_صفیں) {
            uint32_t *newVectors;
            int32_t new_Max_صفیں;

            اگر(pv->max_صفیں<UPVEC_MEDIUM_صفیں) {
                new_Max_صفیں=UPVEC_MEDIUM_صفیں;
            } ورنہ اگر(pv->max_صفیں<UPVEC_MAX_صفیں) {
                new_Max_صفیں=UPVEC_MAX_صفیں;
            } ورنہ {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_رمز=U_اندرونی_پروگرام_ERROR;
                واپس;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_صفیں*columns*4);
            اگر(newVectors==خالی_اشاریہ) {
                *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)صفیں*columns*4);
            پہلا_صف=newVectors+(پہلا_صف-pv->v);
            آخری_صف=newVectors+(آخری_صف-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_صفیں=new_Max_صفیں;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+صفیں*columns)-(آخری_صف+columns));
        اگر(count>0) {
            uprv_memmove(
                آخری_صف+(1+بانٹو_پہلا_صف+بانٹو_آخری_صف)*columns,
                آخری_صف+columns,
                count*4);
        }
        pv->صفیں=صفیں+بانٹو_پہلا_صف+بانٹو_آخری_صف;

        /* split the first row, and move the firstRow pointer to the second part */
        اگر(بانٹو_پہلا_صف) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((آخری_صف-پہلا_صف)+columns);
            uprv_memmove(پہلا_صف+columns, پہلا_صف, (size_t)count*4);
            آخری_صف+=columns;

            /* split the range and move the firstRow pointer */
            پہلا_صف[1]=پہلا_صف[columns]=(uint32_t)آغاز;
            پہلا_صف+=columns;
        }

        /* split the last row */
        اگر(بانٹو_آخری_صف) {
            /* copy the last row data */
            uprv_memcpy(آخری_صف+columns, آخری_صف, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            آخری_صف[1]=آخری_صف[columns]=(uint32_t)حد_بندی;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->پچھلا_صف=(int32_t)((آخری_صف-(pv->v))/columns);

    /* set the input value in all remaining rows */
    پہلا_صف+=عمود;
    آخری_صف+=عمود;
    mask=~mask;
    برائے(;;) {
        *پہلا_صف=(*پہلا_صف&mask)|value;
        اگر(پہلا_صف==آخری_صف) {
            توڑو;
        }
        پہلا_صف+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_قدر(ثابت UPropsVectors *pv, UChar32 c, int32_t عمود) {
    uint32_t *صف;
    UPropsVectors *ncpv;

    اگر(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || عمود<0 || عمود>=(pv->columns-2)) {
        واپس 0;
    }
    ncpv=(UPropsVectors *)pv;
    صف=ڈھونڈو_صف(ncpv, c);
    واپس صف[2+عمود];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_صف(ثابت UPropsVectors *pv, int32_t صف_اشاریہ,
             UChar32 *p_Range_آغاز, UChar32 *p_Range_انتہا) {
    uint32_t *صف;
    int32_t columns;

    اگر(pv->isCompacted || صف_اشاریہ<0 || صف_اشاریہ>=pv->صفیں) {
        واپس خالی_اشاریہ;
    }

    columns=pv->columns;
    صف=pv->v+صف_اشاریہ*columns;
    اگر(p_Range_آغاز!=خالی_اشاریہ) {
        *p_Range_آغاز=(UChar32)صف[0];
    }
    اگر(p_Range_انتہا!=خالی_اشاریہ) {
        *p_Range_انتہا=(UChar32)صف[1]-1;
    }
    واپس صف+2;
}

ساکن int32_t U_CALLCONV
upvec_compare_صفیں(ثابت خالی *context, ثابت خالی *l, ثابت خالی *r) {
    ثابت uint32_t* بائیں = static_cast<ثابت uint32_t*>(l), *دائیں = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    کرو {
        اگر(بائیں[i]!=دائیں[i]) {
            واپس بائیں[i]<دائیں[i] ? -1 : 1;
        }
        اگر(++i==columns) {
            i=0;
        }
    } جب(--count>0);

    واپس 0;
}

U_CAPI خالی U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, خالی *context, UError_رمز *p_Error_رمز) {
    uint32_t *صف;
    int32_t i, columns, قدر_Columns, صفیں, count;
    UChar32 آغاز, حد_بندی;

    /* argument checking */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر(handler==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر(pv->isCompacted) {
        واپس;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=سچ;

    صفیں=pv->صفیں;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    قدر_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ترتیب_دو_Array(pv->v, صفیں, columns*4,
                   upvec_compare_صفیں, pv, جھوٹ, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    صف=pv->v;
    count=-قدر_Columns;
    برائے(i=0; i<صفیں; ++i) {
        آغاز=(UChar32)صف[0];

        /* count a new values vector if it is different from the current one */
        اگر(count<0 || 0!=uprv_memcmp(صف+2, صف-قدر_Columns, قدر_Columns*4)) {
            count+=قدر_Columns;
        }

        اگر(آغاز>=UPVEC_پہلا_SPECIAL_CP) {
            handler(context, آغاز, آغاز, count, صف+2, قدر_Columns, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                واپس;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=قدر_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_آغاز_REAL_اقدار_CP, UPVEC_آغاز_REAL_اقدار_CP,
            count, صف-قدر_Columns, قدر_Columns, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    صف=pv->v;
    count=-قدر_Columns;
    برائے(i=0; i<صفیں; ++i) {
        /* fetch these first before memmove() may overwrite them */
        آغاز=(UChar32)صف[0];
        حد_بندی=(UChar32)صف[1];

        /* add a new values vector if it is different from the current one */
        اگر(count<0 || 0!=uprv_memcmp(صف+2, pv->v+count, قدر_Columns*4)) {
            count+=قدر_Columns;
            uprv_memmove(pv->v+count, صف+2, (size_t)قدر_Columns*4);
        }

        اگر(آغاز<UPVEC_پہلا_SPECIAL_CP) {
            handler(context, آغاز, حد_بندی-1, count, pv->v+count, قدر_Columns, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                واپس;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->صفیں=count/قدر_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_صفیں, int32_t *pColumns) {
    اگر(!pv->isCompacted) {
        واپس خالی_اشاریہ;
    }
    اگر(p_صفیں!=خالی_اشاریہ) {
        *p_صفیں=pv->صفیں;
    }
    اگر(pColumns!=خالی_اشاریہ) {
        *pColumns=pv->columns-2;
    }
    واپس pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_صفیں, int32_t *pColumns, UError_رمز *p_Error_رمز) {
    uint32_t *clonedArray;
    int32_t byte_لمبائی;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(!pv->isCompacted) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    byte_لمبائی=pv->صفیں*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_لمبائی);
    اگر(clonedArray==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(clonedArray, pv->v, byte_لمبائی);
    اگر(p_صفیں!=خالی_اشاریہ) {
        *p_صفیں=pv->صفیں;
    }
    اگر(pColumns!=خالی_اشاریہ) {
        *pColumns=pv->columns-2;
    }
    واپس clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_مقصد_UTrie2_With_صف_Indexes(UPropsVectors *pv, UError_رمز *p_Error_رمز) {
    UPVec_مقصد_UTrie2_Context مقصد_UTrie2={ خالی_اشاریہ, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_مقصد_UTrie2_Handler, &مقصد_UTrie2, p_Error_رمز);
    utrie2_freeze(مقصد_UTrie2.trie, UTRIE2_16_قدر_بٹ, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        utrie2_close(مقصد_UTrie2.trie);
        مقصد_UTrie2.trie=خالی_اشاریہ;
    }
    واپس مقصد_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI خالی U_CALLCONV
upvec_compact_مقصد_UTrie2_Handler(خالی *context,
                             UChar32 آغاز, UChar32 انتہا,
                             int32_t صف_اشاریہ, uint32_t *صف, int32_t columns,
                             UError_رمز *p_Error_رمز) {
    (خالی)صف;
    (خالی)columns;
    UPVec_مقصد_UTrie2_Context *مقصد_UTrie2=(UPVec_مقصد_UTrie2_Context *)context;
    اگر(آغاز<UPVEC_پہلا_SPECIAL_CP) {
        utrie2_سیٹ_Range32(مقصد_UTrie2->trie, آغاز, انتہا, (uint32_t)صف_اشاریہ, سچ, p_Error_رمز);
    } ورنہ {
        انتخاب(آغاز) {
        حالت UPVEC_INITIAL_قدر_CP:
            مقصد_UTrie2->initial_قدر=صف_اشاریہ;
            توڑو;
        حالت UPVEC_ERROR_قدر_CP:
            مقصد_UTrie2->error_قدر=صف_اشاریہ;
            توڑو;
        حالت UPVEC_آغاز_REAL_اقدار_CP:
            مقصد_UTrie2->max_قدر=صف_اشاریہ;
            اگر(صف_اشاریہ>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            } ورنہ {
                مقصد_UTrie2->trie=utrie2_open(مقصد_UTrie2->initial_قدر,
                                           مقصد_UTrie2->error_قدر, p_Error_رمز);
            }
            توڑو;
        طےشدہ:
            توڑو;
        }
    }
}
