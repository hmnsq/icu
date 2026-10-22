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

ಸಂರಚನಾ UPropsVectors {
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

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(ಪರಿಮಾಣ(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    ಯದಿ(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memset(pv, 0, ಪರಿಮಾಣ(UPropsVectors));
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
    ಪ್ರತಿ(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    ಪ್ರತ್ಯಾಗಮ pv;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    ಯದಿ(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ಸ್ಥಿರ uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    ಯದಿ (rangeStart >= static_cast<UChar32>(row[0])) {
        ಯದಿ (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            ಪ್ರತ್ಯಾಗಮ row;
        } ಅನ್ಯಥಾ ಯದಿ (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            ಪ್ರತ್ಯಾಗಮ row;
        } ಅನ್ಯಥಾ ಯದಿ (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            ಪ್ರತ್ಯಾಗಮ row;
        } ಅನ್ಯಥಾ ಯದಿ ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            ಕುರು {
                ++prevRow;
                row+=columns;
            } ಯಾವತ್ (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            ಪ್ರತ್ಯಾಗಮ row;
        }
    } ಅನ್ಯಥಾ ಯದಿ (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        ಪ್ರತ್ಯಾಗಮ pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    ಯಾವತ್(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        ಯದಿ (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } ಅನ್ಯಥಾ ಯದಿ (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            ಪ್ರತ್ಯಾಗಮ row;
        } ಅನ್ಯಥಾ {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    ಪ್ರತ್ಯಾಗಮ pv->v+start*columns;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_set_ಮೌಲ್ಯ(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ಪ್ರತ್ಯಾಗಮ;
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
    ಯದಿ(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        ಯದಿ((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            ಯದಿ(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } ಅನ್ಯಥಾ ಯದಿ(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } ಅನ್ಯಥಾ {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            ಯದಿ(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
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
        ಯದಿ(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        ಯದಿ(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        ಯದಿ(splitLastRow) {
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
    ಪ್ರತಿ(;;) {
        *firstRow=(*firstRow&mask)|value;
        ಯದಿ(firstRow==lastRow) {
            ಭಙ್ಗ;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_ಮೌಲ್ಯ(ಧ್ರುವ UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    ಯದಿ(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    ಪ್ರತ್ಯಾಗಮ row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ಧ್ರುವ UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    ಯದಿ(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    ಯದಿ(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    ಯದಿ(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    ಪ್ರತ್ಯಾಗಮ row+2;
}

ಸ್ಥಿರ int32_t U_CALLCONV
upvec_compareRows(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *l, ಧ್ರುವ ಶೂನ್ಯ *r) {
    ಧ್ರುವ uint32_t* left = static_cast<ಧ್ರುವ uint32_t*>(l), *ಬಲ = static_cast<ಧ್ರುವ uint32_t*>(r);
    ಧ್ರುವ UPropsVectors* pv = static_cast<ಧ್ರುವ UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    ಕುರು {
        ಯದಿ(left[i]!=ಬಲ[i]) {
            ಪ್ರತ್ಯಾಗಮ left[i]<ಬಲ[i] ? -1 : 1;
        }
        ಯದಿ(++i==columns) {
            i=0;
        }
    } ಯಾವತ್(--count>0);

    ಪ್ರತ್ಯಾಗಮ 0;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ಶೂನ್ಯ *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, ಮೌಲ್ಯ_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(pv->isCompacted) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=ಸತ್ಯ;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    ಮೌಲ್ಯ_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, ಅಸತ್ಯ, pErrorCode);
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-ಮೌಲ್ಯ_Columns;
    ಪ್ರತಿ(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        ಯದಿ(count<0 || 0!=uprv_memcmp(row+2, row-ಮೌಲ್ಯ_Columns, ಮೌಲ್ಯ_Columns*4)) {
            count+=ಮೌಲ್ಯ_Columns;
        }

        ಯದಿ(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, ಮೌಲ್ಯ_Columns, pErrorCode);
            ಯದಿ(U_FAILURE(*pErrorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=ಮೌಲ್ಯ_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_ಮೌಲ್ಯಗಳು_CP, UPVEC_START_REAL_ಮೌಲ್ಯಗಳು_CP,
            count, row-ಮೌಲ್ಯ_Columns, ಮೌಲ್ಯ_Columns, pErrorCode);
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-ಮೌಲ್ಯ_Columns;
    ಪ್ರತಿ(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        ಯದಿ(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, ಮೌಲ್ಯ_Columns*4)) {
            count+=ಮೌಲ್ಯ_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)ಮೌಲ್ಯ_Columns*4);
        }

        ಯದಿ(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, ಮೌಲ್ಯ_Columns, pErrorCode);
            ಯದಿ(U_FAILURE(*pErrorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/ಮೌಲ್ಯ_Columns+1;
}

U_CAPI ಧ್ರುವ uint32_t * U_EXPORT2
upvec_getArray(ಧ್ರುವ UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    ಯದಿ(!pv->isCompacted) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    ಯದಿ(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ಪ್ರತ್ಯಾಗಮ pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ಧ್ರುವ UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    ಯದಿ(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    ಯದಿ(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    ಯದಿ(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ಪ್ರತ್ಯಾಗಮ clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_ಮೌಲ್ಯ_BITS, pErrorCode);
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI ಶೂನ್ಯ U_CALLCONV
upvec_compactToUTrie2Handler(ಶೂನ್ಯ *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)row;
    (ಶೂನ್ಯ)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    ಯದಿ(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, ಸತ್ಯ, pErrorCode);
    } ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ(start) {
        ಪಕ್ಷ UPVEC_INITIAL_ಮೌಲ್ಯ_CP:
            toUTrie2->initial_ಮೌಲ್ಯ=rowIndex;
            ಭಙ್ಗ;
        ಪಕ್ಷ UPVEC_ERROR_ಮೌಲ್ಯ_CP:
            toUTrie2->error_ಮೌಲ್ಯ=rowIndex;
            ಭಙ್ಗ;
        ಪಕ್ಷ UPVEC_START_REAL_ಮೌಲ್ಯಗಳು_CP:
            toUTrie2->max_ಮೌಲ್ಯ=rowIndex;
            ಯದಿ(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } ಅನ್ಯಥಾ {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_ಮೌಲ್ಯ,
                                           toUTrie2->error_ಮೌಲ್ಯ, pErrorCode);
            }
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ;
        }
    }
}
