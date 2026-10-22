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

#شامل <stdlib.h>
#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "utrie.h"
#شامل "utrie2.h"
#شامل "uarrsort.h"
#شامل "propsvec.h"
#شامل "uassert.h"

ساختار UPropsVectors {
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

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }
    اگر(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(اندازه(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    اگر(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memset(pv, 0, اندازه(UPropsVectors));
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
    برای(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    بازگشت pv;
}

U_CAPI تهی U_EXPORT2
upvec_close(UPropsVectors *pv) {
    اگر(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ایستا uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    اگر (rangeStart >= static_cast<UChar32>(row[0])) {
        اگر (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            بازگشت row;
        } وگرنه اگر (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            بازگشت row;
        } وگرنه اگر (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            بازگشت row;
        } وگرنه اگر ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            انجامبده {
                ++prevRow;
                row+=columns;
            } وقتی (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            بازگشت row;
        }
    } وگرنه اگر (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        بازگشت pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    وقتی(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        اگر (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } وگرنه اگر (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            بازگشت row;
        } وگرنه {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    بازگشت pv->v+start*columns;
}

U_CAPI تهی U_EXPORT2
upvec_set_مقدار(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }
    اگر( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        بازگشت;
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
    اگر(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        اگر((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            اگر(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } وگرنه اگر(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } وگرنه {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                بازگشت;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            اگر(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
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
        اگر(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        اگر(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        اگر(splitLastRow) {
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
    برای(;;) {
        *firstRow=(*firstRow&mask)|value;
        اگر(firstRow==lastRow) {
            بشکن;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_مقدار(ثابت UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    اگر(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        بازگشت 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    بازگشت row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ثابت UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    اگر(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        بازگشت nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    اگر(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    اگر(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    بازگشت row+2;
}

ایستا int32_t U_CALLCONV
upvec_compareRows(ثابت تهی *context, ثابت تهی *l, ثابت تهی *r) {
    ثابت uint32_t* left = static_cast<ثابت uint32_t*>(l), *راست = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    انجامبده {
        اگر(left[i]!=راست[i]) {
            بازگشت left[i]<راست[i] ? -1 : 1;
        }
        اگر(++i==columns) {
            i=0;
        }
    } وقتی(--count>0);

    بازگشت 0;
}

U_CAPI تهی U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, تهی *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, مقدار_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }
    اگر(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر(pv->isCompacted) {
        بازگشت;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=درست;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    مقدار_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, نادرست, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-مقدار_Columns;
    برای(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        اگر(count<0 || 0!=uprv_memcmp(row+2, row-مقدار_Columns, مقدار_Columns*4)) {
            count+=مقدار_Columns;
        }

        اگر(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, مقدار_Columns, pErrorCode);
            اگر(U_FAILURE(*pErrorCode)) {
                بازگشت;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=مقدار_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_مقادیر_CP, UPVEC_START_REAL_مقادیر_CP,
            count, row-مقدار_Columns, مقدار_Columns, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-مقدار_Columns;
    برای(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        اگر(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, مقدار_Columns*4)) {
            count+=مقدار_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)مقدار_Columns*4);
        }

        اگر(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, مقدار_Columns, pErrorCode);
            اگر(U_FAILURE(*pErrorCode)) {
                بازگشت;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/مقدار_Columns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    اگر(!pv->isCompacted) {
        بازگشت nullptr;
    }
    اگر(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    اگر(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    بازگشت pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }
    اگر(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    اگر(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    اگر(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    اگر(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    بازگشت clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_مقدار_BITS, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    بازگشت toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI تهی U_CALLCONV
upvec_compactToUTrie2Handler(تهی *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (تهی)row;
    (تهی)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    اگر(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, درست, pErrorCode);
    } وگرنه {
        انتخاب(start) {
        حالت UPVEC_INITIAL_مقدار_CP:
            toUTrie2->initial_مقدار=rowIndex;
            بشکن;
        حالت UPVEC_ERROR_مقدار_CP:
            toUTrie2->error_مقدار=rowIndex;
            بشکن;
        حالت UPVEC_START_REAL_مقادیر_CP:
            toUTrie2->max_مقدار=rowIndex;
            اگر(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } وگرنه {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_مقدار,
                                           toUTrie2->error_مقدار, pErrorCode);
            }
            بشکن;
        پیشفرض:
            بشکن;
        }
    }
}
