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
    int32_t maxRows;
    int32_t rows;
    int32_t prevRow;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#define UPVEC_INITIAL_ROWS (1<<12)
#define UPVEC_MEDIUM_ROWS ((int32_t)1<<16)
#define UPVEC_MAX_ROWS (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UErrorCode *pErrorCode) {
    UPropsVectors *pv;
    uint32_t *v, *row;
    uint32_t cp;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(परिमाण(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    सचे(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    uprv_memset(pv, 0, परिमाण(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->maxRows=UPVEC_INITIAL_ROWS;
    pv->rows=2+(UPVEC_MAX_CP-UPVEC_FIRST_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    row=pv->v;
    uprv_memset(row, 0, pv->rows*columns*4);
    row[0]=0;
    row[1]=0x110000;
    row+=columns;
    अत्थाय(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    पटिनिवत्त pv;
}

U_CAPI शून्य U_EXPORT2
upvec_close(UPropsVectors *pv) {
    सचे(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

स्थिर uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    सचे (rangeStart >= static_cast<UChar32>(row[0])) {
        सचे (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            पटिनिवत्त row;
        } अञ्ञथा सचे (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            पटिनिवत्त row;
        } अञ्ञथा सचे (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            पटिनिवत्त row;
        } अञ्ञथा सचे ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            करोहि {
                ++prevRow;
                row+=columns;
            } याव (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            पटिनिवत्त row;
        }
    } अञ्ञथा सचे (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        पटिनिवत्त pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    याव(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        सचे (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } अञ्ञथा सचे (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            पटिनिवत्त row;
        } अञ्ञथा {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    पटिनिवत्त pv->v+start*columns;
}

U_CAPI शून्य U_EXPORT2
upvec_set_मानम्(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    सचे(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        पटिनिवत्त;
    }
    limit=end+1;

    /* initialize */
    columns=pv->columns;
    column+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    firstRow=_findRow(pv, start);
    lastRow=_findRow(pv, end);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    splitFirstRow = start != static_cast<UChar32>(firstRow[0]) && value != (firstRow[column] & mask);
    splitLastRow = limit != static_cast<UChar32>(lastRow[1]) && value != (lastRow[column] & mask);

    /* split first/last rows if necessary */
    सचे(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        सचे((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            सचे(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } अञ्ञथा सचे(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } अञ्ञथा {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                पटिनिवत्त;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            सचे(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)rows*columns*4);
            firstRow=newVectors+(firstRow-pv->v);
            lastRow=newVectors+(lastRow-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->maxRows=newMaxRows;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+rows*columns)-(lastRow+columns));
        सचे(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        सचे(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        सचे(splitLastRow) {
            /* copy the last row data */
            uprv_memcpy(lastRow+columns, lastRow, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            lastRow[1]=lastRow[columns]=(uint32_t)limit;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prevRow=(int32_t)((lastRow-(pv->v))/columns);

    /* set the input value in all remaining rows */
    firstRow+=column;
    lastRow+=column;
    mask=~mask;
    अत्थाय(;;) {
        *firstRow=(*firstRow&mask)|value;
        सचे(firstRow==lastRow) {
            भङ्ग;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_मानम्(ध्रुव UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    सचे(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        पटिनिवत्त 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    पटिनिवत्त row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ध्रुव UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    सचे(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        पटिनिवत्त nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    सचे(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    सचे(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    पटिनिवत्त row+2;
}

स्थिर int32_t U_CALLCONV
upvec_compareRows(ध्रुव शून्य *context, ध्रुव शून्य *l, ध्रुव शून्य *r) {
    ध्रुव uint32_t* left = static_cast<ध्रुव uint32_t*>(l), *दक्षिणः = static_cast<ध्रुव uint32_t*>(r);
    ध्रुव UPropsVectors* pv = static_cast<ध्रुव UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    करोहि {
        सचे(left[i]!=दक्षिणः[i]) {
            पटिनिवत्त left[i]<दक्षिणः[i] ? -1 : 1;
        }
        सचे(++i==columns) {
            i=0;
        }
    } याव(--count>0);

    पटिनिवत्त 0;
}

U_CAPI शून्य U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, शून्य *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, मानम्_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    सचे(pv->isCompacted) {
        पटिनिवत्त;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=सच्च;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    मानम्_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, मिच्छा, pErrorCode);
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-मानम्_Columns;
    अत्थाय(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        सचे(count<0 || 0!=uprv_memcmp(row+2, row-मानम्_Columns, मानम्_Columns*4)) {
            count+=मानम्_Columns;
        }

        सचे(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, मानम्_Columns, pErrorCode);
            सचे(U_FAILURE(*pErrorCode)) {
                पटिनिवत्त;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=मानम्_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_मानानि_CP, UPVEC_START_REAL_मानानि_CP,
            count, row-मानम्_Columns, मानम्_Columns, pErrorCode);
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
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
    अत्थाय(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        सचे(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, मानम्_Columns*4)) {
            count+=मानम्_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)मानम्_Columns*4);
        }

        सचे(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, मानम्_Columns, pErrorCode);
            सचे(U_FAILURE(*pErrorCode)) {
                पटिनिवत्त;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/मानम्_Columns+1;
}

U_CAPI ध्रुव uint32_t * U_EXPORT2
upvec_getArray(ध्रुव UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    सचे(!pv->isCompacted) {
        पटिनिवत्त nullptr;
    }
    सचे(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    सचे(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    पटिनिवत्त pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ध्रुव UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    सचे(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    सचे(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    सचे(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    पटिनिवत्त clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_मानम्_BITS, pErrorCode);
    सचे(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    पटिनिवत्त toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI शून्य U_CALLCONV
upvec_compactToUTrie2Handler(शून्य *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (शून्य)row;
    (शून्य)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    सचे(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, सच्च, pErrorCode);
    } अञ्ञथा {
        विकल्प(start) {
        पक्ष UPVEC_INITIAL_मानम्_CP:
            toUTrie2->initial_मानम्=rowIndex;
            भङ्ग;
        पक्ष UPVEC_ERROR_मानम्_CP:
            toUTrie2->error_मानम्=rowIndex;
            भङ्ग;
        पक्ष UPVEC_START_REAL_मानानि_CP:
            toUTrie2->max_मानम्=rowIndex;
            सचे(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } अञ्ञथा {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_मानम्,
                                           toUTrie2->error_मानम्, pErrorCode);
            }
            भङ्ग;
        स्वभाव:
            भङ्ग;
        }
    }
}
