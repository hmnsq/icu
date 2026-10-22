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

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "propsvec.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

ರಚನೆ UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_ಅಡ್ಡಸಾಲುಗಳು;
    int32_t ಅಡ್ಡಸಾಲುಗಳು;
    int32_t prev_ಅಡ್ಡಸಾಲು;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_INITIAL_ಅಡ್ಡಸಾಲುಗಳು (1<<12)
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_MEDIUM_ಅಡ್ಡಸಾಲುಗಳು ((int32_t)1<<16)
#ವ್ಯಾಖ್ಯಾನಿಸು UPVEC_MAX_ಅಡ್ಡಸಾಲುಗಳು (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UPropsVectors *pv;
    uint32_t *v, *ಅಡ್ಡಸಾಲು;
    uint32_t cp;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(columns<1) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(ಗಾತ್ರ(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ಅಡ್ಡಸಾಲುಗಳು*columns*4);
    ಆದರೆ(pv==ಶೂನ್ಯಸೂಚಕ || v==ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memset(pv, 0, ಗಾತ್ರ(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_ಅಡ್ಡಸಾಲುಗಳು=UPVEC_INITIAL_ಅಡ್ಡಸಾಲುಗಳು;
    pv->ಅಡ್ಡಸಾಲುಗಳು=2+(UPVEC_MAX_CP-UPVEC_ಮೊದಲ_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    ಅಡ್ಡಸಾಲು=pv->v;
    uprv_memset(ಅಡ್ಡಸಾಲು, 0, pv->ಅಡ್ಡಸಾಲುಗಳು*columns*4);
    ಅಡ್ಡಸಾಲು[0]=0;
    ಅಡ್ಡಸಾಲು[1]=0x110000;
    ಅಡ್ಡಸಾಲು+=columns;
    ಗಾಗಿ(cp=UPVEC_ಮೊದಲ_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        ಅಡ್ಡಸಾಲು[0]=cp;
        ಅಡ್ಡಸಾಲು[1]=cp+1;
        ಅಡ್ಡಸಾಲು+=columns;
    }
    ಹಿಂತಿರುಗಿ pv;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    ಆದರೆ(pv!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ಸ್ಥಿರ uint32_t *
ಕಂಡುಹಿಡಿ_ಅಡ್ಡಸಾಲು(UPropsVectors *pv, UChar32 range_ಆರಂಭ) {
    uint32_t *ಅಡ್ಡಸಾಲು;
    int32_t columns, i, ಆರಂಭ, ಎಲ್ಲೆ, prev_ಅಡ್ಡಸಾಲು;

    columns=pv->columns;
    ಎಲ್ಲೆ=pv->ಅಡ್ಡಸಾಲುಗಳು;
    prev_ಅಡ್ಡಸಾಲು=pv->prev_ಅಡ್ಡಸಾಲು;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    ಅಡ್ಡಸಾಲು=pv->v+prev_ಅಡ್ಡಸಾಲು*columns;
    ಆದರೆ (range_ಆರಂಭ >= static_cast<UChar32>(ಅಡ್ಡಸಾಲು[0])) {
        ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>(ಅಡ್ಡಸಾಲು[1])) {
            /* same row as last seen */
            ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>((ಅಡ್ಡಸಾಲು += columns)[1])) {
            /* next row after the last one */
            pv->prev_ಅಡ್ಡಸಾಲು=prev_ಅಡ್ಡಸಾಲು+1;
            ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>((ಅಡ್ಡಸಾಲು += columns)[1])) {
            /* second row after the last one */
            pv->prev_ಅಡ್ಡಸಾಲು=prev_ಅಡ್ಡಸಾಲು+2;
            ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((range_ಆರಂಭ - static_cast<UChar32>(ಅಡ್ಡಸಾಲು[1])) < 10) {
            /* we are close, continue looping */
            prev_ಅಡ್ಡಸಾಲು+=2;
            ಮಾಡು {
                ++prev_ಅಡ್ಡಸಾಲು;
                ಅಡ್ಡಸಾಲು+=columns;
            } ಆಗ (range_ಆರಂಭ >= static_cast<UChar32>(ಅಡ್ಡಸಾಲು[1]));
            pv->prev_ಅಡ್ಡಸಾಲು=prev_ಅಡ್ಡಸಾಲು;
            ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_ಅಡ್ಡಸಾಲು=0;
        ಹಿಂತಿರುಗಿ pv->v;
    }

    /* do a binary search for the start of the range */
    ಆರಂಭ=0;
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ-1) {
        i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        ಅಡ್ಡಸಾಲು=pv->v+i*columns;
        ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>(ಅಡ್ಡಸಾಲು[0])) {
            ಎಲ್ಲೆ=i;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range_ಆರಂಭ < static_cast<UChar32>(ಅಡ್ಡಸಾಲು[1])) {
            pv->prev_ಅಡ್ಡಸಾಲು=i;
            ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_ಅಡ್ಡಸಾಲು=ಆರಂಭ;
    ಹಿಂತಿರುಗಿ pv->v+ಆರಂಭ*columns;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_ಗಣ_ಮೌಲ್ಯ(UPropsVectors *pv,
               UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
               int32_t ಸ್ತಂಭ,
               uint32_t value, uint32_t mask,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t *ಮೊದಲ_ಅಡ್ಡಸಾಲು, *ಕೊನೆಯ_ಅಡ್ಡಸಾಲು;
    int32_t columns;
    UChar32 ಎಲ್ಲೆ;
    UBool ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು, ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು;

    /* argument checking */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ( pv==ಶೂನ್ಯಸೂಚಕ ||
        ಆರಂಭ<0 || ಆರಂಭ>ಕೊನೆ || ಕೊನೆ>UPVEC_MAX_CP ||
        ಸ್ತಂಭ<0 || ಸ್ತಂಭ>=(pv->columns-2)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(pv->isCompacted) {
        *p_Error_ಸಂಕೇತ=U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ;
    }
    ಎಲ್ಲೆ=ಕೊನೆ+1;

    /* initialize */
    columns=pv->columns;
    ಸ್ತಂಭ+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    ಮೊದಲ_ಅಡ್ಡಸಾಲು=ಕಂಡುಹಿಡಿ_ಅಡ್ಡಸಾಲು(pv, ಆರಂಭ);
    ಕೊನೆಯ_ಅಡ್ಡಸಾಲು=ಕಂಡುಹಿಡಿ_ಅಡ್ಡಸಾಲು(pv, ಕೊನೆ);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು = ಆರಂಭ != static_cast<UChar32>(ಮೊದಲ_ಅಡ್ಡಸಾಲು[0]) && value != (ಮೊದಲ_ಅಡ್ಡಸಾಲು[ಸ್ತಂಭ] & mask);
    ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು = ಎಲ್ಲೆ != static_cast<UChar32>(ಕೊನೆಯ_ಅಡ್ಡಸಾಲು[1]) && value != (ಕೊನೆಯ_ಅಡ್ಡಸಾಲು[ಸ್ತಂಭ] & mask);

    /* split first/last rows if necessary */
    ಆದರೆ(ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು || ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು) {
        int32_t count, ಅಡ್ಡಸಾಲುಗಳು;

        ಅಡ್ಡಸಾಲುಗಳು=pv->ಅಡ್ಡಸಾಲುಗಳು;
        ಆದರೆ((ಅಡ್ಡಸಾಲುಗಳು+ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು+ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು)>pv->max_ಅಡ್ಡಸಾಲುಗಳು) {
            uint32_t *newVectors;
            int32_t new_Max_ಅಡ್ಡಸಾಲುಗಳು;

            ಆದರೆ(pv->max_ಅಡ್ಡಸಾಲುಗಳು<UPVEC_MEDIUM_ಅಡ್ಡಸಾಲುಗಳು) {
                new_Max_ಅಡ್ಡಸಾಲುಗಳು=UPVEC_MEDIUM_ಅಡ್ಡಸಾಲುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pv->max_ಅಡ್ಡಸಾಲುಗಳು<UPVEC_MAX_ಅಡ್ಡಸಾಲುಗಳು) {
                new_Max_ಅಡ್ಡಸಾಲುಗಳು=UPVEC_MAX_ಅಡ್ಡಸಾಲುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_ಸಂಕೇತ=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_ಅಡ್ಡಸಾಲುಗಳು*columns*4);
            ಆದರೆ(newVectors==ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)ಅಡ್ಡಸಾಲುಗಳು*columns*4);
            ಮೊದಲ_ಅಡ್ಡಸಾಲು=newVectors+(ಮೊದಲ_ಅಡ್ಡಸಾಲು-pv->v);
            ಕೊನೆಯ_ಅಡ್ಡಸಾಲು=newVectors+(ಕೊನೆಯ_ಅಡ್ಡಸಾಲು-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_ಅಡ್ಡಸಾಲುಗಳು=new_Max_ಅಡ್ಡಸಾಲುಗಳು;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+ಅಡ್ಡಸಾಲುಗಳು*columns)-(ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+columns));
        ಆದರೆ(count>0) {
            uprv_memmove(
                ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+(1+ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು+ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು)*columns,
                ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+columns,
                count*4);
        }
        pv->ಅಡ್ಡಸಾಲುಗಳು=ಅಡ್ಡಸಾಲುಗಳು+ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು+ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು;

        /* split the first row, and move the firstRow pointer to the second part */
        ಆದರೆ(ವಿಭಜಿಸು_ಮೊದಲ_ಅಡ್ಡಸಾಲು) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((ಕೊನೆಯ_ಅಡ್ಡಸಾಲು-ಮೊದಲ_ಅಡ್ಡಸಾಲು)+columns);
            uprv_memmove(ಮೊದಲ_ಅಡ್ಡಸಾಲು+columns, ಮೊದಲ_ಅಡ್ಡಸಾಲು, (size_t)count*4);
            ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+=columns;

            /* split the range and move the firstRow pointer */
            ಮೊದಲ_ಅಡ್ಡಸಾಲು[1]=ಮೊದಲ_ಅಡ್ಡಸಾಲು[columns]=(uint32_t)ಆರಂಭ;
            ಮೊದಲ_ಅಡ್ಡಸಾಲು+=columns;
        }

        /* split the last row */
        ಆದರೆ(ವಿಭಜಿಸು_ಕೊನೆಯ_ಅಡ್ಡಸಾಲು) {
            /* copy the last row data */
            uprv_memcpy(ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+columns, ಕೊನೆಯ_ಅಡ್ಡಸಾಲು, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            ಕೊನೆಯ_ಅಡ್ಡಸಾಲು[1]=ಕೊನೆಯ_ಅಡ್ಡಸಾಲು[columns]=(uint32_t)ಎಲ್ಲೆ;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_ಅಡ್ಡಸಾಲು=(int32_t)((ಕೊನೆಯ_ಅಡ್ಡಸಾಲು-(pv->v))/columns);

    /* set the input value in all remaining rows */
    ಮೊದಲ_ಅಡ್ಡಸಾಲು+=ಸ್ತಂಭ;
    ಕೊನೆಯ_ಅಡ್ಡಸಾಲು+=ಸ್ತಂಭ;
    mask=~mask;
    ಗಾಗಿ(;;) {
        *ಮೊದಲ_ಅಡ್ಡಸಾಲು=(*ಮೊದಲ_ಅಡ್ಡಸಾಲು&mask)|value;
        ಆದರೆ(ಮೊದಲ_ಅಡ್ಡಸಾಲು==ಕೊನೆಯ_ಅಡ್ಡಸಾಲು) {
            ನಿಲ್ಲಿಸು;
        }
        ಮೊದಲ_ಅಡ್ಡಸಾಲು+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, UChar32 c, int32_t ಸ್ತಂಭ) {
    uint32_t *ಅಡ್ಡಸಾಲು;
    UPropsVectors *ncpv;

    ಆದರೆ(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || ಸ್ತಂಭ<0 || ಸ್ತಂಭ>=(pv->columns-2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ncpv=(UPropsVectors *)pv;
    ಅಡ್ಡಸಾಲು=ಕಂಡುಹಿಡಿ_ಅಡ್ಡಸಾಲು(ncpv, c);
    ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು[2+ಸ್ತಂಭ];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_ಅಡ್ಡಸಾಲು(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, int32_t ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ,
             UChar32 *p_Range_ಆರಂಭ, UChar32 *p_Range_ಕೊನೆ) {
    uint32_t *ಅಡ್ಡಸಾಲು;
    int32_t columns;

    ಆದರೆ(pv->isCompacted || ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ<0 || ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ>=pv->ಅಡ್ಡಸಾಲುಗಳು) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    columns=pv->columns;
    ಅಡ್ಡಸಾಲು=pv->v+ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ*columns;
    ಆದರೆ(p_Range_ಆರಂಭ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Range_ಆರಂಭ=(UChar32)ಅಡ್ಡಸಾಲು[0];
    }
    ಆದರೆ(p_Range_ಕೊನೆ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Range_ಕೊನೆ=(UChar32)ಅಡ್ಡಸಾಲು[1]-1;
    }
    ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು+2;
}

ಸ್ಥಿರ int32_t U_CALLCONV
upvec_compare_ಅಡ್ಡಸಾಲುಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *l, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *r) {
    ಸ್ಥಿರಾಂಕ uint32_t* ಎಡ = static_cast<ಸ್ಥಿರಾಂಕ uint32_t*>(l), *ಬಲ = static_cast<ಸ್ಥಿರಾಂಕ uint32_t*>(r);
    ಸ್ಥಿರಾಂಕ UPropsVectors* pv = static_cast<ಸ್ಥಿರಾಂಕ UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    ಮಾಡು {
        ಆದರೆ(ಎಡ[i]!=ಬಲ[i]) {
            ಹಿಂತಿರುಗಿ ಎಡ[i]<ಬಲ[i] ? -1 : 1;
        }
        ಆದರೆ(++i==columns) {
            i=0;
        }
    } ಆಗ(--count>0);

    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ಶೂನ್ಯ *context, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t *ಅಡ್ಡಸಾಲು;
    int32_t i, columns, ಮೌಲ್ಯ_Columns, ಅಡ್ಡಸಾಲುಗಳು, count;
    UChar32 ಆರಂಭ, ಎಲ್ಲೆ;

    /* argument checking */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(handler==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(pv->isCompacted) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=ನಿಜ;

    ಅಡ್ಡಸಾಲುಗಳು=pv->ಅಡ್ಡಸಾಲುಗಳು;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    ಮೌಲ್ಯ_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_ವಿಂಗಡಿಸು_Array(pv->v, ಅಡ್ಡಸಾಲುಗಳು, columns*4,
                   upvec_compare_ಅಡ್ಡಸಾಲುಗಳು, pv, ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    ಅಡ್ಡಸಾಲು=pv->v;
    count=-ಮೌಲ್ಯ_Columns;
    ಗಾಗಿ(i=0; i<ಅಡ್ಡಸಾಲುಗಳು; ++i) {
        ಆರಂಭ=(UChar32)ಅಡ್ಡಸಾಲು[0];

        /* count a new values vector if it is different from the current one */
        ಆದರೆ(count<0 || 0!=uprv_memcmp(ಅಡ್ಡಸಾಲು+2, ಅಡ್ಡಸಾಲು-ಮೌಲ್ಯ_Columns, ಮೌಲ್ಯ_Columns*4)) {
            count+=ಮೌಲ್ಯ_Columns;
        }

        ಆದರೆ(ಆರಂಭ>=UPVEC_ಮೊದಲ_SPECIAL_CP) {
            handler(context, ಆರಂಭ, ಆರಂಭ, count, ಅಡ್ಡಸಾಲು+2, ಮೌಲ್ಯ_Columns, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
        }

        ಅಡ್ಡಸಾಲು+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=ಮೌಲ್ಯ_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_ಆರಂಭ_REAL_ಮೌಲ್ಯಗಳು_CP, UPVEC_ಆರಂಭ_REAL_ಮೌಲ್ಯಗಳು_CP,
            count, ಅಡ್ಡಸಾಲು-ಮೌಲ್ಯ_Columns, ಮೌಲ್ಯ_Columns, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    ಅಡ್ಡಸಾಲು=pv->v;
    count=-ಮೌಲ್ಯ_Columns;
    ಗಾಗಿ(i=0; i<ಅಡ್ಡಸಾಲುಗಳು; ++i) {
        /* fetch these first before memmove() may overwrite them */
        ಆರಂಭ=(UChar32)ಅಡ್ಡಸಾಲು[0];
        ಎಲ್ಲೆ=(UChar32)ಅಡ್ಡಸಾಲು[1];

        /* add a new values vector if it is different from the current one */
        ಆದರೆ(count<0 || 0!=uprv_memcmp(ಅಡ್ಡಸಾಲು+2, pv->v+count, ಮೌಲ್ಯ_Columns*4)) {
            count+=ಮೌಲ್ಯ_Columns;
            uprv_memmove(pv->v+count, ಅಡ್ಡಸಾಲು+2, (size_t)ಮೌಲ್ಯ_Columns*4);
        }

        ಆದರೆ(ಆರಂಭ<UPVEC_ಮೊದಲ_SPECIAL_CP) {
            handler(context, ಆರಂಭ, ಎಲ್ಲೆ-1, count, pv->v+count, ಮೌಲ್ಯ_Columns, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
        }

        ಅಡ್ಡಸಾಲು+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->ಅಡ್ಡಸಾಲುಗಳು=count/ಮೌಲ್ಯ_Columns+1;
}

U_CAPI ಸ್ಥಿರಾಂಕ uint32_t * U_EXPORT2
upvec_getArray(ಸ್ಥಿರಾಂಕ UPropsVectors *pv, int32_t *p_ಅಡ್ಡಸಾಲುಗಳು, int32_t *pColumns) {
    ಆದರೆ(!pv->isCompacted) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(p_ಅಡ್ಡಸಾಲುಗಳು!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಡ್ಡಸಾಲುಗಳು=pv->ಅಡ್ಡಸಾಲುಗಳು;
    }
    ಆದರೆ(pColumns!=ಶೂನ್ಯಸೂಚಕ) {
        *pColumns=pv->columns-2;
    }
    ಹಿಂತಿರುಗಿ pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ಸ್ಥಿರಾಂಕ UPropsVectors *pv,
                 int32_t *p_ಅಡ್ಡಸಾಲುಗಳು, int32_t *pColumns, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t *clonedArray;
    int32_t byte_ಉದ್ದಳತೆ;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(!pv->isCompacted) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    byte_ಉದ್ದಳತೆ=pv->ಅಡ್ಡಸಾಲುಗಳು*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_ಉದ್ದಳತೆ);
    ಆದರೆ(clonedArray==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(clonedArray, pv->v, byte_ಉದ್ದಳತೆ);
    ಆದರೆ(p_ಅಡ್ಡಸಾಲುಗಳು!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಡ್ಡಸಾಲುಗಳು=pv->ಅಡ್ಡಸಾಲುಗಳು;
    }
    ಆದರೆ(pColumns!=ಶೂನ್ಯಸೂಚಕ) {
        *pColumns=pv->columns-2;
    }
    ಹಿಂತಿರುಗಿ clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_ಗಮ್ಯ_UTrie2_With_ಅಡ್ಡಸಾಲು_Indexes(UPropsVectors *pv, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UPVec_ಗಮ್ಯ_UTrie2_Context ಗಮ್ಯ_UTrie2={ ಶೂನ್ಯಸೂಚಕ, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_ಗಮ್ಯ_UTrie2_Handler, &ಗಮ್ಯ_UTrie2, p_Error_ಸಂಕೇತ);
    utrie2_freeze(ಗಮ್ಯ_UTrie2.trie, UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        utrie2_close(ಗಮ್ಯ_UTrie2.trie);
        ಗಮ್ಯ_UTrie2.trie=ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI ಶೂನ್ಯ U_CALLCONV
upvec_compact_ಗಮ್ಯ_UTrie2_Handler(ಶೂನ್ಯ *context,
                             UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                             int32_t ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ, uint32_t *ಅಡ್ಡಸಾಲು, int32_t columns,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)ಅಡ್ಡಸಾಲು;
    (ಶೂನ್ಯ)columns;
    UPVec_ಗಮ್ಯ_UTrie2_Context *ಗಮ್ಯ_UTrie2=(UPVec_ಗಮ್ಯ_UTrie2_Context *)context;
    ಆದರೆ(ಆರಂಭ<UPVEC_ಮೊದಲ_SPECIAL_CP) {
        utrie2_ಗಣ_Range32(ಗಮ್ಯ_UTrie2->trie, ಆರಂಭ, ಕೊನೆ, (uint32_t)ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ, ನಿಜ, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ(ಆರಂಭ) {
        ಸ್ಥಿತಿ UPVEC_INITIAL_ಮೌಲ್ಯ_CP:
            ಗಮ್ಯ_UTrie2->initial_ಮೌಲ್ಯ=ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UPVEC_ERROR_ಮೌಲ್ಯ_CP:
            ಗಮ್ಯ_UTrie2->error_ಮೌಲ್ಯ=ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UPVEC_ಆರಂಭ_REAL_ಮೌಲ್ಯಗಳು_CP:
            ಗಮ್ಯ_UTrie2->max_ಮೌಲ್ಯ=ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(ಅಡ್ಡಸಾಲು_ಸೂಚ್ಯಂಕ>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಮ್ಯ_UTrie2->trie=utrie2_open(ಗಮ್ಯ_UTrie2->initial_ಮೌಲ್ಯ,
                                           ಗಮ್ಯ_UTrie2->error_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);
            }
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
        }
    }
}
