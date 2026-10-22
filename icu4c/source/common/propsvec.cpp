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
    int32_t max_لیکې;
    int32_t لیکې;
    int32_t prev_لیکه;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#تعریف_کړه UPVEC_INITIAL_لیکې (1<<12)
#تعریف_کړه UPVEC_MEDIUM_لیکې ((int32_t)1<<16)
#تعریف_کړه UPVEC_MAX_لیکې (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_کوډ *p_Error_کوډ) {
    UPropsVectors *pv;
    uint32_t *v, *لیکه;
    uint32_t cp;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(columns<1) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_لیکې*columns*4);
    إذا(pv==تش_اشاره || v==تش_اشاره) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_لیکې=UPVEC_INITIAL_لیکې;
    pv->لیکې=2+(UPVEC_MAX_CP-UPVEC_لومړی_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    لیکه=pv->v;
    uprv_memset(لیکه, 0, pv->لیکې*columns*4);
    لیکه[0]=0;
    لیکه[1]=0x110000;
    لیکه+=columns;
    لكل(cp=UPVEC_لومړی_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        لیکه[0]=cp;
        لیکه[1]=cp+1;
        لیکه+=columns;
    }
    أرجع pv;
}

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    إذا(pv!=تش_اشاره) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساكن uint32_t *
پیدا_کړه_لیکه(UPropsVectors *pv, UChar32 range_پیل) {
    uint32_t *لیکه;
    int32_t columns, i, پیل, حد, prev_لیکه;

    columns=pv->columns;
    حد=pv->لیکې;
    prev_لیکه=pv->prev_لیکه;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    لیکه=pv->v+prev_لیکه*columns;
    إذا (range_پیل >= static_cast<UChar32>(لیکه[0])) {
        إذا (range_پیل < static_cast<UChar32>(لیکه[1])) {
            /* same row as last seen */
            أرجع لیکه;
        } وإلا إذا (range_پیل < static_cast<UChar32>((لیکه += columns)[1])) {
            /* next row after the last one */
            pv->prev_لیکه=prev_لیکه+1;
            أرجع لیکه;
        } وإلا إذا (range_پیل < static_cast<UChar32>((لیکه += columns)[1])) {
            /* second row after the last one */
            pv->prev_لیکه=prev_لیکه+2;
            أرجع لیکه;
        } وإلا إذا ((range_پیل - static_cast<UChar32>(لیکه[1])) < 10) {
            /* we are close, continue looping */
            prev_لیکه+=2;
            افعل {
                ++prev_لیکه;
                لیکه+=columns;
            } طالما (range_پیل >= static_cast<UChar32>(لیکه[1]));
            pv->prev_لیکه=prev_لیکه;
            أرجع لیکه;
        }
    } وإلا إذا (range_پیل < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_لیکه=0;
        أرجع pv->v;
    }

    /* do a binary search for the start of the range */
    پیل=0;
    طالما(پیل<حد-1) {
        i=(پیل+حد)/2;
        لیکه=pv->v+i*columns;
        إذا (range_پیل < static_cast<UChar32>(لیکه[0])) {
            حد=i;
        } وإلا إذا (range_پیل < static_cast<UChar32>(لیکه[1])) {
            pv->prev_لیکه=i;
            أرجع لیکه;
        } وإلا {
            پیل=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_لیکه=پیل;
    أرجع pv->v+پیل*columns;
}

U_CAPI فراغ U_EXPORT2
upvec_سېټ_ارزښت(UPropsVectors *pv,
               UChar32 پیل, UChar32 پای_ټکی,
               int32_t ستنه,
               uint32_t value, uint32_t mask,
               UError_کوډ *p_Error_کوډ) {
    uint32_t *لومړی_لیکه, *وروستی_لیکه;
    int32_t columns;
    UChar32 حد;
    UBool وېشل_لومړی_لیکه, وېشل_وروستی_لیکه;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا( pv==تش_اشاره ||
        پیل<0 || پیل>پای_ټکی || پای_ټکی>UPVEC_MAX_CP ||
        ستنه<0 || ستنه>=(pv->columns-2)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        *p_Error_کوډ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
    حد=پای_ټکی+1;

    /* initialize */
    columns=pv->columns;
    ستنه+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    لومړی_لیکه=پیدا_کړه_لیکه(pv, پیل);
    وروستی_لیکه=پیدا_کړه_لیکه(pv, پای_ټکی);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    وېشل_لومړی_لیکه = پیل != static_cast<UChar32>(لومړی_لیکه[0]) && value != (لومړی_لیکه[ستنه] & mask);
    وېشل_وروستی_لیکه = حد != static_cast<UChar32>(وروستی_لیکه[1]) && value != (وروستی_لیکه[ستنه] & mask);

    /* split first/last rows if necessary */
    إذا(وېشل_لومړی_لیکه || وېشل_وروستی_لیکه) {
        int32_t count, لیکې;

        لیکې=pv->لیکې;
        إذا((لیکې+وېشل_لومړی_لیکه+وېشل_وروستی_لیکه)>pv->max_لیکې) {
            uint32_t *newVectors;
            int32_t new_Max_لیکې;

            إذا(pv->max_لیکې<UPVEC_MEDIUM_لیکې) {
                new_Max_لیکې=UPVEC_MEDIUM_لیکې;
            } وإلا إذا(pv->max_لیکې<UPVEC_MAX_لیکې) {
                new_Max_لیکې=UPVEC_MAX_لیکې;
            } وإلا {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_کوډ=U_دننی_پروګرام_ERROR;
                أرجع;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_لیکې*columns*4);
            إذا(newVectors==تش_اشاره) {
                *p_Error_کوډ=U_حافظه_ویش_ERROR;
                أرجع;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)لیکې*columns*4);
            لومړی_لیکه=newVectors+(لومړی_لیکه-pv->v);
            وروستی_لیکه=newVectors+(وروستی_لیکه-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_لیکې=new_Max_لیکې;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+لیکې*columns)-(وروستی_لیکه+columns));
        إذا(count>0) {
            uprv_memmove(
                وروستی_لیکه+(1+وېشل_لومړی_لیکه+وېشل_وروستی_لیکه)*columns,
                وروستی_لیکه+columns,
                count*4);
        }
        pv->لیکې=لیکې+وېشل_لومړی_لیکه+وېشل_وروستی_لیکه;

        /* split the first row, and move the firstRow pointer to the second part */
        إذا(وېشل_لومړی_لیکه) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((وروستی_لیکه-لومړی_لیکه)+columns);
            uprv_memmove(لومړی_لیکه+columns, لومړی_لیکه, (size_t)count*4);
            وروستی_لیکه+=columns;

            /* split the range and move the firstRow pointer */
            لومړی_لیکه[1]=لومړی_لیکه[columns]=(uint32_t)پیل;
            لومړی_لیکه+=columns;
        }

        /* split the last row */
        إذا(وېشل_وروستی_لیکه) {
            /* copy the last row data */
            uprv_memcpy(وروستی_لیکه+columns, وروستی_لیکه, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            وروستی_لیکه[1]=وروستی_لیکه[columns]=(uint32_t)حد;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_لیکه=(int32_t)((وروستی_لیکه-(pv->v))/columns);

    /* set the input value in all remaining rows */
    لومړی_لیکه+=ستنه;
    وروستی_لیکه+=ستنه;
    mask=~mask;
    لكل(;;) {
        *لومړی_لیکه=(*لومړی_لیکه&mask)|value;
        إذا(لومړی_لیکه==وروستی_لیکه) {
            اكسر;
        }
        لومړی_لیکه+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_ارزښت(ثابت UPropsVectors *pv, UChar32 c, int32_t ستنه) {
    uint32_t *لیکه;
    UPropsVectors *ncpv;

    إذا(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || ستنه<0 || ستنه>=(pv->columns-2)) {
        أرجع 0;
    }
    ncpv=(UPropsVectors *)pv;
    لیکه=پیدا_کړه_لیکه(ncpv, c);
    أرجع لیکه[2+ستنه];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_لیکه(ثابت UPropsVectors *pv, int32_t لیکه_شاخص,
             UChar32 *p_Range_پیل, UChar32 *p_Range_پای_ټکی) {
    uint32_t *لیکه;
    int32_t columns;

    إذا(pv->isCompacted || لیکه_شاخص<0 || لیکه_شاخص>=pv->لیکې) {
        أرجع تش_اشاره;
    }

    columns=pv->columns;
    لیکه=pv->v+لیکه_شاخص*columns;
    إذا(p_Range_پیل!=تش_اشاره) {
        *p_Range_پیل=(UChar32)لیکه[0];
    }
    إذا(p_Range_پای_ټکی!=تش_اشاره) {
        *p_Range_پای_ټکی=(UChar32)لیکه[1]-1;
    }
    أرجع لیکه+2;
}

ساكن int32_t U_CALLCONV
upvec_compare_لیکې(ثابت فراغ *context, ثابت فراغ *l, ثابت فراغ *r) {
    ثابت uint32_t* کیڼ = static_cast<ثابت uint32_t*>(l), *ښی = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    افعل {
        إذا(کیڼ[i]!=ښی[i]) {
            أرجع کیڼ[i]<ښی[i] ? -1 : 1;
        }
        إذا(++i==columns) {
            i=0;
        }
    } طالما(--count>0);

    أرجع 0;
}

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UError_کوډ *p_Error_کوډ) {
    uint32_t *لیکه;
    int32_t i, columns, ارزښت_Columns, لیکې, count;
    UChar32 پیل, حد;

    /* argument checking */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا(handler==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(pv->isCompacted) {
        أرجع;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=ریښتیا;

    لیکې=pv->لیکې;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    ارزښت_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ترتیبول_Array(pv->v, لیکې, columns*4,
                   upvec_compare_لیکې, pv, خطأ, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    لیکه=pv->v;
    count=-ارزښت_Columns;
    لكل(i=0; i<لیکې; ++i) {
        پیل=(UChar32)لیکه[0];

        /* count a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(لیکه+2, لیکه-ارزښت_Columns, ارزښت_Columns*4)) {
            count+=ارزښت_Columns;
        }

        إذا(پیل>=UPVEC_لومړی_SPECIAL_CP) {
            handler(context, پیل, پیل, count, لیکه+2, ارزښت_Columns, p_Error_کوډ);
            إذا(U_FAILURE(*p_Error_کوډ)) {
                أرجع;
            }
        }

        لیکه+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=ارزښت_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_پیل_REAL_ارزښتونه_CP, UPVEC_پیل_REAL_ارزښتونه_CP,
            count, لیکه-ارزښت_Columns, ارزښت_Columns, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    لیکه=pv->v;
    count=-ارزښت_Columns;
    لكل(i=0; i<لیکې; ++i) {
        /* fetch these first before memmove() may overwrite them */
        پیل=(UChar32)لیکه[0];
        حد=(UChar32)لیکه[1];

        /* add a new values vector if it is different from the current one */
        إذا(count<0 || 0!=uprv_memcmp(لیکه+2, pv->v+count, ارزښت_Columns*4)) {
            count+=ارزښت_Columns;
            uprv_memmove(pv->v+count, لیکه+2, (size_t)ارزښت_Columns*4);
        }

        إذا(پیل<UPVEC_لومړی_SPECIAL_CP) {
            handler(context, پیل, حد-1, count, pv->v+count, ارزښت_Columns, p_Error_کوډ);
            إذا(U_FAILURE(*p_Error_کوډ)) {
                أرجع;
            }
        }

        لیکه+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->لیکې=count/ارزښت_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *p_لیکې, int32_t *pColumns) {
    إذا(!pv->isCompacted) {
        أرجع تش_اشاره;
    }
    إذا(p_لیکې!=تش_اشاره) {
        *p_لیکې=pv->لیکې;
    }
    إذا(pColumns!=تش_اشاره) {
        *pColumns=pv->columns-2;
    }
    أرجع pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *p_لیکې, int32_t *pColumns, UError_کوډ *p_Error_کوډ) {
    uint32_t *clonedArray;
    int32_t byte_اوږدتیا;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(!pv->isCompacted) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    byte_اوږدتیا=pv->لیکې*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_اوږدتیا);
    إذا(clonedArray==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(clonedArray, pv->v, byte_اوږدتیا);
    إذا(p_لیکې!=تش_اشاره) {
        *p_لیکې=pv->لیکې;
    }
    إذا(pColumns!=تش_اشاره) {
        *pColumns=pv->columns-2;
    }
    أرجع clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_منزل_UTrie2_With_لیکه_Indexes(UPropsVectors *pv, UError_کوډ *p_Error_کوډ) {
    UPVec_منزل_UTrie2_Context منزل_UTrie2={ تش_اشاره, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_منزل_UTrie2_Handler, &منزل_UTrie2, p_Error_کوډ);
    utrie2_freeze(منزل_UTrie2.trie, UTRIE2_16_ارزښت_بټونه, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        utrie2_close(منزل_UTrie2.trie);
        منزل_UTrie2.trie=تش_اشاره;
    }
    أرجع منزل_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI فراغ U_CALLCONV
upvec_compact_منزل_UTrie2_Handler(فراغ *context,
                             UChar32 پیل, UChar32 پای_ټکی,
                             int32_t لیکه_شاخص, uint32_t *لیکه, int32_t columns,
                             UError_کوډ *p_Error_کوډ) {
    (فراغ)لیکه;
    (فراغ)columns;
    UPVec_منزل_UTrie2_Context *منزل_UTrie2=(UPVec_منزل_UTrie2_Context *)context;
    إذا(پیل<UPVEC_لومړی_SPECIAL_CP) {
        utrie2_سېټ_Range32(منزل_UTrie2->trie, پیل, پای_ټکی, (uint32_t)لیکه_شاخص, ریښتیا, p_Error_کوډ);
    } وإلا {
        بدل(پیل) {
        حالة UPVEC_INITIAL_ارزښت_CP:
            منزل_UTrie2->initial_ارزښت=لیکه_شاخص;
            اكسر;
        حالة UPVEC_ERROR_ارزښت_CP:
            منزل_UTrie2->error_ارزښت=لیکه_شاخص;
            اكسر;
        حالة UPVEC_پیل_REAL_ارزښتونه_CP:
            منزل_UTrie2->max_ارزښت=لیکه_شاخص;
            إذا(لیکه_شاخص>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            } وإلا {
                منزل_UTrie2->trie=utrie2_open(منزل_UTrie2->initial_ارزښت,
                                           منزل_UTrie2->error_ارزښت, p_Error_کوډ);
            }
            اكسر;
        افتراضي:
            اكسر;
        }
    }
}
