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

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "utrie.h"
#అన్తర్భావ "utrie2.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "propsvec.h"
#అన్తర్భావ "uassert.h"

నిర్మాణం UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_అడ్డువరుసలు;
    int32_t అడ్డువరుసలు;
    int32_t prev_అడ్డువరుస;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#నిర్వచించు UPVEC_INITIAL_అడ్డువరుసలు (1<<12)
#నిర్వచించు UPVEC_MEDIUM_అడ్డువరుసలు ((int32_t)1<<16)
#నిర్వచించు UPVEC_MAX_అడ్డువరుసలు (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_సంకేతం *p_Error_సంకేతం) {
    UPropsVectors *pv;
    uint32_t *v, *అడ్డువరుస;
    uint32_t cp;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(columns<1) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(పరిమాణం(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_అడ్డువరుసలు*columns*4);
    ఒకవేళ(pv==శూన్యసూచిక || v==శూన్యసూచిక) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memset(pv, 0, పరిమాణం(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_అడ్డువరుసలు=UPVEC_INITIAL_అడ్డువరుసలు;
    pv->అడ్డువరుసలు=2+(UPVEC_MAX_CP-UPVEC_మొదటి_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    అడ్డువరుస=pv->v;
    uprv_memset(అడ్డువరుస, 0, pv->అడ్డువరుసలు*columns*4);
    అడ్డువరుస[0]=0;
    అడ్డువరుస[1]=0x110000;
    అడ్డువరుస+=columns;
    కొరకు(cp=UPVEC_మొదటి_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        అడ్డువరుస[0]=cp;
        అడ్డువరుస[1]=cp+1;
        అడ్డువరుస+=columns;
    }
    తిరిగి pv;
}

U_CAPI శూన్య U_EXPORT2
upvec_close(UPropsVectors *pv) {
    ఒకవేళ(pv!=శూన్యసూచిక) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

స్థిర uint32_t *
కనుగొను_అడ్డువరుస(UPropsVectors *pv, UChar32 range_ప్రారంభం) {
    uint32_t *అడ్డువరుస;
    int32_t columns, i, ప్రారంభం, హద్దు, prev_అడ్డువరుస;

    columns=pv->columns;
    హద్దు=pv->అడ్డువరుసలు;
    prev_అడ్డువరుస=pv->prev_అడ్డువరుస;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    అడ్డువరుస=pv->v+prev_అడ్డువరుస*columns;
    ఒకవేళ (range_ప్రారంభం >= static_cast<UChar32>(అడ్డువరుస[0])) {
        ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>(అడ్డువరుస[1])) {
            /* same row as last seen */
            తిరిగి అడ్డువరుస;
        } లేకపోతే ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>((అడ్డువరుస += columns)[1])) {
            /* next row after the last one */
            pv->prev_అడ్డువరుస=prev_అడ్డువరుస+1;
            తిరిగి అడ్డువరుస;
        } లేకపోతే ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>((అడ్డువరుస += columns)[1])) {
            /* second row after the last one */
            pv->prev_అడ్డువరుస=prev_అడ్డువరుస+2;
            తిరిగి అడ్డువరుస;
        } లేకపోతే ఒకవేళ ((range_ప్రారంభం - static_cast<UChar32>(అడ్డువరుస[1])) < 10) {
            /* we are close, continue looping */
            prev_అడ్డువరుస+=2;
            చేయి {
                ++prev_అడ్డువరుస;
                అడ్డువరుస+=columns;
            } అయితే (range_ప్రారంభం >= static_cast<UChar32>(అడ్డువరుస[1]));
            pv->prev_అడ్డువరుస=prev_అడ్డువరుస;
            తిరిగి అడ్డువరుస;
        }
    } లేకపోతే ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prev_అడ్డువరుస=0;
        తిరిగి pv->v;
    }

    /* do a binary search for the start of the range */
    ప్రారంభం=0;
    అయితే(ప్రారంభం<హద్దు-1) {
        i=(ప్రారంభం+హద్దు)/2;
        అడ్డువరుస=pv->v+i*columns;
        ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>(అడ్డువరుస[0])) {
            హద్దు=i;
        } లేకపోతే ఒకవేళ (range_ప్రారంభం < static_cast<UChar32>(అడ్డువరుస[1])) {
            pv->prev_అడ్డువరుస=i;
            తిరిగి అడ్డువరుస;
        } లేకపోతే {
            ప్రారంభం=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prev_అడ్డువరుస=ప్రారంభం;
    తిరిగి pv->v+ప్రారంభం*columns;
}

U_CAPI శూన్య U_EXPORT2
upvec_సమితి_విలువ(UPropsVectors *pv,
               UChar32 ప్రారంభం, UChar32 చివర,
               int32_t స్తంభం,
               uint32_t value, uint32_t mask,
               UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t *మొదటి_అడ్డువరుస, *చివరి_అడ్డువరుస;
    int32_t columns;
    UChar32 హద్దు;
    UBool విభజించు_మొదటి_అడ్డువరుస, విభజించు_చివరి_అడ్డువరుస;

    /* argument checking */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ( pv==శూన్యసూచిక ||
        ప్రారంభం<0 || ప్రారంభం>చివర || చివర>UPVEC_MAX_CP ||
        స్తంభం<0 || స్తంభం>=(pv->columns-2)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ(pv->isCompacted) {
        *p_Error_సంకేతం=U_NO_WRITE_PERMISSION;
        తిరిగి;
    }
    హద్దు=చివర+1;

    /* initialize */
    columns=pv->columns;
    స్తంభం+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    మొదటి_అడ్డువరుస=కనుగొను_అడ్డువరుస(pv, ప్రారంభం);
    చివరి_అడ్డువరుస=కనుగొను_అడ్డువరుస(pv, చివర);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    విభజించు_మొదటి_అడ్డువరుస = ప్రారంభం != static_cast<UChar32>(మొదటి_అడ్డువరుస[0]) && value != (మొదటి_అడ్డువరుస[స్తంభం] & mask);
    విభజించు_చివరి_అడ్డువరుస = హద్దు != static_cast<UChar32>(చివరి_అడ్డువరుస[1]) && value != (చివరి_అడ్డువరుస[స్తంభం] & mask);

    /* split first/last rows if necessary */
    ఒకవేళ(విభజించు_మొదటి_అడ్డువరుస || విభజించు_చివరి_అడ్డువరుస) {
        int32_t count, అడ్డువరుసలు;

        అడ్డువరుసలు=pv->అడ్డువరుసలు;
        ఒకవేళ((అడ్డువరుసలు+విభజించు_మొదటి_అడ్డువరుస+విభజించు_చివరి_అడ్డువరుస)>pv->max_అడ్డువరుసలు) {
            uint32_t *newVectors;
            int32_t new_Max_అడ్డువరుసలు;

            ఒకవేళ(pv->max_అడ్డువరుసలు<UPVEC_MEDIUM_అడ్డువరుసలు) {
                new_Max_అడ్డువరుసలు=UPVEC_MEDIUM_అడ్డువరుసలు;
            } లేకపోతే ఒకవేళ(pv->max_అడ్డువరుసలు<UPVEC_MAX_అడ్డువరుసలు) {
                new_Max_అడ్డువరుసలు=UPVEC_MAX_అడ్డువరుసలు;
            } లేకపోతే {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_సంకేతం=U_అంతర్గత_కార్యక్రమం_ERROR;
                తిరిగి;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_అడ్డువరుసలు*columns*4);
            ఒకవేళ(newVectors==శూన్యసూచిక) {
                *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)అడ్డువరుసలు*columns*4);
            మొదటి_అడ్డువరుస=newVectors+(మొదటి_అడ్డువరుస-pv->v);
            చివరి_అడ్డువరుస=newVectors+(చివరి_అడ్డువరుస-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_అడ్డువరుసలు=new_Max_అడ్డువరుసలు;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+అడ్డువరుసలు*columns)-(చివరి_అడ్డువరుస+columns));
        ఒకవేళ(count>0) {
            uprv_memmove(
                చివరి_అడ్డువరుస+(1+విభజించు_మొదటి_అడ్డువరుస+విభజించు_చివరి_అడ్డువరుస)*columns,
                చివరి_అడ్డువరుస+columns,
                count*4);
        }
        pv->అడ్డువరుసలు=అడ్డువరుసలు+విభజించు_మొదటి_అడ్డువరుస+విభజించు_చివరి_అడ్డువరుస;

        /* split the first row, and move the firstRow pointer to the second part */
        ఒకవేళ(విభజించు_మొదటి_అడ్డువరుస) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((చివరి_అడ్డువరుస-మొదటి_అడ్డువరుస)+columns);
            uprv_memmove(మొదటి_అడ్డువరుస+columns, మొదటి_అడ్డువరుస, (size_t)count*4);
            చివరి_అడ్డువరుస+=columns;

            /* split the range and move the firstRow pointer */
            మొదటి_అడ్డువరుస[1]=మొదటి_అడ్డువరుస[columns]=(uint32_t)ప్రారంభం;
            మొదటి_అడ్డువరుస+=columns;
        }

        /* split the last row */
        ఒకవేళ(విభజించు_చివరి_అడ్డువరుస) {
            /* copy the last row data */
            uprv_memcpy(చివరి_అడ్డువరుస+columns, చివరి_అడ్డువరుస, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            చివరి_అడ్డువరుస[1]=చివరి_అడ్డువరుస[columns]=(uint32_t)హద్దు;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prev_అడ్డువరుస=(int32_t)((చివరి_అడ్డువరుస-(pv->v))/columns);

    /* set the input value in all remaining rows */
    మొదటి_అడ్డువరుస+=స్తంభం;
    చివరి_అడ్డువరుస+=స్తంభం;
    mask=~mask;
    కొరకు(;;) {
        *మొదటి_అడ్డువరుస=(*మొదటి_అడ్డువరుస&mask)|value;
        ఒకవేళ(మొదటి_అడ్డువరుస==చివరి_అడ్డువరుస) {
            ఆపు;
        }
        మొదటి_అడ్డువరుస+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_విలువ(స్థిరరాశి UPropsVectors *pv, UChar32 c, int32_t స్తంభం) {
    uint32_t *అడ్డువరుస;
    UPropsVectors *ncpv;

    ఒకవేళ(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || స్తంభం<0 || స్తంభం>=(pv->columns-2)) {
        తిరిగి 0;
    }
    ncpv=(UPropsVectors *)pv;
    అడ్డువరుస=కనుగొను_అడ్డువరుస(ncpv, c);
    తిరిగి అడ్డువరుస[2+స్తంభం];
}

U_CAPI uint32_t * U_EXPORT2
upvec_get_అడ్డువరుస(స్థిరరాశి UPropsVectors *pv, int32_t అడ్డువరుస_సూచీ,
             UChar32 *p_Range_ప్రారంభం, UChar32 *p_Range_చివర) {
    uint32_t *అడ్డువరుస;
    int32_t columns;

    ఒకవేళ(pv->isCompacted || అడ్డువరుస_సూచీ<0 || అడ్డువరుస_సూచీ>=pv->అడ్డువరుసలు) {
        తిరిగి శూన్యసూచిక;
    }

    columns=pv->columns;
    అడ్డువరుస=pv->v+అడ్డువరుస_సూచీ*columns;
    ఒకవేళ(p_Range_ప్రారంభం!=శూన్యసూచిక) {
        *p_Range_ప్రారంభం=(UChar32)అడ్డువరుస[0];
    }
    ఒకవేళ(p_Range_చివర!=శూన్యసూచిక) {
        *p_Range_చివర=(UChar32)అడ్డువరుస[1]-1;
    }
    తిరిగి అడ్డువరుస+2;
}

స్థిర int32_t U_CALLCONV
upvec_compare_అడ్డువరుసలు(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *l, స్థిరరాశి శూన్య *r) {
    స్థిరరాశి uint32_t* ఎడమ = static_cast<స్థిరరాశి uint32_t*>(l), *కుడి = static_cast<స్థిరరాశి uint32_t*>(r);
    స్థిరరాశి UPropsVectors* pv = static_cast<స్థిరరాశి UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    చేయి {
        ఒకవేళ(ఎడమ[i]!=కుడి[i]) {
            తిరిగి ఎడమ[i]<కుడి[i] ? -1 : 1;
        }
        ఒకవేళ(++i==columns) {
            i=0;
        }
    } అయితే(--count>0);

    తిరిగి 0;
}

U_CAPI శూన్య U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, శూన్య *context, UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t *అడ్డువరుస;
    int32_t i, columns, విలువ_Columns, అడ్డువరుసలు, count;
    UChar32 ప్రారంభం, హద్దు;

    /* argument checking */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(handler==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ(pv->isCompacted) {
        తిరిగి;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=నిజం;

    అడ్డువరుసలు=pv->అడ్డువరుసలు;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    విలువ_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_క్రమీకరించు_Array(pv->v, అడ్డువరుసలు, columns*4,
                   upvec_compare_అడ్డువరుసలు, pv, అబద్ధం, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    అడ్డువరుస=pv->v;
    count=-విలువ_Columns;
    కొరకు(i=0; i<అడ్డువరుసలు; ++i) {
        ప్రారంభం=(UChar32)అడ్డువరుస[0];

        /* count a new values vector if it is different from the current one */
        ఒకవేళ(count<0 || 0!=uprv_memcmp(అడ్డువరుస+2, అడ్డువరుస-విలువ_Columns, విలువ_Columns*4)) {
            count+=విలువ_Columns;
        }

        ఒకవేళ(ప్రారంభం>=UPVEC_మొదటి_SPECIAL_CP) {
            handler(context, ప్రారంభం, ప్రారంభం, count, అడ్డువరుస+2, విలువ_Columns, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి;
            }
        }

        అడ్డువరుస+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=విలువ_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_ప్రారంభం_REAL_విలువలు_CP, UPVEC_ప్రారంభం_REAL_విలువలు_CP,
            count, అడ్డువరుస-విలువ_Columns, విలువ_Columns, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    అడ్డువరుస=pv->v;
    count=-విలువ_Columns;
    కొరకు(i=0; i<అడ్డువరుసలు; ++i) {
        /* fetch these first before memmove() may overwrite them */
        ప్రారంభం=(UChar32)అడ్డువరుస[0];
        హద్దు=(UChar32)అడ్డువరుస[1];

        /* add a new values vector if it is different from the current one */
        ఒకవేళ(count<0 || 0!=uprv_memcmp(అడ్డువరుస+2, pv->v+count, విలువ_Columns*4)) {
            count+=విలువ_Columns;
            uprv_memmove(pv->v+count, అడ్డువరుస+2, (size_t)విలువ_Columns*4);
        }

        ఒకవేళ(ప్రారంభం<UPVEC_మొదటి_SPECIAL_CP) {
            handler(context, ప్రారంభం, హద్దు-1, count, pv->v+count, విలువ_Columns, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి;
            }
        }

        అడ్డువరుస+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->అడ్డువరుసలు=count/విలువ_Columns+1;
}

U_CAPI స్థిరరాశి uint32_t * U_EXPORT2
upvec_getArray(స్థిరరాశి UPropsVectors *pv, int32_t *p_అడ్డువరుసలు, int32_t *pColumns) {
    ఒకవేళ(!pv->isCompacted) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(p_అడ్డువరుసలు!=శూన్యసూచిక) {
        *p_అడ్డువరుసలు=pv->అడ్డువరుసలు;
    }
    ఒకవేళ(pColumns!=శూన్యసూచిక) {
        *pColumns=pv->columns-2;
    }
    తిరిగి pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(స్థిరరాశి UPropsVectors *pv,
                 int32_t *p_అడ్డువరుసలు, int32_t *pColumns, UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t *clonedArray;
    int32_t byte_నిడివి;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(!pv->isCompacted) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    byte_నిడివి=pv->అడ్డువరుసలు*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_నిడివి);
    ఒకవేళ(clonedArray==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(clonedArray, pv->v, byte_నిడివి);
    ఒకవేళ(p_అడ్డువరుసలు!=శూన్యసూచిక) {
        *p_అడ్డువరుసలు=pv->అడ్డువరుసలు;
    }
    ఒకవేళ(pColumns!=శూన్యసూచిక) {
        *pColumns=pv->columns-2;
    }
    తిరిగి clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_గమ్యం_UTrie2_With_అడ్డువరుస_Indexes(UPropsVectors *pv, UError_సంకేతం *p_Error_సంకేతం) {
    UPVec_గమ్యం_UTrie2_Context గమ్యం_UTrie2={ శూన్యసూచిక, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_గమ్యం_UTrie2_Handler, &గమ్యం_UTrie2, p_Error_సంకేతం);
    utrie2_freeze(గమ్యం_UTrie2.trie, UTRIE2_16_విలువ_బిట్లు, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        utrie2_close(గమ్యం_UTrie2.trie);
        గమ్యం_UTrie2.trie=శూన్యసూచిక;
    }
    తిరిగి గమ్యం_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI శూన్య U_CALLCONV
upvec_compact_గమ్యం_UTrie2_Handler(శూన్య *context,
                             UChar32 ప్రారంభం, UChar32 చివర,
                             int32_t అడ్డువరుస_సూచీ, uint32_t *అడ్డువరుస, int32_t columns,
                             UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)అడ్డువరుస;
    (శూన్య)columns;
    UPVec_గమ్యం_UTrie2_Context *గమ్యం_UTrie2=(UPVec_గమ్యం_UTrie2_Context *)context;
    ఒకవేళ(ప్రారంభం<UPVEC_మొదటి_SPECIAL_CP) {
        utrie2_సమితి_Range32(గమ్యం_UTrie2->trie, ప్రారంభం, చివర, (uint32_t)అడ్డువరుస_సూచీ, నిజం, p_Error_సంకేతం);
    } లేకపోతే {
        ఎంపిక(ప్రారంభం) {
        స్థితి UPVEC_INITIAL_విలువ_CP:
            గమ్యం_UTrie2->initial_విలువ=అడ్డువరుస_సూచీ;
            ఆపు;
        స్థితి UPVEC_ERROR_విలువ_CP:
            గమ్యం_UTrie2->error_విలువ=అడ్డువరుస_సూచీ;
            ఆపు;
        స్థితి UPVEC_ప్రారంభం_REAL_విలువలు_CP:
            గమ్యం_UTrie2->max_విలువ=అడ్డువరుస_సూచీ;
            ఒకవేళ(అడ్డువరుస_సూచీ>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            } లేకపోతే {
                గమ్యం_UTrie2->trie=utrie2_open(గమ్యం_UTrie2->initial_విలువ,
                                           గమ్యం_UTrie2->error_విలువ, p_Error_సంకేతం);
            }
            ఆపు;
        అప్రమేయ:
            ఆపు;
        }
    }
}
