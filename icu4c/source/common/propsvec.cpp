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

#ضمن <stdlib.h>
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"
#ضمن "utrie2.h"
#ضمن "uarrsort.h"
#ضمن "propsvec.h"
#ضمن "uassert.h"

بنية UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_صفاں;
    int32_t صفاں;
    int32_t prev_صف;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#تعریف_کرو UPVEC_INITIAL_صفاں (1<<12)
#تعریف_کرو UPVEC_MEDIUM_صفاں ((int32_t)1<<16)
#تعریف_کرو UPVEC_MAX_صفاں (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_کوڈ *p_Error_کوڈ) {
    UPropsVectors *pv;
    uint32_t *v, *صف;
    uint32_t cp;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(columns<1) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_صفاں*columns*4);
    إذا(pv==خالی_سوچک || v==خالی_سوچک) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_صفاں=UPVEC_INITIAL_صفاں;
    pv->صفاں=2+(UPVEC_MAX_CP-UPVEC_پہلا_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    صف=pv->v;
    uprv_memset(صف, 0, pv->صفاں*columns*4);
    صف[0]=0;
    صف[1]=0x110000;
    صف+=columns;
    لكل(cp=UPVEC_پہلا_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        صف[0]=cp;
        صف[1]=cp+1;
        صف+=columns;
    }
    أرجع pv;
}

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    إذا(pv!=خالی_سوچک) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساكن uint32_t *
لبھو_صف(UPropsVectors *pv, UChar32 range_شروع) {
    uint32_t *صف;
    int32_t columns, i, شروع, حد_بندی, prev_صف;

    columns=pv->columns;
    حد_بندی=pv->صفاں;
    prev_صف=pv->prev_صف;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    صف=pv->v+prev_صف*columns;
    إذا (range_شروع >= static_cast<UChar32>(صف[0])) {
        إذا (range_شروع < static_cast<UChar32>(صف[1])) {
            /* same row as last seen */
            أرجع صف;
        } وإلا إذا (range_شروع < static_cast<UChar32>((صف += columns)[1])) {
            /* next row after the last one */
            pv->prev_صف=prev_صف+1;
            أرجع صف;
        } وإلا إذا (range_شروع < static_cast<UChar32>((صف += columns)[1])) {
            /* second row after the last one */
            pv->prev_صف=prev_صف+2;
            أرجع صف;
        } وإلا إذا ((range_شروع - static_cast<UChar32>(صف[1])) < 10) {
            /* we are close, continue looping */
            prev_صف+=2;
            افعل {
                ++prev_صف;
                صف+=columns;
            } طالما (range_شروع >= static_cast<UChar32>(صف[1]));
            pv->prev_صف=prev_صف;
            أرجع صف;
        }
    } وإلا إذا (range_شروع < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_صف=0;
        أرجع pv->v;
    }

    /* do a binary search for the start of the range */
    شروع=0;
    طالما(شروع<حد_بندی-1) {
        i=(شروع+حد_بندی)/2;
        صف=pv->v+i*columns;
        إذا (range_شروع < static_cast<UChar32>(صف[0])) {
            حد_بندی=i;
        } وإلا إذا (range_شروع < static_cast<UChar32>(صف[1])) {
            pv->prev_صف=i;
            أرجع صف;
        } وإلا {
            شروع=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_صف=شروع;
    أرجع pv->v+شروع*columns;
}

U_CAPI فراغ U_EXPORT2
upvec_سیٹ_قدر(UPropsVectors *pv,
               UChar32 شروع, UChar32 سرا,
               int32_t کالم,
               uint32_t value, uint32_t mask,
               UError_کوڈ *p_Error_کوڈ) {
    uint32_t *پہلا_صف, *آخری_صف;
    int32_t columns;
    UChar32 حد_بندی;
    UBool ونڈو_پہلا_صف, ونڈو_آخری_صف;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا( pv==خالی_سوچک ||
        شروع<0 || شروع>سرا || سرا>UPVEC_MAX_CP ||
        کالم<0 || کالم>=(pv->columns-2)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        *p_Error_کوڈ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
    حد_بندی=سرا+1;

    /* initialize */
    columns=pv->columns;
    کالم+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    پہلا_صف=لبھو_صف(pv, شروع);
    آخری_صف=لبھو_صف(pv, سرا);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    ونڈو_پہلا_صف = شروع != static_cast<UChar32>(پہلا_صف[0]) && value != (پہلا_صف[کالم] & mask);
    ونڈو_آخری_صف = حد_بندی != static_cast<UChar32>(آخری_صف[1]) && value != (آخری_صف[کالم] & mask);

    /* split first/last rows if necessary */
    إذا(ونڈو_پہلا_صف || ونڈو_آخری_صف) {
        int32_t count, صفاں;

        صفاں=pv->صفاں;
        إذا((صفاں+ونڈو_پہلا_صف+ونڈو_آخری_صف)>pv->max_صفاں) {
            uint32_t *newVectors;
            int32_t new_Max_صفاں;

            إذا(pv->max_صفاں<UPVEC_MEDIUM_صفاں) {
                new_Max_صفاں=UPVEC_MEDIUM_صفاں;
            } وإلا إذا(pv->max_صفاں<UPVEC_MAX_صفاں) {
                new_Max_صفاں=UPVEC_MAX_صفاں;
            } وإلا {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_کوڈ=U_اندرونی_پروگرام_ERROR;
                أرجع;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_صفاں*columns*4);
            إذا(newVectors==خالی_سوچک) {
                *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)صفاں*columns*4);
            پہلا_صف=newVectors+(پہلا_صف-pv->v);
            آخری_صف=newVectors+(آخری_صف-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_صفاں=new_Max_صفاں;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+صفاں*columns)-(آخری_صف+columns));
        إذا(count>0) {
            uprv_memmove(
                آخری_صف+(1+ونڈو_پہلا_صف+ونڈو_آخری_صف)*columns,
                آخری_صف+columns,
                count*4);
        }
        pv->صفاں=صفاں+ونڈو_پہلا_صف+ونڈو_آخری_صف;

        /* split the first row, and move the firstRow pointer to the second part */
        إذا(ونڈو_پہلا_صف) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((آخری_صف-پہلا_صف)+columns);
            uprv_memmove(پہلا_صف+columns, پہلا_صف, (size_t)count*4);
            آخری_صف+=columns;

            /* split the range and move the firstRow pointer */
            پہلا_صف[1]=پہلا_صف[columns]=(uint32_t)شروع;
            پہلا_صف+=columns;
        }

        /* split the last row */
        إذا(ونڈو_آخری_صف) {
            /* copy the last row data */
            uprv_memcpy(آخری_صف+columns, آخری_صف, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            آخری_صف[1]=آخری_صف[columns]=(uint32_t)حد_بندی;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_صف=(int32_t)((آخری_صف-(pv->v))/columns);

    /* set the input value in all remaining rows */
    پہلا_صف+=کالم;
    آخری_صف+=کالم;
    mask=~mask;
    لكل(;;) {
        *پہلا_صف=(*پہلا_صف&mask)|value;
        إذا(پہلا_صف==آخری_صف) {
            اكسر;
        }
        پہلا_صف+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_قدر(ثابت UPropsVectors *pv, UChar32 c, int32_t کالم) {
    uint32_t *صف;
    UPropsVectors *ncpv;

    إذا(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || کالم<0 || کالم>=(pv->columns-2)) {
        أرجع 0;
    }
    ncpv=(UPropsVectors *)pv;
    صف=لبھو_صف(ncpv, c);
    أرجع صف[2+کالم];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_صف(ثابت UPropsVectors *pv, int32_t صف_اشاریہ,
             UChar32 *p_Range_شروع, UChar32 *p_Range_سرا) {
    uint32_t *صف;
    int32_t columns;

    إذا(pv->isCompacted || صف_اشاریہ<0 || صف_اشاریہ>=pv->صفاں) {
        أرجع خالی_سوچک;
    }

    columns=pv->columns;
    صف=pv->v+صف_اشاریہ*columns;
    إذا(p_Range_شروع!=خالی_سوچک) {
        *p_Range_شروع=(UChar32)صف[0];
    }
    إذا(p_Range_سرا!=خالی_سوچک) {
        *p_Range_سرا=(UChar32)صف[1]-1;
    }
    أرجع صف+2;
}

ساكن int32_t U_CALLCONV
upvec_compare_صفاں(ثابت فراغ *context, ثابت فراغ *l, ثابت فراغ *r) {
    ثابت uint32_t* کھبے = static_cast<ثابت uint32_t*>(l), *سجے = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    افعل {
        إذا(کھبے[i]!=سجے[i]) {
            أرجع کھبے[i]<سجے[i] ? -1 : 1;
        }
        إذا(++i==columns) {
            i=0;
        }
    } طالما(--count>0);

    أرجع 0;
}

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UError_کوڈ *p_Error_کوڈ) {
    uint32_t *صف;
    int32_t i, columns, قدر_Columns, صفاں, count;
    UChar32 شروع, حد_بندی;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا(handler==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        أرجع;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=سچ;

    صفاں=pv->صفاں;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    قدر_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ترتیب_دیو_Array(pv->v, صفاں, columns*4,
                   upvec_compare_صفاں, pv, خطأ, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    صف=pv->v;
    count=-قدر_Columns;
    لكل(i=0; i<صفاں; ++i) {
        شروع=(UChar32)صف[0];

        /* count a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(صف+2, صف-قدر_Columns, قدر_Columns*4)) {
            count+=قدر_Columns;
        }

        إذا(شروع>=UPVEC_پہلا_SPECIAL_CP) {
            handler(context, شروع, شروع, count, صف+2, قدر_Columns, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
                أرجع;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=قدر_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_شروع_REAL_اقدار_CP, UPVEC_شروع_REAL_اقدار_CP,
            count, صف-قدر_Columns, قدر_Columns, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
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
    لكل(i=0; i<صفاں; ++i) {
        /* fetch these first before memmove() may overwrite them */
        شروع=(UChar32)صف[0];
        حد_بندی=(UChar32)صف[1];

        /* add a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(صف+2, pv->v+count, قدر_Columns*4)) {
            count+=قدر_Columns;
            uprv_memmove(pv->v+count, صف+2, (size_t)قدر_Columns*4);
        }

        إذا(شروع<UPVEC_پہلا_SPECIAL_CP) {
            handler(context, شروع, حد_بندی-1, count, pv->v+count, قدر_Columns, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
                أرجع;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->صفاں=count/قدر_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_صفاں, int32_t *pColumns) {
    إذا(!pv->isCompacted) {
        أرجع خالی_سوچک;
    }
    إذا(p_صفاں!=خالی_سوچک) {
        *p_صفاں=pv->صفاں;
    }
    إذا(pColumns!=خالی_سوچک) {
        *pColumns=pv->columns-2;
    }
    أرجع pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_صفاں, int32_t *pColumns, UError_کوڈ *p_Error_کوڈ) {
    uint32_t *clonedArray;
    int32_t byte_لمائی;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(!pv->isCompacted) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    byte_لمائی=pv->صفاں*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_لمائی);
    إذا(clonedArray==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(clonedArray, pv->v, byte_لمائی);
    إذا(p_صفاں!=خالی_سوچک) {
        *p_صفاں=pv->صفاں;
    }
    إذا(pColumns!=خالی_سوچک) {
        *pColumns=pv->columns-2;
    }
    أرجع clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_منزل_UTrie2_With_صف_Indexes(UPropsVectors *pv, UError_کوڈ *p_Error_کوڈ) {
    UPVec_منزل_UTrie2_Context منزل_UTrie2={ خالی_سوچک, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_منزل_UTrie2_Handler, &منزل_UTrie2, p_Error_کوڈ);
    utrie2_freeze(منزل_UTrie2.trie, UTRIE2_16_قدر_بٹ, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        utrie2_close(منزل_UTrie2.trie);
        منزل_UTrie2.trie=خالی_سوچک;
    }
    أرجع منزل_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI فراغ U_CALLCONV
upvec_compact_منزل_UTrie2_Handler(فراغ *context,
                             UChar32 شروع, UChar32 سرا,
                             int32_t صف_اشاریہ, uint32_t *صف, int32_t columns,
                             UError_کوڈ *p_Error_کوڈ) {
    (فراغ)صف;
    (فراغ)columns;
    UPVec_منزل_UTrie2_Context *منزل_UTrie2=(UPVec_منزل_UTrie2_Context *)context;
    إذا(شروع<UPVEC_پہلا_SPECIAL_CP) {
        utrie2_سیٹ_Range32(منزل_UTrie2->trie, شروع, سرا, (uint32_t)صف_اشاریہ, سچ, p_Error_کوڈ);
    } وإلا {
        بدل(شروع) {
        حالة UPVEC_INITIAL_قدر_CP:
            منزل_UTrie2->initial_قدر=صف_اشاریہ;
            اكسر;
        حالة UPVEC_ERROR_قدر_CP:
            منزل_UTrie2->error_قدر=صف_اشاریہ;
            اكسر;
        حالة UPVEC_شروع_REAL_اقدار_CP:
            منزل_UTrie2->max_قدر=صف_اشاریہ;
            إذا(صف_اشاریہ>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            } وإلا {
                منزل_UTrie2->trie=utrie2_open(منزل_UTrie2->initial_قدر,
                                           منزل_UTrie2->error_قدر, p_Error_کوڈ);
            }
            اكسر;
        افتراضي:
            اكسر;
        }
    }
}
