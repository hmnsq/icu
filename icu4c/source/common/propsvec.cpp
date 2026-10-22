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

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "utrie.h"
#അന്തര്ഭാവ "utrie2.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "propsvec.h"
#അന്തര്ഭാവ "uassert.h"

ഘടന UPropsVectors {
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

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(വലുപ്പം(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    എങ്കിൽ(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memset(pv, 0, വലുപ്പം(UPropsVectors));
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
    വേണ്ടി(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    തിരികെ pv;
}

U_CAPI ശൂന്യം U_EXPORT2
upvec_close(UPropsVectors *pv) {
    എങ്കിൽ(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

സ്ഥിതം uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    എങ്കിൽ (rangeStart >= static_cast<UChar32>(row[0])) {
        എങ്കിൽ (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            തിരികെ row;
        } അല്ലെങ്കിൽ എങ്കിൽ (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            തിരികെ row;
        } അല്ലെങ്കിൽ എങ്കിൽ (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            തിരികെ row;
        } അല്ലെങ്കിൽ എങ്കിൽ ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            ചെയ്യുക {
                ++prevRow;
                row+=columns;
            } ആകുമ്പോൾ (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            തിരികെ row;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        തിരികെ pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    ആകുമ്പോൾ(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        എങ്കിൽ (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } അല്ലെങ്കിൽ എങ്കിൽ (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            തിരികെ row;
        } അല്ലെങ്കിൽ {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    തിരികെ pv->v+start*columns;
}

U_CAPI ശൂന്യം U_EXPORT2
upvec_set_മൂല്യം(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    എങ്കിൽ(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        തിരികെ;
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
    എങ്കിൽ(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        എങ്കിൽ((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            എങ്കിൽ(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } അല്ലെങ്കിൽ എങ്കിൽ(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } അല്ലെങ്കിൽ {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                തിരികെ;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            എങ്കിൽ(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
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
        എങ്കിൽ(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        എങ്കിൽ(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        എങ്കിൽ(splitLastRow) {
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
    വേണ്ടി(;;) {
        *firstRow=(*firstRow&mask)|value;
        എങ്കിൽ(firstRow==lastRow) {
            നിർത്തുക;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_മൂല്യം(സ്ഥിരം UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    എങ്കിൽ(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        തിരികെ 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    തിരികെ row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(സ്ഥിരം UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    എങ്കിൽ(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        തിരികെ nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    എങ്കിൽ(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    എങ്കിൽ(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    തിരികെ row+2;
}

സ്ഥിതം int32_t U_CALLCONV
upvec_compareRows(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *l, സ്ഥിരം ശൂന്യം *r) {
    സ്ഥിരം uint32_t* left = static_cast<സ്ഥിരം uint32_t*>(l), *വലത് = static_cast<സ്ഥിരം uint32_t*>(r);
    സ്ഥിരം UPropsVectors* pv = static_cast<സ്ഥിരം UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    ചെയ്യുക {
        എങ്കിൽ(left[i]!=വലത്[i]) {
            തിരികെ left[i]<വലത്[i] ? -1 : 1;
        }
        എങ്കിൽ(++i==columns) {
            i=0;
        }
    } ആകുമ്പോൾ(--count>0);

    തിരികെ 0;
}

U_CAPI ശൂന്യം U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, ശൂന്യം *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, മൂല്യം_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    എങ്കിൽ(pv->isCompacted) {
        തിരികെ;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=സത്യം;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    മൂല്യം_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, അസത്യം, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-മൂല്യം_Columns;
    വേണ്ടി(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        എങ്കിൽ(count<0 || 0!=uprv_memcmp(row+2, row-മൂല്യം_Columns, മൂല്യം_Columns*4)) {
            count+=മൂല്യം_Columns;
        }

        എങ്കിൽ(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, മൂല്യം_Columns, pErrorCode);
            എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                തിരികെ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=മൂല്യം_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_മൂല്യങ്ങൾ_CP, UPVEC_START_REAL_മൂല്യങ്ങൾ_CP,
            count, row-മൂല്യം_Columns, മൂല്യം_Columns, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-മൂല്യം_Columns;
    വേണ്ടി(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        എങ്കിൽ(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, മൂല്യം_Columns*4)) {
            count+=മൂല്യം_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)മൂല്യം_Columns*4);
        }

        എങ്കിൽ(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, മൂല്യം_Columns, pErrorCode);
            എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                തിരികെ;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/മൂല്യം_Columns+1;
}

U_CAPI സ്ഥിരം uint32_t * U_EXPORT2
upvec_getArray(സ്ഥിരം UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    എങ്കിൽ(!pv->isCompacted) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    എങ്കിൽ(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    തിരികെ pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(സ്ഥിരം UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    എങ്കിൽ(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    എങ്കിൽ(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    എങ്കിൽ(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    തിരികെ clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_മൂല്യം_BITS, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    തിരികെ toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI ശൂന്യം U_CALLCONV
upvec_compactToUTrie2Handler(ശൂന്യം *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (ശൂന്യം)row;
    (ശൂന്യം)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    എങ്കിൽ(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, സത്യം, pErrorCode);
    } അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ്(start) {
        അവസ്ഥ UPVEC_INITIAL_മൂല്യം_CP:
            toUTrie2->initial_മൂല്യം=rowIndex;
            നിർത്തുക;
        അവസ്ഥ UPVEC_ERROR_മൂല്യം_CP:
            toUTrie2->error_മൂല്യം=rowIndex;
            നിർത്തുക;
        അവസ്ഥ UPVEC_START_REAL_മൂല്യങ്ങൾ_CP:
            toUTrie2->max_മൂല്യം=rowIndex;
            എങ്കിൽ(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } അല്ലെങ്കിൽ {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_മൂല്യം,
                                           toUTrie2->error_മൂല്യം, pErrorCode);
            }
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക;
        }
    }
}
