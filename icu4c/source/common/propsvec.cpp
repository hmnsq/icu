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

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "utrie.h"
#ଅନ୍ତର୍ଭାଵ "utrie2.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "propsvec.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

ସଂରଚନା UPropsVectors {
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

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(ଆକାର(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    ଯଦି(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memset(pv, 0, ଆକାର(UPropsVectors));
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
    ପାଇଁ(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    ଫେରନ୍ତୁ pv;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    ଯଦି(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ସ୍ଥିର uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    ଯଦି (rangeStart >= static_cast<UChar32>(row[0])) {
        ଯଦି (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            ଫେରନ୍ତୁ row;
        } ଅନ୍ୟଥା ଯଦି (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            ଫେରନ୍ତୁ row;
        } ଅନ୍ୟଥା ଯଦି (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            ଫେରନ୍ତୁ row;
        } ଅନ୍ୟଥା ଯଦି ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            କର {
                ++prevRow;
                row+=columns;
            } ଯେତେବେଳେ (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            ଫେରନ୍ତୁ row;
        }
    } ଅନ୍ୟଥା ଯଦି (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        ଫେରନ୍ତୁ pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    ଯେତେବେଳେ(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        ଯଦି (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } ଅନ୍ୟଥା ଯଦି (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            ଫେରନ୍ତୁ row;
        } ଅନ୍ୟଥା {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    ଫେରନ୍ତୁ pv->v+start*columns;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
upvec_set_ମୂଲ୍ୟ(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ;
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
    ଯଦି(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        ଯଦି((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            ଯଦି(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } ଅନ୍ୟଥା ଯଦି(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } ଅନ୍ୟଥା {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                ଫେରନ୍ତୁ;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            ଯଦି(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
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
        ଯଦି(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        ଯଦି(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        ଯଦି(splitLastRow) {
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
    ପାଇଁ(;;) {
        *firstRow=(*firstRow&mask)|value;
        ଯଦି(firstRow==lastRow) {
            ଭାଙ୍ଗ;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_ମୂଲ୍ୟ(ଅଚଳ UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    ଯଦି(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        ଫେରନ୍ତୁ 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    ଫେରନ୍ତୁ row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ଅଚଳ UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    ଯଦି(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        ଫେରନ୍ତୁ nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    ଯଦି(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    ଯଦି(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    ଫେରନ୍ତୁ row+2;
}

ସ୍ଥିର int32_t U_CALLCONV
upvec_compareRows(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *l, ଅଚଳ ଶୂନ୍ୟ *r) {
    ଅଚଳ uint32_t* left = static_cast<ଅଚଳ uint32_t*>(l), *ଡାହାଣ = static_cast<ଅଚଳ uint32_t*>(r);
    ଅଚଳ UPropsVectors* pv = static_cast<ଅଚଳ UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    କର {
        ଯଦି(left[i]!=ଡାହାଣ[i]) {
            ଫେରନ୍ତୁ left[i]<ଡାହାଣ[i] ? -1 : 1;
        }
        ଯଦି(++i==columns) {
            i=0;
        }
    } ଯେତେବେଳେ(--count>0);

    ଫେରନ୍ତୁ 0;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ଶୂନ୍ୟ *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, ମୂଲ୍ୟ_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି(pv->isCompacted) {
        ଫେରନ୍ତୁ;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=ସତ୍ୟ;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    ମୂଲ୍ୟ_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, ମିଥ୍ୟା, pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-ମୂଲ୍ୟ_Columns;
    ପାଇଁ(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        ଯଦି(count<0 || 0!=uprv_memcmp(row+2, row-ମୂଲ୍ୟ_Columns, ମୂଲ୍ୟ_Columns*4)) {
            count+=ମୂଲ୍ୟ_Columns;
        }

        ଯଦି(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, ମୂଲ୍ୟ_Columns, pErrorCode);
            ଯଦି(U_FAILURE(*pErrorCode)) {
                ଫେରନ୍ତୁ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=ମୂଲ୍ୟ_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_ମୂଲ୍ୟଗୁଡ଼ିକ_CP, UPVEC_START_REAL_ମୂଲ୍ୟଗୁଡ଼ିକ_CP,
            count, row-ମୂଲ୍ୟ_Columns, ମୂଲ୍ୟ_Columns, pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-ମୂଲ୍ୟ_Columns;
    ପାଇଁ(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        ଯଦି(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, ମୂଲ୍ୟ_Columns*4)) {
            count+=ମୂଲ୍ୟ_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)ମୂଲ୍ୟ_Columns*4);
        }

        ଯଦି(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, ମୂଲ୍ୟ_Columns, pErrorCode);
            ଯଦି(U_FAILURE(*pErrorCode)) {
                ଫେରନ୍ତୁ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/ମୂଲ୍ୟ_Columns+1;
}

U_CAPI ଅଚଳ uint32_t * U_EXPORT2
upvec_getArray(ଅଚଳ UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    ଯଦି(!pv->isCompacted) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    ଯଦି(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ଫେରନ୍ତୁ pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ଅଚଳ UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    ଯଦି(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    ଯଦି(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    ଯଦି(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ଫେରନ୍ତୁ clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_ମୂଲ୍ୟ_BITS, pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    ଫେରନ୍ତୁ toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI ଶୂନ୍ୟ U_CALLCONV
upvec_compactToUTrie2Handler(ଶୂନ୍ୟ *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)row;
    (ଶୂନ୍ୟ)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    ଯଦି(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, ସତ୍ୟ, pErrorCode);
    } ଅନ୍ୟଥା {
        ବାଛ(start) {
        ସ୍ଥିତି UPVEC_INITIAL_ମୂଲ୍ୟ_CP:
            toUTrie2->initial_ମୂଲ୍ୟ=rowIndex;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UPVEC_ERROR_ମୂଲ୍ୟ_CP:
            toUTrie2->error_ମୂଲ୍ୟ=rowIndex;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UPVEC_START_REAL_ମୂଲ୍ୟଗୁଡ଼ିକ_CP:
            toUTrie2->max_ମୂଲ୍ୟ=rowIndex;
            ଯଦି(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } ଅନ୍ୟଥା {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_ମୂଲ୍ୟ,
                                           toUTrie2->error_ମୂଲ୍ୟ, pErrorCode);
            }
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ;
        }
    }
}
