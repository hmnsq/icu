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

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "propsvec.h"
#अन्तर्भाव "uassert.h"

संरचना UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t max_पङ्क्तिकाः;
    int32_t पङ्क्तिकाः;
    int32_t पूर्व_Row;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#परिभाषय UPVEC_INITIAL_पङ्क्तिकाः (1<<12)
#परिभाषय UPVEC_MEDIUM_पङ्क्तिकाः ((int32_t)1<<16)
#परिभाषय UPVEC_MAX_पङ्क्तिकाः (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UError_संहिता *p_Error_संहिता) {
    UPropsVectors *pv;
    uint32_t *v, *row;
    uint32_t cp;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(columns<1) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(परिमाण(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_पङ्क्तिकाः*columns*4);
    यदि(pv==शून्यसूचकः || v==शून्यसूचकः) {
        uprv_free(pv);
        uprv_free(v);
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memset(pv, 0, परिमाण(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->max_पङ्क्तिकाः=UPVEC_INITIAL_पङ्क्तिकाः;
    pv->पङ्क्तिकाः=2+(UPVEC_MAX_CP-UPVEC_FIRST_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    row=pv->v;
    uprv_memset(row, 0, pv->पङ्क्तिकाः*columns*4);
    row[0]=0;
    row[1]=0x110000;
    row+=columns;
    प्रति(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    प्रत्यागम pv;
}

U_CAPI शून्य U_EXPORT2
upvec_close(UPropsVectors *pv) {
    यदि(pv!=शून्यसूचकः) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

स्थिर uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, सीमा, पूर्व_Row;

    columns=pv->columns;
    सीमा=pv->पङ्क्तिकाः;
    पूर्व_Row=pv->पूर्व_Row;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+पूर्व_Row*columns;
    यदि (rangeStart >= static_cast<UChar32>(row[0])) {
        यदि (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            प्रत्यागम row;
        } अन्यथा यदि (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->पूर्व_Row=पूर्व_Row+1;
            प्रत्यागम row;
        } अन्यथा यदि (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->पूर्व_Row=पूर्व_Row+2;
            प्रत्यागम row;
        } अन्यथा यदि ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            पूर्व_Row+=2;
            कुरु {
                ++पूर्व_Row;
                row+=columns;
            } यावत् (rangeStart >= static_cast<UChar32>(row[1]));
            pv->पूर्व_Row=पूर्व_Row;
            प्रत्यागम row;
        }
    } अन्यथा यदि (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->पूर्व_Row=0;
        प्रत्यागम pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    यावत्(start<सीमा-1) {
        i=(start+सीमा)/2;
        row=pv->v+i*columns;
        यदि (rangeStart < static_cast<UChar32>(row[0])) {
            सीमा=i;
        } अन्यथा यदि (rangeStart < static_cast<UChar32>(row[1])) {
            pv->पूर्व_Row=i;
            प्रत्यागम row;
        } अन्यथा {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->पूर्व_Row=start;
    प्रत्यागम pv->v+start*columns;
}

U_CAPI शून्य U_EXPORT2
upvec_समुच्चयः_मानम्(UPropsVectors *pv,
               UChar32 start, UChar32 अन्तः,
               int32_t column,
               uint32_t value, uint32_t mask,
               UError_संहिता *p_Error_संहिता) {
    uint32_t *firstRow, *चरमः_Row;
    int32_t columns;
    UChar32 सीमा;
    UBool विभज_First_Row, विभज_चरमः_Row;

    /* argument checking */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि( pv==शून्यसूचकः ||
        start<0 || start>अन्तः || अन्तः>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि(pv->isCompacted) {
        *p_Error_संहिता=U_NO_WRITE_PERMISSION;
        प्रत्यागम;
    }
    सीमा=अन्तः+1;

    /* initialize */
    columns=pv->columns;
    column+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    firstRow=_findRow(pv, start);
    चरमः_Row=_findRow(pv, अन्तः);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    विभज_First_Row = start != static_cast<UChar32>(firstRow[0]) && value != (firstRow[column] & mask);
    विभज_चरमः_Row = सीमा != static_cast<UChar32>(चरमः_Row[1]) && value != (चरमः_Row[column] & mask);

    /* split first/last rows if necessary */
    यदि(विभज_First_Row || विभज_चरमः_Row) {
        int32_t count, पङ्क्तिकाः;

        पङ्क्तिकाः=pv->पङ्क्तिकाः;
        यदि((पङ्क्तिकाः+विभज_First_Row+विभज_चरमः_Row)>pv->max_पङ्क्तिकाः) {
            uint32_t *newVectors;
            int32_t new_Max_पङ्क्तिकाः;

            यदि(pv->max_पङ्क्तिकाः<UPVEC_MEDIUM_पङ्क्तिकाः) {
                new_Max_पङ्क्तिकाः=UPVEC_MEDIUM_पङ्क्तिकाः;
            } अन्यथा यदि(pv->max_पङ्क्तिकाः<UPVEC_MAX_पङ्क्तिकाः) {
                new_Max_पङ्क्तिकाः=UPVEC_MAX_पङ्क्तिकाः;
            } अन्यथा {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *p_Error_संहिता=U_आन्तरिकम्_क्रमादेशः_ERROR;
                प्रत्यागम;
            }
            newVectors=(uint32_t *)uprv_malloc(new_Max_पङ्क्तिकाः*columns*4);
            यदि(newVectors==शून्यसूचकः) {
                *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)पङ्क्तिकाः*columns*4);
            firstRow=newVectors+(firstRow-pv->v);
            चरमः_Row=newVectors+(चरमः_Row-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->max_पङ्क्तिकाः=new_Max_पङ्क्तिकाः;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+पङ्क्तिकाः*columns)-(चरमः_Row+columns));
        यदि(count>0) {
            uprv_memmove(
                चरमः_Row+(1+विभज_First_Row+विभज_चरमः_Row)*columns,
                चरमः_Row+columns,
                count*4);
        }
        pv->पङ्क्तिकाः=पङ्क्तिकाः+विभज_First_Row+विभज_चरमः_Row;

        /* split the first row, and move the firstRow pointer to the second part */
        यदि(विभज_First_Row) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((चरमः_Row-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            चरमः_Row+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        यदि(विभज_चरमः_Row) {
            /* copy the last row data */
            uprv_memcpy(चरमः_Row+columns, चरमः_Row, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            चरमः_Row[1]=चरमः_Row[columns]=(uint32_t)सीमा;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->पूर्व_Row=(int32_t)((चरमः_Row-(pv->v))/columns);

    /* set the input value in all remaining rows */
    firstRow+=column;
    चरमः_Row+=column;
    mask=~mask;
    प्रति(;;) {
        *firstRow=(*firstRow&mask)|value;
        यदि(firstRow==चरमः_Row) {
            भङ्ग;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_मानम्(ध्रुव UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    यदि(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        प्रत्यागम 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    प्रत्यागम row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ध्रुव UPropsVectors *pv, int32_t row_सूचकांक,
             UChar32 *pRangeStart, UChar32 *p_Range_अन्तः) {
    uint32_t *row;
    int32_t columns;

    यदि(pv->isCompacted || row_सूचकांक<0 || row_सूचकांक>=pv->पङ्क्तिकाः) {
        प्रत्यागम शून्यसूचकः;
    }

    columns=pv->columns;
    row=pv->v+row_सूचकांक*columns;
    यदि(pRangeStart!=शून्यसूचकः) {
        *pRangeStart=(UChar32)row[0];
    }
    यदि(p_Range_अन्तः!=शून्यसूचकः) {
        *p_Range_अन्तः=(UChar32)row[1]-1;
    }
    प्रत्यागम row+2;
}

स्थिर int32_t U_CALLCONV
upvec_compare_पङ्क्तिकाः(ध्रुव शून्य *context, ध्रुव शून्य *l, ध्रुव शून्य *r) {
    ध्रुव uint32_t* वामतः = static_cast<ध्रुव uint32_t*>(l), *दक्षिणतः = static_cast<ध्रुव uint32_t*>(r);
    ध्रुव UPropsVectors* pv = static_cast<ध्रुव UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    कुरु {
        यदि(वामतः[i]!=दक्षिणतः[i]) {
            प्रत्यागम वामतः[i]<दक्षिणतः[i] ? -1 : 1;
        }
        यदि(++i==columns) {
            i=0;
        }
    } यावत्(--count>0);

    प्रत्यागम 0;
}

U_CAPI शून्य U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, शून्य *context, UError_संहिता *p_Error_संहिता) {
    uint32_t *row;
    int32_t i, columns, मानम्_Columns, पङ्क्तिकाः, count;
    UChar32 start, सीमा;

    /* argument checking */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि(handler==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि(pv->isCompacted) {
        प्रत्यागम;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=सत्य;

    पङ्क्तिकाः=pv->पङ्क्तिकाः;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    मानम्_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_क्रमय_Array(pv->v, पङ्क्तिकाः, columns*4,
                   upvec_compare_पङ्क्तिकाः, pv, असत्य, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-मानम्_Columns;
    प्रति(i=0; i<पङ्क्तिकाः; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        यदि(count<0 || 0!=uprv_memcmp(row+2, row-मानम्_Columns, मानम्_Columns*4)) {
            count+=मानम्_Columns;
        }

        यदि(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, मानम्_Columns, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=मानम्_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_मानानि_CP, UPVEC_START_REAL_मानानि_CP,
            count, row-मानम्_Columns, मानम्_Columns, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-मानम्_Columns;
    प्रति(i=0; i<पङ्क्तिकाः; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        सीमा=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        यदि(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, मानम्_Columns*4)) {
            count+=मानम्_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)मानम्_Columns*4);
        }

        यदि(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, सीमा-1, count, pv->v+count, मानम्_Columns, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->पङ्क्तिकाः=count/मानम्_Columns+1;
}

U_CAPI ध्रुव uint32_t * U_EXPORT2
upvec_getArray(ध्रुव UPropsVectors *pv, int32_t *p_पङ्क्तिकाः, int32_t *pColumns) {
    यदि(!pv->isCompacted) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(p_पङ्क्तिकाः!=शून्यसूचकः) {
        *p_पङ्क्तिकाः=pv->पङ्क्तिकाः;
    }
    यदि(pColumns!=शून्यसूचकः) {
        *pColumns=pv->columns-2;
    }
    प्रत्यागम pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ध्रुव UPropsVectors *pv,
                 int32_t *p_पङ्क्तिकाः, int32_t *pColumns, UError_संहिता *p_Error_संहिता) {
    uint32_t *clonedArray;
    int32_t byte_दीर्घता;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(!pv->isCompacted) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    byte_दीर्घता=pv->पङ्क्तिकाः*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_दीर्घता);
    यदि(clonedArray==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(clonedArray, pv->v, byte_दीर्घता);
    यदि(p_पङ्क्तिकाः!=शून्यसूचकः) {
        *p_पङ्क्तिकाः=pv->पङ्क्तिकाः;
    }
    यदि(pColumns!=शून्यसूचकः) {
        *pColumns=pv->columns-2;
    }
    प्रत्यागम clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compact_गन्तव्यम्_UTrie2_With_Row_Indexes(UPropsVectors *pv, UError_संहिता *p_Error_संहिता) {
    UPVec_गन्तव्यम्_UTrie2_Context गन्तव्यम्_UTrie2={ शून्यसूचकः, 0, 0, 0 };
    upvec_compact(pv, upvec_compact_गन्तव्यम्_UTrie2_Handler, &गन्तव्यम्_UTrie2, p_Error_संहिता);
    utrie2_freeze(गन्तव्यम्_UTrie2.trie, UTRIE2_16_मानम्_द्वयङ्काः, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        utrie2_close(गन्तव्यम्_UTrie2.trie);
        गन्तव्यम्_UTrie2.trie=शून्यसूचकः;
    }
    प्रत्यागम गन्तव्यम्_UTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI शून्य U_CALLCONV
upvec_compact_गन्तव्यम्_UTrie2_Handler(शून्य *context,
                             UChar32 start, UChar32 अन्तः,
                             int32_t row_सूचकांक, uint32_t *row, int32_t columns,
                             UError_संहिता *p_Error_संहिता) {
    (शून्य)row;
    (शून्य)columns;
    UPVec_गन्तव्यम्_UTrie2_Context *गन्तव्यम्_UTrie2=(UPVec_गन्तव्यम्_UTrie2_Context *)context;
    यदि(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_समुच्चयः_Range32(गन्तव्यम्_UTrie2->trie, start, अन्तः, (uint32_t)row_सूचकांक, सत्य, p_Error_संहिता);
    } अन्यथा {
        विकल्प(start) {
        पक्ष UPVEC_INITIAL_मानम्_CP:
            गन्तव्यम्_UTrie2->initial_मानम्=row_सूचकांक;
            भङ्ग;
        पक्ष UPVEC_ERROR_मानम्_CP:
            गन्तव्यम्_UTrie2->error_मानम्=row_सूचकांक;
            भङ्ग;
        पक्ष UPVEC_START_REAL_मानानि_CP:
            गन्तव्यम्_UTrie2->max_मानम्=row_सूचकांक;
            यदि(row_सूचकांक>0xffff) {
                /* too many rows for a 16-bit trie */
                *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            } अन्यथा {
                गन्तव्यम्_UTrie2->trie=utrie2_open(गन्तव्यम्_UTrie2->initial_मानम्,
                                           गन्तव्यम्_UTrie2->error_मानम्, p_Error_संहिता);
            }
            भङ्ग;
        स्वभाव:
            भङ्ग;
        }
    }
}
