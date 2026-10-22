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
    int32_t max_صفون;
    int32_t صفون;
    int32_t prev_صف;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#وصف_ڪريو UPVEC_INITIAL_صفون (1<<12)
#وصف_ڪريو UPVEC_MEDIUM_صفون ((int32_t)1<<16)
#وصف_ڪريو UPVEC_MAX_صفون (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_ڪوڊ *p_Error_ڪوڊ) {
    UPropsVectors *pv;
    uint32_t *v, *صف;
    uint32_t cp;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(columns<1) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_صفون*columns*4);
    إذا(pv==خالي_اشارو || v==خالي_اشارو) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_صفون=UPVEC_INITIAL_صفون;
    pv->صفون=2+(UPVEC_MAX_CP-UPVEC_پهريون_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    صف=pv->v;
    uprv_memset(صف, 0, pv->صفون*columns*4);
    صف[0]=0;
    صف[1]=0x110000;
    صف+=columns;
    لكل(cp=UPVEC_پهريون_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        صف[0]=cp;
        صف[1]=cp+1;
        صف+=columns;
    }
    أرجع pv;
}

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    إذا(pv!=خالي_اشارو) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساكن uint32_t *
ڳوليو_صف(UPropsVectors *pv, UChar32 range_شروعات) {
    uint32_t *صف;
    int32_t columns, i, شروعات, حد_بندي, prev_صف;

    columns=pv->columns;
    حد_بندي=pv->صفون;
    prev_صف=pv->prev_صف;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    صف=pv->v+prev_صف*columns;
    إذا (range_شروعات >= static_cast<UChar32>(صف[0])) {
        إذا (range_شروعات < static_cast<UChar32>(صف[1])) {
            /* same row as last seen */
            أرجع صف;
        } وإلا إذا (range_شروعات < static_cast<UChar32>((صف += columns)[1])) {
            /* next row after the last one */
            pv->prev_صف=prev_صف+1;
            أرجع صف;
        } وإلا إذا (range_شروعات < static_cast<UChar32>((صف += columns)[1])) {
            /* second row after the last one */
            pv->prev_صف=prev_صف+2;
            أرجع صف;
        } وإلا إذا ((range_شروعات - static_cast<UChar32>(صف[1])) < 10) {
            /* we are close, continue looping */
            prev_صف+=2;
            افعل {
                ++prev_صف;
                صف+=columns;
            } طالما (range_شروعات >= static_cast<UChar32>(صف[1]));
            pv->prev_صف=prev_صف;
            أرجع صف;
        }
    } وإلا إذا (range_شروعات < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_صف=0;
        أرجع pv->v;
    }

    /* do a binary search for the start of the range */
    شروعات=0;
    طالما(شروعات<حد_بندي-1) {
        i=(شروعات+حد_بندي)/2;
        صف=pv->v+i*columns;
        إذا (range_شروعات < static_cast<UChar32>(صف[0])) {
            حد_بندي=i;
        } وإلا إذا (range_شروعات < static_cast<UChar32>(صف[1])) {
            pv->prev_صف=i;
            أرجع صف;
        } وإلا {
            شروعات=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_صف=شروعات;
    أرجع pv->v+شروعات*columns;
}

U_CAPI فراغ U_EXPORT2
upvec_سيٽ_قدر(UPropsVectors *pv,
               UChar32 شروعات, UChar32 پڇاڙي,
               int32_t ڪالم,
               uint32_t value, uint32_t mask,
               UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t *پهريون_صف, *آخري_صف;
    int32_t columns;
    UChar32 حد_بندي;
    UBool ورهايو_پهريون_صف, ورهايو_آخري_صف;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا( pv==خالي_اشارو ||
        شروعات<0 || شروعات>پڇاڙي || پڇاڙي>UPVEC_MAX_CP ||
        ڪالم<0 || ڪالم>=(pv->columns-2)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        *p_Error_ڪوڊ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
    حد_بندي=پڇاڙي+1;

    /* initialize */
    columns=pv->columns;
    ڪالم+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    پهريون_صف=ڳوليو_صف(pv, شروعات);
    آخري_صف=ڳوليو_صف(pv, پڇاڙي);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    ورهايو_پهريون_صف = شروعات != static_cast<UChar32>(پهريون_صف[0]) && value != (پهريون_صف[ڪالم] & mask);
    ورهايو_آخري_صف = حد_بندي != static_cast<UChar32>(آخري_صف[1]) && value != (آخري_صف[ڪالم] & mask);

    /* split first/last rows if necessary */
    إذا(ورهايو_پهريون_صف || ورهايو_آخري_صف) {
        int32_t count, صفون;

        صفون=pv->صفون;
        إذا((صفون+ورهايو_پهريون_صف+ورهايو_آخري_صف)>pv->max_صفون) {
            uint32_t *newVectors;
            int32_t new_Max_صفون;

            إذا(pv->max_صفون<UPVEC_MEDIUM_صفون) {
                new_Max_صفون=UPVEC_MEDIUM_صفون;
            } وإلا إذا(pv->max_صفون<UPVEC_MAX_صفون) {
                new_Max_صفون=UPVEC_MAX_صفون;
            } وإلا {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_ڪوڊ=U_اندروني_پروگرام_ERROR;
                أرجع;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_صفون*columns*4);
            إذا(newVectors==خالي_اشارو) {
                *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)صفون*columns*4);
            پهريون_صف=newVectors+(پهريون_صف-pv->v);
            آخري_صف=newVectors+(آخري_صف-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_صفون=new_Max_صفون;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+صفون*columns)-(آخري_صف+columns));
        إذا(count>0) {
            uprv_memmove(
                آخري_صف+(1+ورهايو_پهريون_صف+ورهايو_آخري_صف)*columns,
                آخري_صف+columns,
                count*4);
        }
        pv->صفون=صفون+ورهايو_پهريون_صف+ورهايو_آخري_صف;

        /* split the first row, and move the firstRow pointer to the second part */
        إذا(ورهايو_پهريون_صف) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((آخري_صف-پهريون_صف)+columns);
            uprv_memmove(پهريون_صف+columns, پهريون_صف, (size_t)count*4);
            آخري_صف+=columns;

            /* split the range and move the firstRow pointer */
            پهريون_صف[1]=پهريون_صف[columns]=(uint32_t)شروعات;
            پهريون_صف+=columns;
        }

        /* split the last row */
        إذا(ورهايو_آخري_صف) {
            /* copy the last row data */
            uprv_memcpy(آخري_صف+columns, آخري_صف, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            آخري_صف[1]=آخري_صف[columns]=(uint32_t)حد_بندي;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_صف=(int32_t)((آخري_صف-(pv->v))/columns);

    /* set the input value in all remaining rows */
    پهريون_صف+=ڪالم;
    آخري_صف+=ڪالم;
    mask=~mask;
    لكل(;;) {
        *پهريون_صف=(*پهريون_صف&mask)|value;
        إذا(پهريون_صف==آخري_صف) {
            اكسر;
        }
        پهريون_صف+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_قدر(ثابت UPropsVectors *pv, UChar32 c, int32_t ڪالم) {
    uint32_t *صف;
    UPropsVectors *ncpv;

    إذا(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || ڪالم<0 || ڪالم>=(pv->columns-2)) {
        أرجع 0;
    }
    ncpv=(UPropsVectors *)pv;
    صف=ڳوليو_صف(ncpv, c);
    أرجع صف[2+ڪالم];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_صف(ثابت UPropsVectors *pv, int32_t صف_اشاريو,
             UChar32 *p_Range_شروعات, UChar32 *p_Range_پڇاڙي) {
    uint32_t *صف;
    int32_t columns;

    إذا(pv->isCompacted || صف_اشاريو<0 || صف_اشاريو>=pv->صفون) {
        أرجع خالي_اشارو;
    }

    columns=pv->columns;
    صف=pv->v+صف_اشاريو*columns;
    إذا(p_Range_شروعات!=خالي_اشارو) {
        *p_Range_شروعات=(UChar32)صف[0];
    }
    إذا(p_Range_پڇاڙي!=خالي_اشارو) {
        *p_Range_پڇاڙي=(UChar32)صف[1]-1;
    }
    أرجع صف+2;
}

ساكن int32_t U_CALLCONV
upvec_compare_صفون(ثابت فراغ *context, ثابت فراغ *l, ثابت فراغ *r) {
    ثابت uint32_t* کاٻي = static_cast<ثابت uint32_t*>(l), *ساڄي = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    افعل {
        إذا(کاٻي[i]!=ساڄي[i]) {
            أرجع کاٻي[i]<ساڄي[i] ? -1 : 1;
        }
        إذا(++i==columns) {
            i=0;
        }
    } طالما(--count>0);

    أرجع 0;
}

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t *صف;
    int32_t i, columns, قدر_Columns, صفون, count;
    UChar32 شروعات, حد_بندي;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا(handler==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        أرجع;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=سچ;

    صفون=pv->صفون;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    قدر_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ترتيب_ڏيو_Array(pv->v, صفون, columns*4,
                   upvec_compare_صفون, pv, خطأ, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    صف=pv->v;
    count=-قدر_Columns;
    لكل(i=0; i<صفون; ++i) {
        شروعات=(UChar32)صف[0];

        /* count a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(صف+2, صف-قدر_Columns, قدر_Columns*4)) {
            count+=قدر_Columns;
        }

        إذا(شروعات>=UPVEC_پهريون_SPECIAL_CP) {
            handler(context, شروعات, شروعات, count, صف+2, قدر_Columns, p_Error_ڪوڊ);
            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                أرجع;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=قدر_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_شروعات_REAL_قدرون_CP, UPVEC_شروعات_REAL_قدرون_CP,
            count, صف-قدر_Columns, قدر_Columns, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
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
    لكل(i=0; i<صفون; ++i) {
        /* fetch these first before memmove() may overwrite them */
        شروعات=(UChar32)صف[0];
        حد_بندي=(UChar32)صف[1];

        /* add a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(صف+2, pv->v+count, قدر_Columns*4)) {
            count+=قدر_Columns;
            uprv_memmove(pv->v+count, صف+2, (size_t)قدر_Columns*4);
        }

        إذا(شروعات<UPVEC_پهريون_SPECIAL_CP) {
            handler(context, شروعات, حد_بندي-1, count, pv->v+count, قدر_Columns, p_Error_ڪوڊ);
            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                أرجع;
            }
        }

        صف+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->صفون=count/قدر_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_صفون, int32_t *pColumns) {
    إذا(!pv->isCompacted) {
        أرجع خالي_اشارو;
    }
    إذا(p_صفون!=خالي_اشارو) {
        *p_صفون=pv->صفون;
    }
    إذا(pColumns!=خالي_اشارو) {
        *pColumns=pv->columns-2;
    }
    أرجع pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_صفون, int32_t *pColumns, UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t *clonedArray;
    int32_t byte_ڊيگهائي;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(!pv->isCompacted) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    byte_ڊيگهائي=pv->صفون*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_ڊيگهائي);
    إذا(clonedArray==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(clonedArray, pv->v, byte_ڊيگهائي);
    إذا(p_صفون!=خالي_اشارو) {
        *p_صفون=pv->صفون;
    }
    إذا(pColumns!=خالي_اشارو) {
        *pColumns=pv->columns-2;
    }
    أرجع clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_منزل_UTrie2_With_صف_Indexes(UPropsVectors *pv, UError_ڪوڊ *p_Error_ڪوڊ) {
    UPVec_منزل_UTrie2_Context منزل_UTrie2={ خالي_اشارو, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_منزل_UTrie2_Handler, &منزل_UTrie2, p_Error_ڪوڊ);
    utrie2_freeze(منزل_UTrie2.trie, UTRIE2_16_قدر_بٽ, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        utrie2_close(منزل_UTrie2.trie);
        منزل_UTrie2.trie=خالي_اشارو;
    }
    أرجع منزل_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI فراغ U_CALLCONV
upvec_compact_منزل_UTrie2_Handler(فراغ *context,
                             UChar32 شروعات, UChar32 پڇاڙي,
                             int32_t صف_اشاريو, uint32_t *صف, int32_t columns,
                             UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)صف;
    (فراغ)columns;
    UPVec_منزل_UTrie2_Context *منزل_UTrie2=(UPVec_منزل_UTrie2_Context *)context;
    إذا(شروعات<UPVEC_پهريون_SPECIAL_CP) {
        utrie2_سيٽ_Range32(منزل_UTrie2->trie, شروعات, پڇاڙي, (uint32_t)صف_اشاريو, سچ, p_Error_ڪوڊ);
    } وإلا {
        بدل(شروعات) {
        حالة UPVEC_INITIAL_قدر_CP:
            منزل_UTrie2->initial_قدر=صف_اشاريو;
            اكسر;
        حالة UPVEC_ERROR_قدر_CP:
            منزل_UTrie2->error_قدر=صف_اشاريو;
            اكسر;
        حالة UPVEC_شروعات_REAL_قدرون_CP:
            منزل_UTrie2->max_قدر=صف_اشاريو;
            إذا(صف_اشاريو>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            } وإلا {
                منزل_UTrie2->trie=utrie2_open(منزل_UTrie2->initial_قدر,
                                           منزل_UTrie2->error_قدر, p_Error_ڪوڊ);
            }
            اكسر;
        افتراضي:
            اكسر;
        }
    }
}
