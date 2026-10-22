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
    int32_t max_ردیفها;
    int32_t ردیفها;
    int32_t prev_ردیف;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#تعریف_کن UPVEC_INITIAL_ردیفها (1<<12)
#تعریف_کن UPVEC_MEDIUM_ردیفها ((int32_t)1<<16)
#تعریف_کن UPVEC_MAX_ردیفها (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_کد *p_Error_کد) {
    UPropsVectors *pv;
    uint32_t *v, *ردیف;
    uint32_t cp;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(columns<1) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ردیفها*columns*4);
    إذا(pv==اشاره_تهی || v==اشاره_تهی) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_ردیفها=UPVEC_INITIAL_ردیفها;
    pv->ردیفها=2+(UPVEC_MAX_CP-UPVEC_اولی_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    ردیف=pv->v;
    uprv_memset(ردیف, 0, pv->ردیفها*columns*4);
    ردیف[0]=0;
    ردیف[1]=0x110000;
    ردیف+=columns;
    لكل(cp=UPVEC_اولی_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        ردیف[0]=cp;
        ردیف[1]=cp+1;
        ردیف+=columns;
    }
    أرجع pv;
}

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    إذا(pv!=اشاره_تهی) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساكن uint32_t *
پیدا_کن_ردیف(UPropsVectors *pv, UChar32 range_شروع) {
    uint32_t *ردیف;
    int32_t columns, i, شروع, حد, prev_ردیف;

    columns=pv->columns;
    حد=pv->ردیفها;
    prev_ردیف=pv->prev_ردیف;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    ردیف=pv->v+prev_ردیف*columns;
    إذا (range_شروع >= static_cast<UChar32>(ردیف[0])) {
        إذا (range_شروع < static_cast<UChar32>(ردیف[1])) {
            /* same row as last seen */
            أرجع ردیف;
        } وإلا إذا (range_شروع < static_cast<UChar32>((ردیف += columns)[1])) {
            /* next row after the last one */
            pv->prev_ردیف=prev_ردیف+1;
            أرجع ردیف;
        } وإلا إذا (range_شروع < static_cast<UChar32>((ردیف += columns)[1])) {
            /* second row after the last one */
            pv->prev_ردیف=prev_ردیف+2;
            أرجع ردیف;
        } وإلا إذا ((range_شروع - static_cast<UChar32>(ردیف[1])) < 10) {
            /* we are close, continue looping */
            prev_ردیف+=2;
            افعل {
                ++prev_ردیف;
                ردیف+=columns;
            } طالما (range_شروع >= static_cast<UChar32>(ردیف[1]));
            pv->prev_ردیف=prev_ردیف;
            أرجع ردیف;
        }
    } وإلا إذا (range_شروع < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_ردیف=0;
        أرجع pv->v;
    }

    /* do a binary search for the start of the range */
    شروع=0;
    طالما(شروع<حد-1) {
        i=(شروع+حد)/2;
        ردیف=pv->v+i*columns;
        إذا (range_شروع < static_cast<UChar32>(ردیف[0])) {
            حد=i;
        } وإلا إذا (range_شروع < static_cast<UChar32>(ردیف[1])) {
            pv->prev_ردیف=i;
            أرجع ردیف;
        } وإلا {
            شروع=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_ردیف=شروع;
    أرجع pv->v+شروع*columns;
}

U_CAPI فراغ U_EXPORT2
upvec_مجموعه_مقدار(UPropsVectors *pv,
               UChar32 شروع, UChar32 انتها,
               int32_t ستونی,
               uint32_t value, uint32_t mask,
               UError_کد *p_Error_کد) {
    uint32_t *اولی_ردیف, *آخرین_ردیف;
    int32_t columns;
    UChar32 حد;
    UBool تقسیم_اولی_ردیف, تقسیم_آخرین_ردیف;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا( pv==اشاره_تهی ||
        شروع<0 || شروع>انتها || انتها>UPVEC_MAX_CP ||
        ستونی<0 || ستونی>=(pv->columns-2)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        *p_Error_کد=U_NO_WRITE_PERMISSION;
        أرجع;
    }
    حد=انتها+1;

    /* initialize */
    columns=pv->columns;
    ستونی+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    اولی_ردیف=پیدا_کن_ردیف(pv, شروع);
    آخرین_ردیف=پیدا_کن_ردیف(pv, انتها);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    تقسیم_اولی_ردیف = شروع != static_cast<UChar32>(اولی_ردیف[0]) && value != (اولی_ردیف[ستونی] & mask);
    تقسیم_آخرین_ردیف = حد != static_cast<UChar32>(آخرین_ردیف[1]) && value != (آخرین_ردیف[ستونی] & mask);

    /* split first/last rows if necessary */
    إذا(تقسیم_اولی_ردیف || تقسیم_آخرین_ردیف) {
        int32_t count, ردیفها;

        ردیفها=pv->ردیفها;
        إذا((ردیفها+تقسیم_اولی_ردیف+تقسیم_آخرین_ردیف)>pv->max_ردیفها) {
            uint32_t *newVectors;
            int32_t new_Max_ردیفها;

            إذا(pv->max_ردیفها<UPVEC_MEDIUM_ردیفها) {
                new_Max_ردیفها=UPVEC_MEDIUM_ردیفها;
            } وإلا إذا(pv->max_ردیفها<UPVEC_MAX_ردیفها) {
                new_Max_ردیفها=UPVEC_MAX_ردیفها;
            } وإلا {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_کد=U_داخلی_برنامه_ERROR;
                أرجع;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_ردیفها*columns*4);
            إذا(newVectors==اشاره_تهی) {
                *p_Error_کد=U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)ردیفها*columns*4);
            اولی_ردیف=newVectors+(اولی_ردیف-pv->v);
            آخرین_ردیف=newVectors+(آخرین_ردیف-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_ردیفها=new_Max_ردیفها;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+ردیفها*columns)-(آخرین_ردیف+columns));
        إذا(count>0) {
            uprv_memmove(
                آخرین_ردیف+(1+تقسیم_اولی_ردیف+تقسیم_آخرین_ردیف)*columns,
                آخرین_ردیف+columns,
                count*4);
        }
        pv->ردیفها=ردیفها+تقسیم_اولی_ردیف+تقسیم_آخرین_ردیف;

        /* split the first row, and move the firstRow pointer to the second part */
        إذا(تقسیم_اولی_ردیف) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((آخرین_ردیف-اولی_ردیف)+columns);
            uprv_memmove(اولی_ردیف+columns, اولی_ردیف, (size_t)count*4);
            آخرین_ردیف+=columns;

            /* split the range and move the firstRow pointer */
            اولی_ردیف[1]=اولی_ردیف[columns]=(uint32_t)شروع;
            اولی_ردیف+=columns;
        }

        /* split the last row */
        إذا(تقسیم_آخرین_ردیف) {
            /* copy the last row data */
            uprv_memcpy(آخرین_ردیف+columns, آخرین_ردیف, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            آخرین_ردیف[1]=آخرین_ردیف[columns]=(uint32_t)حد;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_ردیف=(int32_t)((آخرین_ردیف-(pv->v))/columns);

    /* set the input value in all remaining rows */
    اولی_ردیف+=ستونی;
    آخرین_ردیف+=ستونی;
    mask=~mask;
    لكل(;;) {
        *اولی_ردیف=(*اولی_ردیف&mask)|value;
        إذا(اولی_ردیف==آخرین_ردیف) {
            اكسر;
        }
        اولی_ردیف+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_مقدار(ثابت UPropsVectors *pv, UChar32 c, int32_t ستونی) {
    uint32_t *ردیف;
    UPropsVectors *ncpv;

    إذا(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || ستونی<0 || ستونی>=(pv->columns-2)) {
        أرجع 0;
    }
    ncpv=(UPropsVectors *)pv;
    ردیف=پیدا_کن_ردیف(ncpv, c);
    أرجع ردیف[2+ستونی];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_ردیف(ثابت UPropsVectors *pv, int32_t ردیف_شاخص,
             UChar32 *p_Range_شروع, UChar32 *p_Range_انتها) {
    uint32_t *ردیف;
    int32_t columns;

    إذا(pv->isCompacted || ردیف_شاخص<0 || ردیف_شاخص>=pv->ردیفها) {
        أرجع اشاره_تهی;
    }

    columns=pv->columns;
    ردیف=pv->v+ردیف_شاخص*columns;
    إذا(p_Range_شروع!=اشاره_تهی) {
        *p_Range_شروع=(UChar32)ردیف[0];
    }
    إذا(p_Range_انتها!=اشاره_تهی) {
        *p_Range_انتها=(UChar32)ردیف[1]-1;
    }
    أرجع ردیف+2;
}

ساكن int32_t U_CALLCONV
upvec_compare_ردیفها(ثابت فراغ *context, ثابت فراغ *l, ثابت فراغ *r) {
    ثابت uint32_t* چپ = static_cast<ثابت uint32_t*>(l), *راست = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    افعل {
        إذا(چپ[i]!=راست[i]) {
            أرجع چپ[i]<راست[i] ? -1 : 1;
        }
        إذا(++i==columns) {
            i=0;
        }
    } طالما(--count>0);

    أرجع 0;
}

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UError_کد *p_Error_کد) {
    uint32_t *ردیف;
    int32_t i, columns, مقدار_Columns, ردیفها, count;
    UChar32 شروع, حد;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا(handler==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        أرجع;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=درست;

    ردیفها=pv->ردیفها;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    مقدار_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_مرتب_سازی_Array(pv->v, ردیفها, columns*4,
                   upvec_compare_ردیفها, pv, خطأ, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    ردیف=pv->v;
    count=-مقدار_Columns;
    لكل(i=0; i<ردیفها; ++i) {
        شروع=(UChar32)ردیف[0];

        /* count a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(ردیف+2, ردیف-مقدار_Columns, مقدار_Columns*4)) {
            count+=مقدار_Columns;
        }

        إذا(شروع>=UPVEC_اولی_SPECIAL_CP) {
            handler(context, شروع, شروع, count, ردیف+2, مقدار_Columns, p_Error_کد);
            إذا(U_FAILURE(*p_Error_کد)) {
                أرجع;
            }
        }

        ردیف+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=مقدار_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_شروع_REAL_مقادیر_CP, UPVEC_شروع_REAL_مقادیر_CP,
            count, ردیف-مقدار_Columns, مقدار_Columns, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    ردیف=pv->v;
    count=-مقدار_Columns;
    لكل(i=0; i<ردیفها; ++i) {
        /* fetch these first before memmove() may overwrite them */
        شروع=(UChar32)ردیف[0];
        حد=(UChar32)ردیف[1];

        /* add a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(ردیف+2, pv->v+count, مقدار_Columns*4)) {
            count+=مقدار_Columns;
            uprv_memmove(pv->v+count, ردیف+2, (size_t)مقدار_Columns*4);
        }

        إذا(شروع<UPVEC_اولی_SPECIAL_CP) {
            handler(context, شروع, حد-1, count, pv->v+count, مقدار_Columns, p_Error_کد);
            إذا(U_FAILURE(*p_Error_کد)) {
                أرجع;
            }
        }

        ردیف+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->ردیفها=count/مقدار_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_ردیفها, int32_t *pColumns) {
    إذا(!pv->isCompacted) {
        أرجع اشاره_تهی;
    }
    إذا(p_ردیفها!=اشاره_تهی) {
        *p_ردیفها=pv->ردیفها;
    }
    إذا(pColumns!=اشاره_تهی) {
        *pColumns=pv->columns-2;
    }
    أرجع pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_ردیفها, int32_t *pColumns, UError_کد *p_Error_کد) {
    uint32_t *clonedArray;
    int32_t byte_درازی;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(!pv->isCompacted) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    byte_درازی=pv->ردیفها*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_درازی);
    إذا(clonedArray==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(clonedArray, pv->v, byte_درازی);
    إذا(p_ردیفها!=اشاره_تهی) {
        *p_ردیفها=pv->ردیفها;
    }
    إذا(pColumns!=اشاره_تهی) {
        *pColumns=pv->columns-2;
    }
    أرجع clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_مقصد_UTrie2_With_ردیف_Indexes(UPropsVectors *pv, UError_کد *p_Error_کد) {
    UPVec_مقصد_UTrie2_Context مقصد_UTrie2={ اشاره_تهی, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_مقصد_UTrie2_Handler, &مقصد_UTrie2, p_Error_کد);
    utrie2_freeze(مقصد_UTrie2.trie, UTRIE2_16_مقدار_بیتها, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        utrie2_close(مقصد_UTrie2.trie);
        مقصد_UTrie2.trie=اشاره_تهی;
    }
    أرجع مقصد_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI فراغ U_CALLCONV
upvec_compact_مقصد_UTrie2_Handler(فراغ *context,
                             UChar32 شروع, UChar32 انتها,
                             int32_t ردیف_شاخص, uint32_t *ردیف, int32_t columns,
                             UError_کد *p_Error_کد) {
    (فراغ)ردیف;
    (فراغ)columns;
    UPVec_مقصد_UTrie2_Context *مقصد_UTrie2=(UPVec_مقصد_UTrie2_Context *)context;
    إذا(شروع<UPVEC_اولی_SPECIAL_CP) {
        utrie2_مجموعه_Range32(مقصد_UTrie2->trie, شروع, انتها, (uint32_t)ردیف_شاخص, درست, p_Error_کد);
    } وإلا {
        بدل(شروع) {
        حالة UPVEC_INITIAL_مقدار_CP:
            مقصد_UTrie2->initial_مقدار=ردیف_شاخص;
            اكسر;
        حالة UPVEC_ERROR_مقدار_CP:
            مقصد_UTrie2->error_مقدار=ردیف_شاخص;
            اكسر;
        حالة UPVEC_شروع_REAL_مقادیر_CP:
            مقصد_UTrie2->max_مقدار=ردیف_شاخص;
            إذا(ردیف_شاخص>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            } وإلا {
                مقصد_UTrie2->trie=utrie2_open(مقصد_UTrie2->initial_مقدار,
                                           مقصد_UTrie2->error_مقدار, p_Error_کد);
            }
            اكسر;
        افتراضي:
            اكسر;
        }
    }
}
