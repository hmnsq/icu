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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "propsvec.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

ਬਣਤਰ UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_ਪੰਕਤੀਆਂ;
    int32_t ਪੰਕਤੀਆਂ;
    int32_t prev_ਪੰਕਤੀ;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPVEC_INITIAL_ਪੰਕਤੀਆਂ (1<<12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPVEC_MEDIUM_ਪੰਕਤੀਆਂ ((int32_t)1<<16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPVEC_MAX_ਪੰਕਤੀਆਂ (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UPropsVectors *pv;
    uint32_t *v, *ਪੰਕਤੀ;
    uint32_t cp;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(columns<1) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(ਆਕਾਰ(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ਪੰਕਤੀਆਂ*columns*4);
    ਜੇ(pv==ਖਾਲੀ_ਸੂਚਕ || v==ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memset(pv, 0, ਆਕਾਰ(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_ਪੰਕਤੀਆਂ=UPVEC_INITIAL_ਪੰਕਤੀਆਂ;
    pv->ਪੰਕਤੀਆਂ=2+(UPVEC_MAX_CP-UPVEC_ਪਹਿਲਾ_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    ਪੰਕਤੀ=pv->v;
    uprv_memset(ਪੰਕਤੀ, 0, pv->ਪੰਕਤੀਆਂ*columns*4);
    ਪੰਕਤੀ[0]=0;
    ਪੰਕਤੀ[1]=0x110000;
    ਪੰਕਤੀ+=columns;
    ਲਈ(cp=UPVEC_ਪਹਿਲਾ_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        ਪੰਕਤੀ[0]=cp;
        ਪੰਕਤੀ[1]=cp+1;
        ਪੰਕਤੀ+=columns;
    }
    ਵਾਪਸ pv;
}

U_CAPI ਖਾਲੀ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    ਜੇ(pv!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ਸਥਿਰ uint32_t *
ਲੱਭੋ_ਪੰਕਤੀ(UPropsVectors *pv, UChar32 range_ਸ਼ੁਰੂ) {
    uint32_t *ਪੰਕਤੀ;
    int32_t columns, i, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, prev_ਪੰਕਤੀ;

    columns=pv->columns;
    ਹੱਦਬੰਦੀ=pv->ਪੰਕਤੀਆਂ;
    prev_ਪੰਕਤੀ=pv->prev_ਪੰਕਤੀ;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    ਪੰਕਤੀ=pv->v+prev_ਪੰਕਤੀ*columns;
    ਜੇ (range_ਸ਼ੁਰੂ >= static_cast<UChar32>(ਪੰਕਤੀ[0])) {
        ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>(ਪੰਕਤੀ[1])) {
            /* same row as last seen */
            ਵਾਪਸ ਪੰਕਤੀ;
        } ਨਹੀਂਤਾਂ ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>((ਪੰਕਤੀ += columns)[1])) {
            /* next row after the last one */
            pv->prev_ਪੰਕਤੀ=prev_ਪੰਕਤੀ+1;
            ਵਾਪਸ ਪੰਕਤੀ;
        } ਨਹੀਂਤਾਂ ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>((ਪੰਕਤੀ += columns)[1])) {
            /* second row after the last one */
            pv->prev_ਪੰਕਤੀ=prev_ਪੰਕਤੀ+2;
            ਵਾਪਸ ਪੰਕਤੀ;
        } ਨਹੀਂਤਾਂ ਜੇ ((range_ਸ਼ੁਰੂ - static_cast<UChar32>(ਪੰਕਤੀ[1])) < 10) {
            /* we are close, continue looping */
            prev_ਪੰਕਤੀ+=2;
            ਕਰੋ {
                ++prev_ਪੰਕਤੀ;
                ਪੰਕਤੀ+=columns;
            } ਜਦੋਂ (range_ਸ਼ੁਰੂ >= static_cast<UChar32>(ਪੰਕਤੀ[1]));
            pv->prev_ਪੰਕਤੀ=prev_ਪੰਕਤੀ;
            ਵਾਪਸ ਪੰਕਤੀ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_ਪੰਕਤੀ=0;
        ਵਾਪਸ pv->v;
    }

    /* do a binary search for the start of the range */
    ਸ਼ੁਰੂ=0;
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ-1) {
        i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        ਪੰਕਤੀ=pv->v+i*columns;
        ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>(ਪੰਕਤੀ[0])) {
            ਹੱਦਬੰਦੀ=i;
        } ਨਹੀਂਤਾਂ ਜੇ (range_ਸ਼ੁਰੂ < static_cast<UChar32>(ਪੰਕਤੀ[1])) {
            pv->prev_ਪੰਕਤੀ=i;
            ਵਾਪਸ ਪੰਕਤੀ;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_ਪੰਕਤੀ=ਸ਼ੁਰੂ;
    ਵਾਪਸ pv->v+ਸ਼ੁਰੂ*columns;
}

U_CAPI ਖਾਲੀ U_EXPORT2
upvec_ਸੈੱਟ_ਮੁੱਲ(UPropsVectors *pv,
               UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
               int32_t ਕਾਲਮ,
               uint32_t value, uint32_t mask,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t *ਪਹਿਲਾ_ਪੰਕਤੀ, *ਆਖਰੀ_ਪੰਕਤੀ;
    int32_t columns;
    UChar32 ਹੱਦਬੰਦੀ;
    UBool ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ, ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ;

    /* argument checking */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ( pv==ਖਾਲੀ_ਸੂਚਕ ||
        ਸ਼ੁਰੂ<0 || ਸ਼ੁਰੂ>ਸਿਰਾ || ਸਿਰਾ>UPVEC_MAX_CP ||
        ਕਾਲਮ<0 || ਕਾਲਮ>=(pv->columns-2)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ(pv->isCompacted) {
        *p_Error_ਕੋਡ=U_NO_WRITE_PERMISSION;
        ਵਾਪਸ;
    }
    ਹੱਦਬੰਦੀ=ਸਿਰਾ+1;

    /* initialize */
    columns=pv->columns;
    ਕਾਲਮ+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    ਪਹਿਲਾ_ਪੰਕਤੀ=ਲੱਭੋ_ਪੰਕਤੀ(pv, ਸ਼ੁਰੂ);
    ਆਖਰੀ_ਪੰਕਤੀ=ਲੱਭੋ_ਪੰਕਤੀ(pv, ਸਿਰਾ);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ = ਸ਼ੁਰੂ != static_cast<UChar32>(ਪਹਿਲਾ_ਪੰਕਤੀ[0]) && value != (ਪਹਿਲਾ_ਪੰਕਤੀ[ਕਾਲਮ] & mask);
    ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ = ਹੱਦਬੰਦੀ != static_cast<UChar32>(ਆਖਰੀ_ਪੰਕਤੀ[1]) && value != (ਆਖਰੀ_ਪੰਕਤੀ[ਕਾਲਮ] & mask);

    /* split first/last rows if necessary */
    ਜੇ(ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ || ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ) {
        int32_t count, ਪੰਕਤੀਆਂ;

        ਪੰਕਤੀਆਂ=pv->ਪੰਕਤੀਆਂ;
        ਜੇ((ਪੰਕਤੀਆਂ+ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ+ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ)>pv->max_ਪੰਕਤੀਆਂ) {
            uint32_t *newVectors;
            int32_t new_Max_ਪੰਕਤੀਆਂ;

            ਜੇ(pv->max_ਪੰਕਤੀਆਂ<UPVEC_MEDIUM_ਪੰਕਤੀਆਂ) {
                new_Max_ਪੰਕਤੀਆਂ=UPVEC_MEDIUM_ਪੰਕਤੀਆਂ;
            } ਨਹੀਂਤਾਂ ਜੇ(pv->max_ਪੰਕਤੀਆਂ<UPVEC_MAX_ਪੰਕਤੀਆਂ) {
                new_Max_ਪੰਕਤੀਆਂ=UPVEC_MAX_ਪੰਕਤੀਆਂ;
            } ਨਹੀਂਤਾਂ {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_ਕੋਡ=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                ਵਾਪਸ;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_ਪੰਕਤੀਆਂ*columns*4);
            ਜੇ(newVectors==ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)ਪੰਕਤੀਆਂ*columns*4);
            ਪਹਿਲਾ_ਪੰਕਤੀ=newVectors+(ਪਹਿਲਾ_ਪੰਕਤੀ-pv->v);
            ਆਖਰੀ_ਪੰਕਤੀ=newVectors+(ਆਖਰੀ_ਪੰਕਤੀ-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_ਪੰਕਤੀਆਂ=new_Max_ਪੰਕਤੀਆਂ;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+ਪੰਕਤੀਆਂ*columns)-(ਆਖਰੀ_ਪੰਕਤੀ+columns));
        ਜੇ(count>0) {
            uprv_memmove(
                ਆਖਰੀ_ਪੰਕਤੀ+(1+ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ+ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ)*columns,
                ਆਖਰੀ_ਪੰਕਤੀ+columns,
                count*4);
        }
        pv->ਪੰਕਤੀਆਂ=ਪੰਕਤੀਆਂ+ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ+ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ;

        /* split the first row, and move the firstRow pointer to the second part */
        ਜੇ(ਵੰਡੋ_ਪਹਿਲਾ_ਪੰਕਤੀ) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((ਆਖਰੀ_ਪੰਕਤੀ-ਪਹਿਲਾ_ਪੰਕਤੀ)+columns);
            uprv_memmove(ਪਹਿਲਾ_ਪੰਕਤੀ+columns, ਪਹਿਲਾ_ਪੰਕਤੀ, (size_t)count*4);
            ਆਖਰੀ_ਪੰਕਤੀ+=columns;

            /* split the range and move the firstRow pointer */
            ਪਹਿਲਾ_ਪੰਕਤੀ[1]=ਪਹਿਲਾ_ਪੰਕਤੀ[columns]=(uint32_t)ਸ਼ੁਰੂ;
            ਪਹਿਲਾ_ਪੰਕਤੀ+=columns;
        }

        /* split the last row */
        ਜੇ(ਵੰਡੋ_ਆਖਰੀ_ਪੰਕਤੀ) {
            /* copy the last row data */
            uprv_memcpy(ਆਖਰੀ_ਪੰਕਤੀ+columns, ਆਖਰੀ_ਪੰਕਤੀ, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            ਆਖਰੀ_ਪੰਕਤੀ[1]=ਆਖਰੀ_ਪੰਕਤੀ[columns]=(uint32_t)ਹੱਦਬੰਦੀ;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_ਪੰਕਤੀ=(int32_t)((ਆਖਰੀ_ਪੰਕਤੀ-(pv->v))/columns);

    /* set the input value in all remaining rows */
    ਪਹਿਲਾ_ਪੰਕਤੀ+=ਕਾਲਮ;
    ਆਖਰੀ_ਪੰਕਤੀ+=ਕਾਲਮ;
    mask=~mask;
    ਲਈ(;;) {
        *ਪਹਿਲਾ_ਪੰਕਤੀ=(*ਪਹਿਲਾ_ਪੰਕਤੀ&mask)|value;
        ਜੇ(ਪਹਿਲਾ_ਪੰਕਤੀ==ਆਖਰੀ_ਪੰਕਤੀ) {
            ਰੋਕੋ;
        }
        ਪਹਿਲਾ_ਪੰਕਤੀ+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_ਮੁੱਲ(ਅਚਲ UPropsVectors *pv, UChar32 c, int32_t ਕਾਲਮ) {
    uint32_t *ਪੰਕਤੀ;
    UPropsVectors *ncpv;

    ਜੇ(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || ਕਾਲਮ<0 || ਕਾਲਮ>=(pv->columns-2)) {
        ਵਾਪਸ 0;
    }
    ncpv=(UPropsVectors *)pv;
    ਪੰਕਤੀ=ਲੱਭੋ_ਪੰਕਤੀ(ncpv, c);
    ਵਾਪਸ ਪੰਕਤੀ[2+ਕਾਲਮ];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_ਪੰਕਤੀ(ਅਚਲ UPropsVectors *pv, int32_t ਪੰਕਤੀ_ਸੂਚਕਾਂਕ,
             UChar32 *p_Range_ਸ਼ੁਰੂ, UChar32 *p_Range_ਸਿਰਾ) {
    uint32_t *ਪੰਕਤੀ;
    int32_t columns;

    ਜੇ(pv->isCompacted || ਪੰਕਤੀ_ਸੂਚਕਾਂਕ<0 || ਪੰਕਤੀ_ਸੂਚਕਾਂਕ>=pv->ਪੰਕਤੀਆਂ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    columns=pv->columns;
    ਪੰਕਤੀ=pv->v+ਪੰਕਤੀ_ਸੂਚਕਾਂਕ*columns;
    ਜੇ(p_Range_ਸ਼ੁਰੂ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Range_ਸ਼ੁਰੂ=(UChar32)ਪੰਕਤੀ[0];
    }
    ਜੇ(p_Range_ਸਿਰਾ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Range_ਸਿਰਾ=(UChar32)ਪੰਕਤੀ[1]-1;
    }
    ਵਾਪਸ ਪੰਕਤੀ+2;
}

ਸਥਿਰ int32_t U_CALLCONV
upvec_compare_ਪੰਕਤੀਆਂ(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *l, ਅਚਲ ਖਾਲੀ *r) {
    ਅਚਲ uint32_t* ਖੱਬੇ = static_cast<ਅਚਲ uint32_t*>(l), *ਸੱਜੇ = static_cast<ਅਚਲ uint32_t*>(r);
    ਅਚਲ UPropsVectors* pv = static_cast<ਅਚਲ UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    ਕਰੋ {
        ਜੇ(ਖੱਬੇ[i]!=ਸੱਜੇ[i]) {
            ਵਾਪਸ ਖੱਬੇ[i]<ਸੱਜੇ[i] ? -1 : 1;
        }
        ਜੇ(++i==columns) {
            i=0;
        }
    } ਜਦੋਂ(--count>0);

    ਵਾਪਸ 0;
}

U_CAPI ਖਾਲੀ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ਖਾਲੀ *context, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t *ਪੰਕਤੀ;
    int32_t i, columns, ਮੁੱਲ_Columns, ਪੰਕਤੀਆਂ, count;
    UChar32 ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;

    /* argument checking */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(handler==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ(pv->isCompacted) {
        ਵਾਪਸ;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=ਸੱਚ;

    ਪੰਕਤੀਆਂ=pv->ਪੰਕਤੀਆਂ;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    ਮੁੱਲ_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ਛਾਂਟੋ_Array(pv->v, ਪੰਕਤੀਆਂ, columns*4,
                   upvec_compare_ਪੰਕਤੀਆਂ, pv, ਝੂਠ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    ਪੰਕਤੀ=pv->v;
    count=-ਮੁੱਲ_Columns;
    ਲਈ(i=0; i<ਪੰਕਤੀਆਂ; ++i) {
        ਸ਼ੁਰੂ=(UChar32)ਪੰਕਤੀ[0];

        /* count a new values vector if it is different from the current one */
        ਜੇ(count<0 || 0!=uprv_memcmp(ਪੰਕਤੀ+2, ਪੰਕਤੀ-ਮੁੱਲ_Columns, ਮੁੱਲ_Columns*4)) {
            count+=ਮੁੱਲ_Columns;
        }

        ਜੇ(ਸ਼ੁਰੂ>=UPVEC_ਪਹਿਲਾ_SPECIAL_CP) {
            handler(context, ਸ਼ੁਰੂ, ਸ਼ੁਰੂ, count, ਪੰਕਤੀ+2, ਮੁੱਲ_Columns, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
        }

        ਪੰਕਤੀ+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=ਮੁੱਲ_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_ਸ਼ੁਰੂ_REAL_ਮੁੱਲਾਂ_CP, UPVEC_ਸ਼ੁਰੂ_REAL_ਮੁੱਲਾਂ_CP,
            count, ਪੰਕਤੀ-ਮੁੱਲ_Columns, ਮੁੱਲ_Columns, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    ਪੰਕਤੀ=pv->v;
    count=-ਮੁੱਲ_Columns;
    ਲਈ(i=0; i<ਪੰਕਤੀਆਂ; ++i) {
        /* fetch these first before memmove() may overwrite them */
        ਸ਼ੁਰੂ=(UChar32)ਪੰਕਤੀ[0];
        ਹੱਦਬੰਦੀ=(UChar32)ਪੰਕਤੀ[1];

        /* add a new values vector if it is different from the current one */
        ਜੇ(count<0 || 0!=uprv_memcmp(ਪੰਕਤੀ+2, pv->v+count, ਮੁੱਲ_Columns*4)) {
            count+=ਮੁੱਲ_Columns;
            uprv_memmove(pv->v+count, ਪੰਕਤੀ+2, (size_t)ਮੁੱਲ_Columns*4);
        }

        ਜੇ(ਸ਼ੁਰੂ<UPVEC_ਪਹਿਲਾ_SPECIAL_CP) {
            handler(context, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-1, count, pv->v+count, ਮੁੱਲ_Columns, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
        }

        ਪੰਕਤੀ+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->ਪੰਕਤੀਆਂ=count/ਮੁੱਲ_Columns+1;
}

U_CAPI ਅਚਲ uint32_t * U_EXPORT2
upvec_getArray(ਅਚਲ UPropsVectors *pv, int32_t *p_ਪੰਕਤੀਆਂ, int32_t *pColumns) {
    ਜੇ(!pv->isCompacted) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(p_ਪੰਕਤੀਆਂ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਪੰਕਤੀਆਂ=pv->ਪੰਕਤੀਆਂ;
    }
    ਜੇ(pColumns!=ਖਾਲੀ_ਸੂਚਕ) {
        *pColumns=pv->columns-2;
    }
    ਵਾਪਸ pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ਅਚਲ UPropsVectors *pv,
                 int32_t *p_ਪੰਕਤੀਆਂ, int32_t *pColumns, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t *clonedArray;
    int32_t byte_ਲੰਮਾਈ;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(!pv->isCompacted) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    byte_ਲੰਮਾਈ=pv->ਪੰਕਤੀਆਂ*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_ਲੰਮਾਈ);
    ਜੇ(clonedArray==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(clonedArray, pv->v, byte_ਲੰਮਾਈ);
    ਜੇ(p_ਪੰਕਤੀਆਂ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਪੰਕਤੀਆਂ=pv->ਪੰਕਤੀਆਂ;
    }
    ਜੇ(pColumns!=ਖਾਲੀ_ਸੂਚਕ) {
        *pColumns=pv->columns-2;
    }
    ਵਾਪਸ clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_ਮੰਜ਼ਿਲ_UTrie2_With_ਪੰਕਤੀ_Indexes(UPropsVectors *pv, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UPVec_ਮੰਜ਼ਿਲ_UTrie2_Context ਮੰਜ਼ਿਲ_UTrie2={ ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_ਮੰਜ਼ਿਲ_UTrie2_Handler, &ਮੰਜ਼ਿਲ_UTrie2, p_Error_ਕੋਡ);
    utrie2_freeze(ਮੰਜ਼ਿਲ_UTrie2.trie, UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        utrie2_close(ਮੰਜ਼ਿਲ_UTrie2.trie);
        ਮੰਜ਼ਿਲ_UTrie2.trie=ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਮੰਜ਼ਿਲ_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI ਖਾਲੀ U_CALLCONV
upvec_compact_ਮੰਜ਼ਿਲ_UTrie2_Handler(ਖਾਲੀ *context,
                             UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
                             int32_t ਪੰਕਤੀ_ਸੂਚਕਾਂਕ, uint32_t *ਪੰਕਤੀ, int32_t columns,
                             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)ਪੰਕਤੀ;
    (ਖਾਲੀ)columns;
    UPVec_ਮੰਜ਼ਿਲ_UTrie2_Context *ਮੰਜ਼ਿਲ_UTrie2=(UPVec_ਮੰਜ਼ਿਲ_UTrie2_Context *)context;
    ਜੇ(ਸ਼ੁਰੂ<UPVEC_ਪਹਿਲਾ_SPECIAL_CP) {
        utrie2_ਸੈੱਟ_Range32(ਮੰਜ਼ਿਲ_UTrie2->trie, ਸ਼ੁਰੂ, ਸਿਰਾ, (uint32_t)ਪੰਕਤੀ_ਸੂਚਕਾਂਕ, ਸੱਚ, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ(ਸ਼ੁਰੂ) {
        ਹਾਲਤ UPVEC_INITIAL_ਮੁੱਲ_CP:
            ਮੰਜ਼ਿਲ_UTrie2->initial_ਮੁੱਲ=ਪੰਕਤੀ_ਸੂਚਕਾਂਕ;
            ਰੋਕੋ;
        ਹਾਲਤ UPVEC_ERROR_ਮੁੱਲ_CP:
            ਮੰਜ਼ਿਲ_UTrie2->error_ਮੁੱਲ=ਪੰਕਤੀ_ਸੂਚਕਾਂਕ;
            ਰੋਕੋ;
        ਹਾਲਤ UPVEC_ਸ਼ੁਰੂ_REAL_ਮੁੱਲਾਂ_CP:
            ਮੰਜ਼ਿਲ_UTrie2->max_ਮੁੱਲ=ਪੰਕਤੀ_ਸੂਚਕਾਂਕ;
            ਜੇ(ਪੰਕਤੀ_ਸੂਚਕਾਂਕ>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            } ਨਹੀਂਤਾਂ {
                ਮੰਜ਼ਿਲ_UTrie2->trie=utrie2_open(ਮੰਜ਼ਿਲ_UTrie2->initial_ਮੁੱਲ,
                                           ਮੰਜ਼ਿਲ_UTrie2->error_ਮੁੱਲ, p_Error_ਕੋਡ);
            }
            ਰੋਕੋ;
        ਮੂਲ:
            ਰੋਕੋ;
        }
    }
}
