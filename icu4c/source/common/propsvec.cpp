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

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "utrie.h"
#அந்தர்பாவ "utrie2.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "propsvec.h"
#அந்தர்பாவ "uassert.h"

கட்டமைப்பு UPropsVectors {
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

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(அளவு(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    எனில்(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memset(pv, 0, அளவு(UPropsVectors));
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
    க்கு(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    திரும்பு pv;
}

U_CAPI வெறுமை U_EXPORT2
upvec_close(UPropsVectors *pv) {
    எனில்(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

நிலையான uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    எனில் (rangeStart >= static_cast<UChar32>(row[0])) {
        எனில் (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            திரும்பு row;
        } இல்லையெனில் எனில் (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            திரும்பு row;
        } இல்லையெனில் எனில் (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            திரும்பு row;
        } இல்லையெனில் எனில் ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            செய் {
                ++prevRow;
                row+=columns;
            } போது (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            திரும்பு row;
        }
    } இல்லையெனில் எனில் (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        திரும்பு pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    போது(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        எனில் (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } இல்லையெனில் எனில் (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            திரும்பு row;
        } இல்லையெனில் {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    திரும்பு pv->v+start*columns;
}

U_CAPI வெறுமை U_EXPORT2
upvec_set_மதிப்பு(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    எனில்( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில்(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        திரும்பு;
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
    எனில்(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        எனில்((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            எனில்(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } இல்லையெனில் எனில்(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } இல்லையெனில் {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                திரும்பு;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            எனில்(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
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
        எனில்(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        எனில்(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        எனில்(splitLastRow) {
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
    க்கு(;;) {
        *firstRow=(*firstRow&mask)|value;
        எனில்(firstRow==lastRow) {
            நிறுத்து;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_மதிப்பு(மாறிலி UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    எனில்(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        திரும்பு 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    திரும்பு row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(மாறிலி UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    எனில்(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        திரும்பு nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    எனில்(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    எனில்(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    திரும்பு row+2;
}

நிலையான int32_t U_CALLCONV
upvec_compareRows(மாறிலி வெறுமை *context, மாறிலி வெறுமை *l, மாறிலி வெறுமை *r) {
    மாறிலி uint32_t* left = static_cast<மாறிலி uint32_t*>(l), *வலது = static_cast<மாறிலி uint32_t*>(r);
    மாறிலி UPropsVectors* pv = static_cast<மாறிலி UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    செய் {
        எனில்(left[i]!=வலது[i]) {
            திரும்பு left[i]<வலது[i] ? -1 : 1;
        }
        எனில்(++i==columns) {
            i=0;
        }
    } போது(--count>0);

    திரும்பு 0;
}

U_CAPI வெறுமை U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, வெறுமை *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, மதிப்பு_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    எனில்(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில்(pv->isCompacted) {
        திரும்பு;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=உண்மை;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    மதிப்பு_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, பொய், pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-மதிப்பு_Columns;
    க்கு(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        எனில்(count<0 || 0!=uprv_memcmp(row+2, row-மதிப்பு_Columns, மதிப்பு_Columns*4)) {
            count+=மதிப்பு_Columns;
        }

        எனில்(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, மதிப்பு_Columns, pErrorCode);
            எனில்(U_FAILURE(*pErrorCode)) {
                திரும்பு;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=மதிப்பு_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_மதிப்புகள்_CP, UPVEC_START_REAL_மதிப்புகள்_CP,
            count, row-மதிப்பு_Columns, மதிப்பு_Columns, pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-மதிப்பு_Columns;
    க்கு(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        எனில்(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, மதிப்பு_Columns*4)) {
            count+=மதிப்பு_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)மதிப்பு_Columns*4);
        }

        எனில்(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, மதிப்பு_Columns, pErrorCode);
            எனில்(U_FAILURE(*pErrorCode)) {
                திரும்பு;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/மதிப்பு_Columns+1;
}

U_CAPI மாறிலி uint32_t * U_EXPORT2
upvec_getArray(மாறிலி UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    எனில்(!pv->isCompacted) {
        திரும்பு nullptr;
    }
    எனில்(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    எனில்(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    திரும்பு pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(மாறிலி UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    எனில்(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    எனில்(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    எனில்(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    திரும்பு clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_மதிப்பு_BITS, pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    திரும்பு toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI வெறுமை U_CALLCONV
upvec_compactToUTrie2Handler(வெறுமை *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (வெறுமை)row;
    (வெறுமை)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    எனில்(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, உண்மை, pErrorCode);
    } இல்லையெனில் {
        தேர்வு(start) {
        நிலை UPVEC_INITIAL_மதிப்பு_CP:
            toUTrie2->initial_மதிப்பு=rowIndex;
            நிறுத்து;
        நிலை UPVEC_ERROR_மதிப்பு_CP:
            toUTrie2->error_மதிப்பு=rowIndex;
            நிறுத்து;
        நிலை UPVEC_START_REAL_மதிப்புகள்_CP:
            toUTrie2->max_மதிப்பு=rowIndex;
            எனில்(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } இல்லையெனில் {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_மதிப்பு,
                                           toUTrie2->error_மதிப்பு, pErrorCode);
            }
            நிறுத்து;
        இயல்பு:
            நிறுத்து;
        }
    }
}
